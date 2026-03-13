#include <iostream>
#include <vector>
#include "Tokeniser.h"
#include "AST_old.h"

enum class token_type{
    ARG,
    FLAG,
};

struct ArgToken{
    std::string name;
    token_type type;
};

int main(int argc, char* argv[]) {
    std::vector<ArgToken> tokens;
    std::vector<ArgToken> flags;
    for (int i = 0; i < argc; i++) {
        ArgToken arg_token = ArgToken();
        char* arg = argv[i];
        std::string arg_str = arg;
        arg_token.name = arg_str;
        if (arg_str.starts_with('-')){
            arg_token.type = token_type::FLAG;
            flags.push_back(arg_token);
        }
        else{
            arg_token.type = token_type::ARG;
            tokens.push_back(arg_token);
        }
    }

    return 0;
}
