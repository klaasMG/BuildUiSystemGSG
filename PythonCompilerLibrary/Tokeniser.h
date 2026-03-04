#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <array>

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

struct string_type{
    bool f_string = false;
    bool b_string = false;
    bool r_string = false;
};

class Tokeniser{
public:
    std::vector<token> tokenise(){
        std::vector<token> tokens;
        while (peektoken() != '\0'){
            if (char c = peektoken();std::isalpha(c)){
                std::string ident = ident_getter();
                if (peektoken() == '"' || peektoken() == '\''){
                    char end_char = nexttoken();
                    parse_string(end_char, string_type_parser(ident), tokens);
                }
            }
        }
        return {};
    }
private:
    uint64_t TokenPos = 0;
    std::string Text = std::string();
    std::vector<int> IdentStack = {0};
    std::vector<char> BrackectStack = {};

    std::string ident_getter(){
        std::string ident = std::string();
        while (std::isalnum(peektoken()) || peektoken() == '_'){
            char c = nexttoken();
            ident.push_back(c);
        }
        return ident;
    }

    void parse_string(char end_char, string_type str_type, std::vector<token>& tokens){
        std::string string = std::string();
        bool is_multiline = false;
        if (peektoken() == end_char){
            nexttoken();
            if (peektoken() == end_char){
                nexttoken();
                is_multiline = true;
            }
            else{ token tok; tok.type = TokenType::STRING; tok.value = ""; }
        }
        token tok;
        tok.type = TokenType::STRING;
        std::string str = std::string();
        if (!is_multiline && !str_type.b_string){
            while (end_char == peektoken()){
                char c = nexttoken();
                if (c == '\\' && !str_type.r_string){
                    char after_slash = nexttoken();
                    if (after_slash == '\\'){
                        str.push_back('\\');
                    }
                    else if (after_slash == '"'){
                        str.push_back('\"');
                    }
                    else if (after_slash == '\''){
                        str.push_back('\'');
                    }
                    else if (after_slash == 'n'){
                        str.push_back('\n');
                    }
                    else if (after_slash == 'r'){
                        str.push_back('\r');
                    }
                    else if (after_slash == 't'){
                        str.push_back('\t');
                    }
                    else if (after_slash == 'b'){
                        str.push_back('\b');
                    }
                    else if (after_slash == 'f'){
                        str.push_back('\f');
                    }
                    else if (after_slash == 'v'){
                        str.push_back('\v');
                    }
                    else if (after_slash == 'a'){
                        str.push_back('\a');
                    }
                    // Octal: \0 to \077 (1–3 digits)
                    else if (after_slash >= '0' && after_slash <= '7'){
                    int value = after_slash - '0';
                    for (int i = 1; i < 3; ++i){
                    char next = peektoken();
                    if (next >= '0' && next <= '7'){
                        value = value * 8 + (next - '0');
                        nexttoken();
                    }
                    else break;
                    }
                    str.push_back(static_cast<char>(value));
                    }
                    else if (after_slash == 'x'){
                        int value = 0;
                        for (int i = 0; i < 2; ++i){
                            char next = nexttoken();
                            if (next >= '0' && next <= '9') value = value*16 + (next - '0');
                            else if (next >= 'a' && next <= 'f') value = value*16 + (next - 'a' + 10);
                            else if (next >= 'A' && next <= 'F') value = value*16 + (next - 'A' + 10);
                            else break; // invalid, stop early
                    }
                    str.push_back(static_cast<char>(value));
                    }
                    else if (after_slash == 'u'){
                    int value = 0;
                    for (int i = 0; i < 4; ++i){
                        char next = nexttoken();
                        if (next >= '0' && next <= '9') value = value*16 + (next - '0');
                        else if (next >= 'a' && next <= 'f') value = value*16 + (next - 'a' + 10);
                        else if (next >= 'A' && next <= 'F') value = value*16 + (next - 'A' + 10);
                        else break; // invalid
                    }
                    str.push_back(static_cast<char>(value)); // note: only works for code points <= 255
                    }
                    else if (after_slash == 'U'){
                    unsigned int value = 0;
                    for (int i = 0; i < 8; ++i){
                        char next = nexttoken();
                        if (next >= '0' && next <= '9') value = value*16 + (next - '0');
                        else if (next >= 'a' && next <= 'f') value = value*16 + (next - 'a' + 10);
                        else if (next >= 'A' && next <= 'F') value = value*16 + (next - 'A' + 10);
                        else break; // invalid
                    }
                    str.push_back(static_cast<char>(value)); // note: only works for code points <= 255
                    }
                    // Unknown escape: just take the char literally
                    else{
                        str.push_back(after_slash);
                    }
                }
                str.push_back(c);
            }
        }
        else if (!str_type.b_string){
            while (!(peektoken() == end_char && peektoken(1) == end_char && peektoken(2) == end_char)){
                char c = nexttoken();
                if (c == '\\' && !str_type.r_string){
                    char after_slash = nexttoken();
                    if (after_slash == '\\'){
                        str.push_back('\\');
                    }
                    else if (after_slash == '"'){
                        str.push_back('\"');
                    }
                    else if (after_slash == '\''){
                        str.push_back('\'');
                    }
                    else if (after_slash == 'n'){
                        str.push_back('\n');
                    }
                    else if (after_slash == 'r'){
                        str.push_back('\r');
                    }
                    else if (after_slash == 't'){
                        str.push_back('\t');
                    }
                    else if (after_slash == 'b'){
                        str.push_back('\b');
                    }
                    else if (after_slash == 'f'){
                        str.push_back('\f');
                    }
                    else if (after_slash == 'v'){
                        str.push_back('\v');
                    }
                    else if (after_slash == 'a'){
                        str.push_back('\a');
                    }
                    // Octal: \0 to \077 (1–3 digits)
                    else if (after_slash >= '0' && after_slash <= '7'){
                    int value = after_slash - '0';
                    for (int i = 1; i < 3; ++i){
                    char next = peektoken();
                    if (next >= '0' && next <= '7'){
                        value = value * 8 + (next - '0');
                        nexttoken();
                    }
                    else break;
                    }
                    str.push_back(static_cast<char>(value));
                    }
                    else if (after_slash == 'x'){
                        int value = 0;
                        for (int i = 0; i < 2; ++i){
                            char next = nexttoken();
                            if (next >= '0' && next <= '9') value = value*16 + (next - '0');
                            else if (next >= 'a' && next <= 'f') value = value*16 + (next - 'a' + 10);
                            else if (next >= 'A' && next <= 'F') value = value*16 + (next - 'A' + 10);
                            else break; // invalid, stop early
                    }
                    str.push_back(static_cast<char>(value));
                    }
                    else if (after_slash == 'u'){
                    int value = 0;
                    for (int i = 0; i < 4; ++i){
                        char next = nexttoken();
                        if (next >= '0' && next <= '9') value = value*16 + (next - '0');
                        else if (next >= 'a' && next <= 'f') value = value*16 + (next - 'a' + 10);
                        else if (next >= 'A' && next <= 'F') value = value*16 + (next - 'A' + 10);
                        else break; // invalid
                    }
                    str.push_back(static_cast<char>(value)); // note: only works for code points <= 255
                    }
                    else if (after_slash == 'U'){
                    unsigned int value = 0;
                    for (int i = 0; i < 8; ++i){
                        char next = nexttoken();
                        if (next >= '0' && next <= '9') value = value*16 + (next - '0');
                        else if (next >= 'a' && next <= 'f') value = value*16 + (next - 'a' + 10);
                        else if (next >= 'A' && next <= 'F') value = value*16 + (next - 'A' + 10);
                        else break; // invalid
                    }
                    str.push_back(static_cast<char>(value)); // note: only works for code points <= 255
                    }
                    // Unknown escape: just take the char literally
                    else{
                        str.push_back(after_slash);
                    }
                }
                str.push_back(c);
            }
        }
        tok.value = str;
        tokens.push_back(tok);
    }

    static string_type string_type_parser(const std::string& ident) {
        string_type str_type = {};
        if (ident.contains('f') || ident.contains('F')){
            str_type.f_string = true;
        }
        if (ident.contains('b') || ident.contains('B')){
            if (str_type.f_string){
                throw std::runtime_error("u and b are not supported");
            }
            str_type.b_string = true;
        }
        if (ident.contains('r') || ident.contains('R')){
            str_type.r_string = true;
        }
        if (!only_valid_prefix_chars(ident)){
            throw std::runtime_error("that is illegal");
        }
        return str_type;
    }

    static bool only_valid_prefix_chars(const std::string& text){
        return !text.empty() && text.find_first_not_of("RrFfBb") == std::string::npos;
    }

    char peektoken(int look_ahead = 0){
        if (Text.size() > TokenPos) return '\0';
        char c = Text[TokenPos + look_ahead];
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