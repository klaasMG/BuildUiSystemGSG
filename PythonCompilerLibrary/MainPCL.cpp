#include <vector>
#include <fstream>
#include <iostream>
#include "GMakePythonParser.h"
#include "Tokeniser.h"

enum class ArgTokenType{
    ARG,
    FLAG,
};

struct ArgToken{
    std::string name;
    ArgTokenType type;
};

inline std::string readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) return "";

    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.seekg(0);

    std::string data(size, '\0');
    file.read(data.data(), size);

    return data;
}

std::string keywordToString(KeyWord kw) {
    for (const auto& [str, value] : keyword_char) {
        if (value == kw) {
            return str;
        }
    }
    return "";
}

int main(int argc, char* argv[]){
    std::vector<ArgToken> arg_tokens;
    std::vector<ArgToken> flags;
    for (int i = 0; i < argc; i++){
        ArgToken arg_token = ArgToken();
        char* arg = argv[i];
        std::string arg_str = arg;
        arg_token.name = arg_str;
        if (arg_str.starts_with('-')){
            arg_token.type = ArgTokenType::FLAG;
            flags.push_back(arg_token);
        }
        else{
            arg_token.type = ArgTokenType::ARG;
            arg_tokens.push_back(arg_token);
        }
    }
    Tokeniser tokeniser = Tokeniser();
    std::string file_data = readFile(arg_tokens[1].name.c_str());
    std::vector<token> tokens = tokeniser.tokenise(file_data);

    return 0;
}
