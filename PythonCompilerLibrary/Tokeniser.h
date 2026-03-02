#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H
#include <atomic>
#include <map>
#include <string>
#include <variant>
#include <vector>

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
    Tokeniser(){
        token_pos = 0;
        text = std::string();
    }

    std::vector<token> tokenize(const std::string& input_text){
        text = input_text;
        std::vector<token> tokens;
        while (token_pos < text.size()){
            literal_token token_literal = get_literal_token();
            if (token_literal.sucses){
                tokens.push_back(token_literal.token);
            }
            else if (peek_token() == '#'){
                bool is_new_line = false;
                while (!is_new_line && !eof()){
                    char c = peek_token();
                    if (c == '\n'){
                        is_new_line = true;
                        token tok;
                        tok.type = TokenType::NEWLINE;
                        tok.value = "";
                        tokens.push_back(tok);
                    }
                    next_token();
                }
            }
            else if (peek_token() == '"'){
                next_token();
                token tok;
                tok.type = TokenType::STRING;
                bool is_string_end = false;
                std::string string_literal_value = std::string();
                while (!is_string_end && !eof()){
                    char c = peek_token();
                    if (c == '"'){
                        is_string_end = true;
                    }
                    else{
                        string_literal_value += c;
                    }
                    next_token();
                }
                tok.value = string_literal_value;
                tokens.push_back(tok);
            }
            else if (std::isdigit(peek_token())){
                token tok;
                tok.type = TokenType::NUMBER;
                std::string number_literal_value = std::string();
                while (std::isdigit(peek_token()) && !eof()){
                    char c = next_token();
                    number_literal_value += c;
                }
                tok.value = number_literal_value;
                tokens.push_back(tok);
            }
            else if (std::isalpha(peek_token())){
                token tok;
                std::string ident_literal_value = std::string();
                while (std::isalnum(peek_token()) or (peek_token() == '_') && !eof()){
                    ident_literal_value += next_token();
                }
                KeyWord keyword_type = check_keyword(ident_literal_value);
                if (keyword_type != KeyWord::EMPTY){
                    tok.type = TokenType::KEYWORD;
                    tok.value = keyword_type;
                }
                else {
                    tok.type = TokenType::IDENT;
                    tok.value = ident_literal_value;
                }
                tokens.push_back(tok);
            }
            else if (peek_token() == '\n'){
                token tok;
                tok.type = TokenType::NEWLINE;
                tok.value = "\n";
                next_token();
                tokens.push_back(tok);
            }
            else{
                next_token();
            }
        }
        reset_tokeniser();
        return tokens;
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
        literal_token result;

        size_t remaining = text.size() - token_pos;

        // Try lengths 3, then 2, then 1 (longest match)
        for (int len = 3; len >= 1; --len){
            if (remaining < len) continue;

            std::string literal = text.substr(token_pos, len);

            for (const auto& [type, repr] : token_type_char){
                if (repr == literal){
                    token_pos += len;
                    result.sucses = true;
                    result.token = {type, ""};
                    return result;
                }
            }
        }

        result.sucses = false;
        result.token = {};
        return result;
    }

    static KeyWord check_keyword(const std::string& keyword){
        for (const std::pair<std::string, KeyWord> token_type : keyword_char){
            if (keyword == token_type.first){
                return token_type.second;
            }
        }
        return KeyWord::EMPTY;
    }

    bool eof() const {
        return token_pos >= text.size();
    }

};

#endif //SUPERBUILD_TOKENISER_H