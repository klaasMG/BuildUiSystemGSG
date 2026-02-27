#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H
#include <string>
#include <vector>

enum class TokenType{

};

struct token{
    TokenType type;
    std::string value;
};

std::vector<token> inline tokenize(const std::string& text){
    return {};
}

#endif //SUPERBUILD_TOKENISER_H