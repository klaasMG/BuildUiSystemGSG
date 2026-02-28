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

struct literal_token{
    bool sucses = false;
    token token;
};

class Tokeniser{
public:
    Tokeniser(const std::map<TokenType, std::string>& token_type_char_in){
        token_pos = 0;
        text = std::string();
        token_type_char = token_type_char_in;
    }

    std::vector<token> tokenize(const std::string& input_text){
        text = input_text;
        std::vector<token> tokens;
        while (text.size() < token_pos){
            literal_token token_literal = get_literal_token();
            if (token_literal.sucses){
                tokens.push_back(token_literal.token);
            }
        }
        reset_tokeniser();
        return {};
    }

private:
    std::map<TokenType, std::string> token_type_char;
    uint64_t token_pos = 0;
    std::string text = std::string();
    void reset_tokeniser(){
        text = std::string();
        token_pos = 0;
    };
    char peek_token(){
        if (token_pos >= text.size()) return '\0';
        return text[token_pos];
    }
    char next_token(){
        char c = peek_token();
        token_pos++;
        return c;
    }

    literal_token get_literal_token(){
        literal_token token_literal;
        std::string literal;
        char c = peek_token();
        literal += c;
        for (std::pair<TokenType, std::string> token_type : token_type_char){
            if (literal == token_type.second){
                next_token();
                token_literal.sucses = true;
                token tok = {token_type.first, ""};
                token_literal.token = tok;
                return token_literal;
            }
        }
        char c1 = peek_token();
        literal += c1;
        for (std::pair<TokenType, std::string> token_type : token_type_char){
            if (literal == token_type.second){
                next_token();
                token_literal.sucses = true;
                token tok = {token_type.first, ""};
                token_literal.token = tok;
                return token_literal;
            }
        }
        char c2 = peek_token();
        literal += c2;
        for (std::pair<TokenType, std::string> token_type : token_type_char){
            if (literal == token_type.second){
                next_token();
                token_literal.sucses = true;
                token tok = {token_type.first, ""};
                token_literal.token = tok;
                return token_literal;
            }
        }
        literal_token tok_literal = literal_token();
        tok_literal.sucses = false;
        tok_literal.token = {};
        return tok_literal;
    }

};

#endif //SUPERBUILD_TOKENISER_H