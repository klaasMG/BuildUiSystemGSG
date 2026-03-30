#include <assert.h>
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
#include <windows.h>
#define PRINT(text) std::cout << text << std::endl
#define ASSERT_MSG(cond, msg)                         \
do {                                              \
    if (!(cond)) {                                \
        std::cerr << "Assertion failed: " << msg   \
        << "\nFile: " << __FILE__       \
        << "\nLine: " << __LINE__       \
        << std::endl;                  \
        std::abort();                             \
    }                                             \
    } while (0)
namespace fs = std::filesystem;

fs::path current_dir;
bool debug = false;
GMAKE_EXCEPTION ExceptionHandler = GMAKE_EXCEPTION{debug};
std::unordered_set<std::string_view> allowed_flags = {"-debug",};

std::string run_command(const fs::path& cmd_path) {
    // mutable command buffer
    std::string cmd = cmd_path.string();
    std::vector<char> cmd_buf(cmd.begin(), cmd.end());
    cmd_buf.push_back('\0');

    // pipe
    HANDLE readPipe = NULL, writePipe = NULL;
    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    CreatePipe(&readPipe, &writePipe, &sa, 0);

    // make sure read end is NOT inherited
    SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);

    // startup info
    STARTUPINFOA si{};
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdOutput = writePipe;
    si.hStdError  = writePipe;
    si.hStdInput  = NULL;

    PROCESS_INFORMATION pi{};

    // create process
    if (!CreateProcessA(
        NULL,
        cmd_buf.data(),
        NULL, NULL,
        TRUE,
        0,
        NULL, NULL,
        &si, &pi
    )) {
        CloseHandle(readPipe);
        CloseHandle(writePipe);
        return "";
    }

    // parent doesn't need write end
    CloseHandle(writePipe);

    // read output
    std::string output;
    char buffer[4096];
    DWORD bytesRead;

    while (true) {
        BOOL success = ReadFile(readPipe, buffer, sizeof(buffer), &bytesRead, NULL);
        if (!success || bytesRead == 0) break;
        output.append(buffer, bytesRead);
    }

    // wait for process
    WaitForSingleObject(pi.hProcess, INFINITE);

    // cleanup
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(readPipe);

    return output;
}

std::string trim(std::string s) {
    // left trim
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch) { return !std::isspace(ch); }));

    // right trim
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());

    return s;
}

std::pair<std::string, std::string> split_once(const std::string& s, char delim) {
    size_t pos = s.find(delim);
    if (pos == std::string::npos)
        return {s, ""};

    return {
        s.substr(0, pos),
        s.substr(pos + 1)
    };
}

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
	    for (const std::string& arg : function_args){
	        config.StandardExtensions.emplace_back(arg);
	    }
	    break;
	}

	case GMakeFunction::SSBO_LAYOUT_BINDING:{
	    PRINT(function_args.size());
        if (function_args.empty()){
            ExceptionHandler.error(2,"No program given");
        }
        const std::string& program_name = function_args[0];

        std::cerr << "SSBO layout binding: " << std::to_string(program_name.size()) << std::endl;
        PRINT(program_name);
        fs::path path_program_build_ssbo_layout = config.ProjectDir / program_name;
	    std::string output = run_command(path_program_build_ssbo_layout);

	    PRINT("this is the output:" + output);

	    std::istringstream stream(output);
	    std::string line;
	    std::map<std::string, std::map<std::string, uint64_t>> mappings;
	    bool has_pending = false;
	    while (true) {
	        if (!has_pending) {
	            if (!std::getline(stream, line)){break;}
	        }
	        else {
	            has_pending = false;
	        }
	        line = trim(line);
	        std::pair<std::string, std::string> key_value = split_once(line, ':');

	        if (key_value.first == "header" && !isdigit(key_value.second[0])) {
	            std::string key_to_mapping = key_value.second;
	            std::map<std::string, uint64_t> mapping;

	            while (std::getline(stream, line)) {
	                line = trim(line);
	                std::pair<std::string, std::string> kv = split_once(line, ':');

	                if (kv.first == "header" && !isdigit(kv.second[0])) {
	                    has_pending = true; // reuse this line in outer loop
	                    break;
	                }
	                std::string key = kv.first;
	                key = trim(key);
	                uint64_t value = std::stoull(kv.second);
                    if (mapping.contains(key)){
                        ExceptionHandler.error(2,"Key already exists");
                    }
	                mapping.insert_or_assign(key, value);
	            }
	            mappings.insert_or_assign(key_to_mapping, mapping);
	        }
	    }
	    config.SSBO_key_to_value = mappings;
	    break;
	}

	case GMakeFunction::UNKNOWN:
		ExceptionHandler.error(1, "Function is not found" + function_name);
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

	fs::path new_dir = config.ProjectDir.parent_path() / "preprocessed_shader";//preprocessed_shaders
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

struct SSBOBlock {
    std::string text;
    size_t start;
    size_t end; // one past the last character (like substr)
};

