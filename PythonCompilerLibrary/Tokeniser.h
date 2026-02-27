#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H
#include <atomic>
#include <string>
#include <vector>

enum class TokenType{

};

struct token{
    TokenType type;
    std::string value;
};
class Tokeniser{
public:
    std::vector<token> tokenize(const std::string& text){
        reset_tokeniser();
        return {};
    }
private:
    uint64_t token = 0;
    void reset_tokeniser(){
        token = 0;
    };
    char peek_token(){}
    char next_token(){}
};

#endif //SUPERBUILD_TOKENISER_H