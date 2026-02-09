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
    TokeniserGMAKE(const std::string& input)
    {
        file = input;
        TokenPos = 0;
        none_symbols = {TokenType::None, TokenType::Identifier, TokenType::Number,};
        symbols = getOtherSymbols(none_symbols);
    }

    std::vector<Token> Tokenise()
    {
        std::vector<Token> tokens;
        while(TokenPos < file.length())
        {
            if (is_alphabet()){
                std::string ident = "";
                char c = consume_char();
                ident.push_back(c);
                while (is_alphabet()){
                    c = consume_char();
                    ident.push_back(c);
                }
                tokens.push_back(Token(TokenType::Identifier, ident));
            }
            else if (is_digit()){
                std::string num = "";
                char c = consume_char();
                num.push_back(c);
                while (is_digit()){
                    c = consume_char();
                    num.push_back(c);
                }
                tokens.push_back(Token(TokenType::Number, num));
            }
            else{
                TokenType t = get_token_type();
                char c = consume_char();
                if (t != TokenType::None){
                    tokens.push_back(Token(t, ""));
                }
            }
        }
        for (const auto& token : tokens){
            std::cerr << "token " << token.value << std::endl;
        }
        return tokens;
    }
private:
    char consume_char(){
        char c = peek_char();
        TokenPos++;
        return c;
    }

    char peek_char(){
        if (TokenPos <= file.length()){
            return file[TokenPos];
        }
        return '\0';
    }

    bool is_alphabet(){
        char c = peek_char();
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    bool is_digit(){
        char c = peek_char();
        return (c >= '0' && c <= '9');
    }

    TokenType get_token_type(){
        char c = peek_char();
        for (const auto& symbol : symbols){
            char value_symbol = static_cast<char>(symbol);
            if (value_symbol == c){
                return symbol;
            }
        }
        return TokenType::None;
    }

    static std::vector<TokenType> getOtherSymbols(const std::vector<TokenType>& exclude) {
        std::vector<TokenType> all = {
            TokenType::Identifier, TokenType::Number, TokenType::LeftBracket,
            TokenType::RightBracket, TokenType::Comma, TokenType::Semicolon, TokenType::None, TokenType::Slash,
        };
        std::vector<TokenType> result;
        for (const auto& token : all) {
            if (std::find(exclude.begin(), exclude.end(), token) == exclude.end()) {
                result.push_back(token);
            }
        }
        return result;
    }

};

#endif //EVENT_STRUCT_TOKENISER_H