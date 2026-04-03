#ifndef EVENT_STRUCT_TOKENISER_H
#define EVENT_STRUCT_TOKENISER_H
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ostream>

#include "Tokens.h"

class TokeniserGMAKE{
    std::string file;
    int TokenPos;
    std::vector<TokenType> none_symbols;
    std::vector<TokenType> symbols;
public:
    TokeniserGMAKE(const std::string& input);

    std::vector<Token> Tokenise();
private:
    char consume_char();
    char peek_char();
    bool is_alphabet();
    bool is_digit();
    TokenType get_token_type();
    static std::vector<TokenType> getOtherSymbols(const std::vector<TokenType>& exclude);
};

#endif
