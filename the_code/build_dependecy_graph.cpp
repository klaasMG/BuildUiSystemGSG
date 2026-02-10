//
// Created by klaas on 2/5/2026.
//
#include "TokeniserGMAKE.h"
#include "SimpleASTGMAKE.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <ranges>
#define PRINT(text) cout << text << endl

using namespace std;
namespace fs = std::filesystem;

fs::path current_dir;

struct GMAKEConfig {
	fs::path ProjectDir;
	map<string, vector<fs::path>> ShaderPrograms;
};

string readFile(const char* path) {
	ifstream file(path, ios::binary);
	if (!file) return "";

	file.seekg(0, ios::end);
	size_t size = file.tellg();
	file.seekg(0);

	string data(size, '\0');
	file.read(data.data(), size);

	return data;
}

void WriteFile(const fs::path& filepath, const std::string& content) {
	std::ofstream file(filepath);
	file << content;
	file.close();  // Optional, destructor closes it
}

string ReadFilePath(const fs::path& path) {
	ifstream file(path, ios::binary);
	if (!file) return "";

	file.seekg(0, ios::end);
	size_t size = file.tellg();
	file.seekg(0);

	string data(size, '\0');
	file.read(data.data(), size);

	return data;
}

GMAKEConfig runGMAKEFunction(const string& function_name, const vector<string>& function_args, GMAKEConfig config) {
	if (function_name == "SetProjectDirectory"){
		fs::path project_dir = function_args[0];

		if (project_dir.is_absolute()) {
			config.ProjectDir = project_dir;
		}
		else {
			config.ProjectDir = (current_dir / project_dir);
		}
	}
	else if (function_name == "SetProgram"){
		string shader_program = function_args[0];
		vector<fs::path> shaders;
		for (const string& arg : function_args | std::views::drop(1)) {
			shaders.push_back(fs::path(arg));
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

string do_includes(string shader, map<fs::path, string>& open_shaders){
	istringstream stream(shader);
	string line;
	string rebuild;
	while (getline(stream, line)){
		string new_line = "";
		if (line.starts_with("#include")) {
			// Extract the filename from #include "filename" or #include <filename>
			size_t first_quote = line.find('"');
			size_t last_quote = line.rfind('"');

			// Handle both "filename" and <filename> formats
			if (first_quote == string::npos) {
				first_quote = line.find('<');
				last_quote = line.rfind('>');
			}

			if (first_quote != string::npos && last_quote != string::npos && first_quote != last_quote) {
				string include_path = line.substr(first_quote + 1, last_quote - first_quote - 1);
				fs::path shader_path(include_path);

				if (shader_path.is_absolute()){
					new_line = ReadFilePath(shader_path);
				}
				else{
					fs::path shader_path_comb = current_dir / shader_path;
					new_line = ReadFilePath(shader_path_comb);
				}
			}
		}
		else{
			new_line = line;
		}
		rebuild.append(new_line);
		rebuild.append("\n");  // Add newline back
	}

	if (rebuild.contains("#include")){
		rebuild = do_includes(rebuild, open_shaders);
	}
	return rebuild;
}

void include_run(const fs::path& shader_directory, const GMAKEConfig &config){
	map<fs::path, string> open_shader_files;
	map<fs::path, string> open_include_files;

	// Create GMakeDir in the current directory
	fs::path new_dir = current_dir / "GMakeDir";
	if (!fs::exists(new_dir)) {
		fs::create_directory(new_dir);
	}

	for (const pair<const string, vector<fs::path>>& shader : config.ShaderPrograms) {
		vector<fs::path> shaders = shader.second;
		for (const fs::path &file : shaders){
			fs::path actual_file_path;

			// Determine the actual file path
			if (file.is_absolute()) {
				actual_file_path = file;
			} else {
				actual_file_path = config.ProjectDir / file;
			}

			PRINT(actual_file_path);

			// Read and process the shader
			string shader_content = ReadFilePath(actual_file_path);
			string included_shader = do_includes(shader_content, open_shader_files);

			// Create output path in GMakeDir
			fs::path output_file = new_dir / file.filename();

			open_include_files[output_file] = included_shader;
		}
	}

	// Write all processed files
	for (const pair<const fs::path, string> &write_file : open_include_files) {
		PRINT("Writing to: " << write_file.first);
		WriteFile(write_file.first, write_file.second);
	}
}

vector<string> make_args(const vector<IdentNode>& args){
	vector<string> arg_string;
	for (const IdentNode& arg : args){
		arg_string.push_back(arg.Ident);
	}
	return arg_string;
}

int main(int argc, char* argv[]) {
	if (argc == 2){
		current_dir = fs::current_path();
		cout << current_dir << endl;
		char* gmake_file_path = argv[1];
		string gmake_file = readFile(gmake_file_path);
		vector<unique_ptr<ASTNode>> nodes = build_ast(gmake_file);
		GMAKEConfig config = GMAKEConfig();

		for (const unique_ptr<ASTNode>& node : nodes){
			if (dynamic_cast<FunctionNode*>(node.get())){
				auto function = dynamic_cast<FunctionNode*>(node.get());
				IdentNode function_name = function->Ident;
				string name_check = function_name.Ident;
				vector<IdentNode> function_args = function->Args;
				vector<string> Args = make_args(function_args);
				for (string i : Args){
					cout << i << endl;
				}
				config = runGMAKEFunction(name_check, Args, config);
			}
		}
		cout << config.ProjectDir << endl;
		include_run("path", config);
	}
	else{
		cout << "wrong number of arguments" << endl;
	}

	return 0;
}