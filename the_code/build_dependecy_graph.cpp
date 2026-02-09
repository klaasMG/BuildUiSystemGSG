//
// Created by klaas on 2/5/2026.
//
#include "TokeniserGMAKE.h"
#include "SimpleASTGMAKE.h"
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include <ranges>
#include "pybind11/detail/descr.h"

using namespace std;
namespace fs = std::filesystem;

fs::path current_dir;

struct GMAKEConfig {
	string ProjectDir;
	unordered_map<string, vector<string>> ShaderPrograms;
};

string readFile(const char* path)
{
	ifstream file(path, ios::binary);
	if (!file) return "";

	file.seekg(0, ios::end);
	size_t size = file.tellg();
	file.seekg(0);

	string data(size, '\0');
	file.read(data.data(), size);

	return data;
}

vector<string> preprocessArgs(const vector<vector<IdentNode>>& Args) {
	vector<string> result;

	for (const auto& argVec : Args) {
		if (argVec.size() == 1) {
			// Single IdentNode - get its string
			result.push_back(argVec[0].Ident);  // or whatever the string member is
		} else if (argVec.size() > 1) {
			// Multiple IdentNodes - concatenate first and last
			string combined = argVec.front().Ident + argVec.back().Ident;
			result.push_back(combined);
		}
		// If argVec is empty, we skip it
	}

	return result;
}

GMAKEConfig runGMAKEFunction(const string& function_name, const vector<string>& function_args, GMAKEConfig config) {
	if (function_name == "SetProjectDirectory"){
		const string current_dir_str = current_dir.string();
		const string project_dir = function_args[0];
		string project_shader_program;
		if (!project_dir.empty() && project_dir[0] == '/'){
			project_shader_program = current_dir_str + "/" + project_dir;
		}
		else{
			project_shader_program = current_dir_str + project_dir;
		}
		config.ProjectDir = project_shader_program;
	}
	else if (function_name == "SetProgram"){
		const string shader_program = function_args[0];
		vector<string> shaders;
		for (const string arg : function_args | std::views::drop(1)) {
			shaders.push_back(arg);
		}
		config.ShaderPrograms[shader_program] = shaders;
	}

	return config;
}

vector<unique_ptr<ASTNode>> build_ast(const string& gmake_file){
	TokeniserGMAKE tokeniser(gmake_file);
	vector<Token> tokens = tokeniser.Tokenise();
	ASTGMAKE ast_builder(tokens);
	vector<unique_ptr<ASTNode>> nodes = ast_builder.getNodes();
	return nodes;
}

int main(int argc, char* argv[]) {
	if (argc == 2)
	{
		current_dir = fs::current_path();
		char* gmake_file_path = argv[1];

		string gmake_file = readFile(gmake_file_path);
		vector<unique_ptr<ASTNode>> nodes = build_ast(gmake_file);

		GMAKEConfig config = GMAKEConfig();

		for (const auto& node : nodes){
			if (dynamic_cast<FunctionNode*>(node.get())){
				auto function = dynamic_cast<FunctionNode*>(node.get());
				IdentNode function_name = function->Ident;
				string name_check = function_name.Ident;
				auto function_args = function->Args;
				vector<string> Args = preprocessArgs(function_args);
				config = runGMAKEFunction(name_check, Args, config);
			}
		}

	}
	else
	{
		cout << "wrong number of arguments" << endl;
	}

	return 0;
}