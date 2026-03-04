#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <map>

enum class KeyWord{
    FALSE,
    NONE,
    TRUE,
    AND,
    AS,
    ASSERT,
    ASYNC,
    AWAIT,
    BREAK,
    CLASS,
    CONTINUE,
    DEF,
    DEL,
    ELIF,
    ELSE,
    EXCEPT,
    FINALLY,
    FOR,
    FROM,
    GLOBAL,
    IF,
    IMPORT,
    IN,
    IS,
    LAMBDA,
    NONLOCAL,
    NOT,
    OR,
    PASS,
    RAISE,
    RETURN,
    TRY,
    WHILE,
    WITH,
    YIELD,
    CASE,
    MATCH,
    TYPE,
    EMPTY,
};

const std::map<std::string, KeyWord> keyword_char = {
    {"",KeyWord::EMPTY},
    {"False", KeyWord::FALSE},
    {"None", KeyWord::NONE},
    {"True", KeyWord::TRUE},

    {"and", KeyWord::AND},
    {"as", KeyWord::AS},
    {"assert", KeyWord::ASSERT},
    {"async", KeyWord::ASYNC},
    {"await", KeyWord::AWAIT},
    {"break", KeyWord::BREAK},
    {"class", KeyWord::CLASS},
    {"continue", KeyWord::CONTINUE},
    {"def", KeyWord::DEF},
    {"del", KeyWord::DEL},
    {"elif", KeyWord::ELIF},
    {"else", KeyWord::ELSE},
    {"except", KeyWord::EXCEPT},
    {"finally", KeyWord::FINALLY},
    {"for", KeyWord::FOR},
    {"from", KeyWord::FROM},
    {"global", KeyWord::GLOBAL},
    {"if", KeyWord::IF},
    {"import", KeyWord::IMPORT},
    {"in", KeyWord::IN},
    {"is", KeyWord::IS},
    {"lambda", KeyWord::LAMBDA},
    {"nonlocal", KeyWord::NONLOCAL},
    {"not", KeyWord::NOT},
    {"or", KeyWord::OR},
    {"pass", KeyWord::PASS},
    {"raise", KeyWord::RAISE},
    {"return", KeyWord::RETURN},
    {"try", KeyWord::TRY},
    {"while", KeyWord::WHILE},
    {"with", KeyWord::WITH},
    {"yield", KeyWord::YIELD},
    {"case", KeyWord::CASE},
    {"match", KeyWord::MATCH},
    {"type", KeyWord::TYPE}
};

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
    std::variant<std::string, KeyWord> value;
};

struct literal_token{
    bool sucses = false;
    token token;
};

class Tokeniser{
public:
    std::vector<token> tokenise(){
        std::vector<token> tokens;
        while (peektoken() != '\0'){

        }
        return {};
    }
private:
    uint64_t TokenPos = 0;
    std::string Text = std::string();
    std::vector<int> IdentStack = {0};
    std::vector<char> BrackectStack = {};
    char peektoken(){
        if (Text.size() > TokenPos) return '\0';
        char c = Text[TokenPos];
        return c;
    }

    char nexttoken(){
        char c = peektoken();
        TokenPos++;
        return c;
    }

    void reset_tokeniser(){
        Text = std::string();
        TokenPos = 0;
        IdentStack = {0};
        BrackectStack = {};
    }
};

#endif //SUPERBUILD_TOKENISER_H