std::vector<SSBOBlock> extractSSBOs(const std::string& src) {
    std::vector<SSBOBlock> result;
    size_t pos = 0;

    while ((pos = src.find("layout(", pos)) != std::string::npos) {
        size_t start = pos;

        // --- match layout(...) ---
        size_t i = pos + 7;
        int parenDepth = 1;

        while (i < src.size() && parenDepth > 0) {
            if (src[i] == '(') parenDepth++;
            else if (src[i] == ')') parenDepth--;
            i++;
        }
        if (parenDepth != 0) break;

        // skip whitespace
        size_t after = src.find_first_not_of(" \t\r\n", i);

        // must be "buffer"
        if (after == std::string::npos ||
            src.compare(after, 6, "buffer") != 0) {
            pos = i;
            continue;
            }

        // find '{'
        size_t braceStart = src.find('{', after);
        if (braceStart == std::string::npos) break;

        // --- match { ... } ---
        size_t j = braceStart + 1;
        int braceDepth = 1;

        while (j < src.size() && braceDepth > 0) {
            if (src[j] == '{') braceDepth++;
            else if (src[j] == '}') braceDepth--;
            j++;
        }
        if (braceDepth != 0) break;

        // find ';' after closing '}'
        size_t semicolon = src.find(';', j);
        if (semicolon == std::string::npos) break;

        size_t end = semicolon + 1;

        result.push_back({
            src.substr(start, end - start),
            start,
            end
        });

        pos = end;
    }

    return result;
}

std::string replace_first(const std::string& input,const std::string& sub,const std::string& repl){
    std::string s = input;
    size_t pos = s.find(sub);
    if (pos != std::string::npos) {
        s.replace(pos, sub.length(), repl);
    }
    return s;
}

char nextNonSpace(const std::string& s, size_t pos) {
    size_t i = pos + 1;  // start after current char
    while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) {
        ++i;
    }
    return (i < s.size()) ? s[i] : '\0'; // '\0' if none found
}
void run_layout_bindings(const GMAKEConfig &config){
    for (const std::pair<const std::string, std::vector<fs::path>>& shader : config.ShaderPrograms){
        std::vector<fs::path> shaders = shader.second;
        for (const fs::path& file : shaders){
            fs::path actual_file_path;
            actual_file_path = config.ProjectDir.parent_path() / "preprocessed_shader" / file.filename();
            std::string shader_content = ReadFilePath(actual_file_path);
            std::vector<SSBOBlock> ssbo_blocks = extractSSBOs(shader_content);
            for ( SSBOBlock& ssbo_block : ssbo_blocks){
                std::string ssbo_content = ssbo_block.text;
                std::string target = "binding";
                size_t pos = 0;
                pos = ssbo_content.find(target);
                uint64_t target_lenght = 7;
                ASSERT_MSG(pos != std::string::npos, "binding must be in the return of find ssbo this is a bug");
                size_t binding_pos = ssbo_content.find("binding");
                ASSERT_MSG(binding_pos != std::string::npos, "binding not found");

                size_t eq_pos = ssbo_content.find('=', binding_pos);
                ASSERT_MSG(eq_pos != std::string::npos, "binding missing '='");

                // find first non-space after '='
                size_t i = eq_pos + 1;
                while (i < ssbo_content.size() && std::isspace(static_cast<unsigned char>(ssbo_content[i]))) {
                    i++;
                }

                if (i >= ssbo_content.size()) {
                    continue;
                }

                // ✅ STOP if numeric binding
                if (std::isdigit(static_cast<unsigned char>(ssbo_content[i]))) {
                    PRINT("Numeric binding found, skipping");
                    continue;
                }

                // ✅ Parse symbolic binding
                if (std::isalpha(static_cast<unsigned char>(ssbo_content[i])) || static_cast<unsigned char>(ssbo_content[i]) == '_') {

                    std::string header_name;
                    while (i < ssbo_content.size() && std::isalpha(static_cast<unsigned char>(ssbo_content[i])) || static_cast<unsigned char>(ssbo_content[i]) == '_') {
                        header_name += ssbo_content[i++];
                    }

                    PRINT("Header: " + header_name);

                    if (i >= ssbo_content.size() || ssbo_content[i] != '.') {
                        ExceptionHandler.error(4, "Expected '.' after header");
                    }

                    i++; // skip '.'

                    std::string attribute;
                    while (i < ssbo_content.size() && std::isalpha(static_cast<unsigned char>(ssbo_content[i])) || static_cast<unsigned char>(ssbo_content[i]) == '_') {
                        attribute += ssbo_content[i++];
                    }

                    PRINT("Attribute: " + attribute);

                    auto& mapping = config.SSBO_key_to_value.at(header_name);
                    uint64_t value = mapping.at(attribute);

                    std::string full_expr = header_name + "." + attribute;

                    ssbo_content = replace_first(ssbo_content, full_expr, std::to_string(value));
                    shader_content = replace_first(shader_content, ssbo_block.text, ssbo_content);
                }
                PRINT("gh");
                fs::path parent_actual_file_path = config.ProjectDir.parent_path();
                WriteFile(parent_actual_file_path / "preprocessed_shader" / file, shader_content); //preprocessed_shaders
            }
        }
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
	    run_layout_bindings(config);
	    //ssbo_layout_bindings();
	}
	else{
		std::cout << "wrong number of arguments" << std::endl;
	}

	return 0;
}