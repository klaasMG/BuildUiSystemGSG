//
// Created by klaas on 2/8/2026.
//

#ifndef EVENT_STRUCT_TOKENS_H
#define EVENT_STRUCT_TOKENS_H

#include <string>

enum class TokenType : char{
    Identifier = 0,
    Number = 1,
    LeftBracket = '(',
    RightBracket = ')',
    Comma = ',',
    Semicolon = ';',
    Slash = '/',
    None = '\0'
};



struct Token{
    TokenType type;
    std::string value;
};

#endif //EVENT_STRUCT_TOKENS_H