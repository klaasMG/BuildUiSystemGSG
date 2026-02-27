#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H
#include <atomic>
#include <map>
#include <string>
#include <vector>

enum class TokenType{
    ENDMARKER,
    DEDENT,
    INDENT,
    SEMICOLON,
    COMMA,
    COLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACK,
    RBRACK,
    ELLIPSIS,
    DOT,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
    VBAR,
    AMPER,
    CIRCUMFLEX,
    TILDE,
    AT,
    EQEQUAL,
    NOTEQUAL,
    LESSEQUAL,
    GREATEREQUAL,
    LEFTSHIFT,
    RIGHTSHIFT,
    DOUBLESTAR,
    DOUBLESLASH,
    PLUSEQUAL,
    MINEQUAL,
    STAREQUAL,
    SLASHEQUAL,
    PERCENTEQUAL,
    AMPEREQUAL,
    VBAREQUAL,
    CIRCUMFLEXEQUAL,
    LEFTSHIFTEQUAL,
    RIGHTSHIFTEQUAL,
    DOUBLESTAREQUAL,
    DOUBLESLASHEQUAL,
    ATEQUAL,
    RARROW,
    COLONEQUAL,
    LESS,
    GREATER,
    EQUAL,
    EXCLAMATION,
    NUMBER,
    STRING,
    KEYWORD,
    IDENT,
    COMMENT,
    TYPE_COMMENT,
    FSTRING_START,
    FSTRING_MIDDLE,
    FSTRING_END,
    NEWLINE,
    NL,
    TYPE_IGNORE,
    SOFT_KEYWORD,
};

const std::map<TokenType, std::string> token_type_char = {
    {TokenType::SEMICOLON, ";"},
    {TokenType::COMMA, ","},
    {TokenType::COLON, ":"},

    {TokenType::LPAREN, "("},
    {TokenType::RPAREN, ")"},
    {TokenType::LBRACE, "{"},
    {TokenType::RBRACE, "}"},
    {TokenType::LBRACK, "["},
    {TokenType::RBRACK, "]"},

    {TokenType::DOT, "."},
    {TokenType::ELLIPSIS, "..."},

    {TokenType::PLUS, "+"},
    {TokenType::MINUS, "-"},
    {TokenType::STAR, "*"},
    {TokenType::SLASH, "/"},
    {TokenType::PERCENT, "%"},
    {TokenType::VBAR, "|"},
    {TokenType::AMPER, "&"},
    {TokenType::CIRCUMFLEX, "^"},
    {TokenType::TILDE, "~"},
    {TokenType::AT, "@"},

    {TokenType::LESS, "<"},
    {TokenType::GREATER, ">"},
    {TokenType::EQUAL, "="},
    {TokenType::EXCLAMATION, "!"},

    {TokenType::EQEQUAL, "=="},
    {TokenType::NOTEQUAL, "!="},
    {TokenType::LESSEQUAL, "<="},
    {TokenType::GREATEREQUAL, ">="},

    {TokenType::LEFTSHIFT, "<<"},
    {TokenType::RIGHTSHIFT, ">>"},
    {TokenType::DOUBLESTAR, "**"},
    {TokenType::DOUBLESLASH, "//"},

    {TokenType::PLUSEQUAL, "+="},
    {TokenType::MINEQUAL, "-="},
    {TokenType::STAREQUAL, "*="},
    {TokenType::SLASHEQUAL, "/="},
    {TokenType::PERCENTEQUAL, "%="},
    {TokenType::AMPEREQUAL, "&="},
    {TokenType::VBAREQUAL, "|="},
    {TokenType::CIRCUMFLEXEQUAL, "^="},
    {TokenType::LEFTSHIFTEQUAL, "<<="},
    {TokenType::RIGHTSHIFTEQUAL, ">>="},
    {TokenType::DOUBLESTAREQUAL, "**="},
    {TokenType::DOUBLESLASHEQUAL, "//="},
    {TokenType::ATEQUAL, "@="},

    {TokenType::RARROW, "->"},
    {TokenType::COLONEQUAL, ":="}};

struct token{
    TokenType type;
    std::string value;
};
class Tokeniser{
public:
    Tokeniser(){
        token = 0;
        text = std::string();
    }

    std::vector<token> tokenize(const std::string& input_text){
        text = input_text;
        while (text.size() < token){

        }
        reset_tokeniser();
        return {};
    }
private:
    uint64_t token = 0;
    std::string text = std::string();
    void reset_tokeniser(){
        text = std::string();
        token = 0;
    };
    char peek_token(){
        if (token >= text.size()) return '\0';
        return text[token];
    }
    char next_token(){
        char c = peek_token();
        token++;
        return c;
    }
};

#endif //SUPERBUILD_TOKENISER_H