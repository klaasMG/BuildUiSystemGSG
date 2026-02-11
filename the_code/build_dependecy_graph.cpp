#include "TokeniserGMAKE.h"
#include "SimpleASTGMAKE.h"
#include "GMAKE_EXEPTION.h"
#include "GMakeTypes.h"
#include "file_utils.h"
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <memory>
#include <ranges>
#include <unordered_set>
#define PRINT(text) std::cout << text << std::endl

namespace fs = std::filesystem;

fs::path current_dir;
bool debug = false;
GMAKE_EXCEPTION ExceptionHandler = GMAKE_EXCEPTION{debug};
std::unordered_set<std::string_view> allowed_flags = {"-debug",};

GMAKEConfig runGMAKEFunction(const std::string& function_name, const std::vector<std::string>& function_args, GMAKEConfig config) {
    ExceptionHandler.add_to_call_stack(function_name);
	switch (parseFunction(function_name)) {
	case GMakeFunction::SET_PROJECT_DIRECTORY: {
			fs::path project_dir = function_args[0];

			if (project_dir.is_absolute()) {
				config.ProjectDir = project_dir;
			}
			else {
				config.ProjectDir = (current_dir / project_dir);
			}
			break;
	}

	case GMakeFunction::SET_PROGRAM: {
			const std::string& shader_program = function_args[0];
			std::vector<fs::path> shaders;
			for (const std::string& arg : function_args | std::views::drop(1)) {
				fs::path path_arg = arg;
				shaders.emplace_back(path_arg);
			}
			config.ShaderPrograms[shader_program] = shaders;
			break;
	}

	case GMakeFunction::EXTEND_STANDARD:{
	    for (const std::string arg : function_args){
	        config.StandardExtensions.emplace_back(arg);
	    }
	    break;
	}

	case GMakeFunction::UNKNOWN:
		ExceptionHandler.send(1, "Function is not found" + function_name);
		break;
	}

	return config;
}

std::vector<std::unique_ptr<ASTNode>> build_ast(const std::string& gmake_file){
	TokeniserGMAKE tokeniser(gmake_file);
	std::vector<Token> tokens = tokeniser.Tokenise();
	ASTGMAKE ast_builder(tokens);
	std::vector<std::unique_ptr<ASTNode>> nodes = ast_builder.getNodes();
	return nodes;
}

std::string do_includes(const std::string& shader, std::map<fs::path, std::string>& open_shaders, const GMAKEConfig &config){
	std::istringstream stream(shader);
	std::string line;
	std::string rebuild;
	while (getline(stream, line)){
		std::string new_line;
		if (line.starts_with("#include")) {
			// Extract the filename from #include "filename" or #include <filename>
			size_t first_quote = line.find('"');
			size_t last_quote = line.rfind('"');

			// Handle both "filename" and <filename> formats
			if (first_quote == std::string::npos) {
				first_quote = line.find('<');
				last_quote = line.rfind('>');
			}

			if (first_quote != std::string::npos && last_quote != std::string::npos && first_quote != last_quote) {
				std::string include_path = line.substr(first_quote + 1, last_quote - first_quote - 1);
				fs::path shader_path(include_path);

				if (shader_path.is_absolute()){
					new_line = ReadFilePath(shader_path);
				}
				else{
					fs::path shader_path_comb = config.ProjectDir / shader_path;
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
		rebuild = do_includes(rebuild, open_shaders, config);
	}
	return rebuild;
}

void include_run(const fs::path& shader_directory, const GMAKEConfig &config){
	std::map<fs::path, std::string> open_shader_files;
	std::map<fs::path, std::string> open_include_files;

	fs::path new_dir = config.ProjectDir.parent_path() / "preprocessed_shaders";
	if (!fs::exists(new_dir)) {
		fs::create_directory(new_dir);
	}

	for (const std::pair<const std::string, std::vector<fs::path>>& shader : config.ShaderPrograms) {
		std::vector<fs::path> shaders = shader.second;
		for (const fs::path &file : shaders){
			fs::path actual_file_path;

			if (file.is_absolute()) {
				actual_file_path = file;
			} else {
				actual_file_path = config.ProjectDir / file;
			}

			std::string shader_content = ReadFilePath(actual_file_path);
		    for (const fs::path& standard_path : config.StandardExtensions){
		        std::string path_string = standard_path.string();
		        std::string standard_file_path_include = "#include " + path_string;
		        shader_content = insertLine(shader_content, 1, standard_file_path_include);
		    }
			std::string included_shader = do_includes(shader_content, open_shader_files, config);

			fs::path output_file = new_dir / file.filename();

			open_include_files[output_file] = included_shader;
		}
	}

	for (const std::pair<const fs::path, std::string> &write_file : open_include_files) {
		PRINT("Writing to: " << write_file.first);
		WriteFile(write_file.first, write_file.second);
	}
}

std::vector<std::string> make_args(const std::vector<IdentNode>& args){
	std::vector<std::string> arg_string;
	for (const IdentNode& arg : args){
		arg_string.push_back(arg.Ident);
	}
	return arg_string;
}

int main(int argc, char* argv[]) {
	if (argc >= 2){
	    current_dir = fs::current_path();
	    std::cout << current_dir << std::endl;
	    char* gmake_file_path = argv[1];
	    std::string gmake_file = readFile(gmake_file_path);
	    std::vector<std::unique_ptr<ASTNode>> nodes = build_ast(gmake_file);
	    GMAKEConfig config = GMAKEConfig();
	    std::vector<std::string> flags;
	    for (int i = 2; i < argc; i++){
	        const std::string& arg = argv[i];
	        if (arg == "-debug"){
	            config.debug = true;
	        }
	        else if (!allowed_flags.contains(arg)){
	            std::string error_message = "This flag: " + arg + " is not allowed\n" + "Do you wish to proceed?(Y/N)";
	            std::cout << error_message << std::endl;
	            std::string continue_program;
	            std::cin >> continue_program;
	            continue_program = toLower(continue_program);
	            bool is_solved = false;
	            while (!is_solved){
	                if (continue_program == "y"){
	                    is_solved = true;
	                }
	                else if (continue_program == "n"){
	                    const int& exit_code = 1;
	                    is_solved = true;
	                    std::exit(exit_code);
	                }
	            }
	            flags.push_back(arg);
	        }
	    }
	    if (config.debug){
	        ExceptionHandler.set_debug(true);
	    }
	    for (const std::unique_ptr<ASTNode>& node : nodes){
	        if (dynamic_cast<FunctionNode*>(node.get())){
                auto function = dynamic_cast<FunctionNode*>(node.get());
	            IdentNode function_name = function->Ident;
	            std::string name_check = function_name.Ident;
	            std::vector<IdentNode> function_args = function->Args;
	            std::vector<std::string> Args = make_args(function_args);
	            config = runGMAKEFunction(name_check, Args, config);
	        }
	    }
	    std::cout << config.ProjectDir << std::endl;
	    include_run("path", config);
	}
	else{
		std::cout << "wrong number of arguments" << std::endl;
	}

	return 0;
}