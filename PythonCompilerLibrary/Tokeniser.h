#ifndef SUPERBUILD_TOKENISER_H
#define SUPERBUILD_TOKENISER_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>
#include <map>
#include <array>
#include <cmath>
#include <stdexcept>

#include "tokens.h"

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
    std::vector<token> tokenise(const std::string& text, bool f_string = false){
        Text = text;
        std::vector<token> tokens;
        while (peektoken() != '\0'){
            if (char c = peektoken();std::isalpha(c)){
                std::string ident = ident_getter();
                if (peektoken() == '"' || peektoken() == '\''){
                    char end_char = nexttoken();
                    parse_string(end_char, string_type_parser(ident), tokens);
                }
                else if (KeyWord keyword = check_keyword(ident); keyword != KeyWord::EMPTY){
                    token tok;
                    tok.type = TokenType::KEYWORD;
                    tok.value = keyword;
                    tokens.push_back(tok);
                }
                else{
                    token tok;
                    tok.type = TokenType::IDENT;
                    tok.value = ident;
                    tokens.push_back(tok);
                }
            }
            else if (TokenType literal_token = get_literal_token_type(); literal_token != TokenType::EMPTY){
                if (literal_token == TokenType::DOT){
                    if (char num_check = peektoken(); std::isdigit(num_check)){
                        std::string number = parse_number();
                        token tok;
                        tok.type = TokenType::NUMBER;
                        tok.value = number;
                        tokens.push_back(tok);
                    }
                    else{
                        token tok;
                        tok.type = TokenType::DOT;
                        tok.value = std::string();
                        tokens.push_back(tok);
                    }
                }
                else if (literal_token == TokenType::LBRACE || literal_token == TokenType::LBRACK || literal_token == TokenType::LPAREN){
                    BracketDepth++;
                    token tok;
                    tok.type = literal_token;
                    tok.value = std::string();
                    tokens.push_back(tok);
                }
                else if (literal_token == TokenType::RBRACE || literal_token == TokenType::RBRACK || literal_token == TokenType::RPAREN){
                    BracketDepth--;
                    token tok;
                    tok.type = literal_token;
                    tok.value = std::string();
                    tokens.push_back(tok);
                }
                else{
                    token tok;
                    tok.type = literal_token;
                    tok.value = std::string();
                    tokens.push_back(tok);
                }
            }
            else if (peektoken() == '\n'){
                if (!BracketDepth){
                    token tok;
                    tok.type = TokenType::NEWLINE;
                    tok.value = std::string();
                    tokens.push_back(tok);
                }
                nexttoken();
                tokens = parse_indent(tokens);
            }
            else if (char end_char = peektoken(); end_char == '"' || end_char == '\''){
                nexttoken();
                string_type str_type;
                parse_string( end_char, str_type, tokens);
            }
            else if (isdigit(peektoken())){
                std::string number = parse_number();
                token tok;
                tok.type = TokenType::NUMBER;
                tok.value = number;
                tokens.push_back(tok);
            }
            else if (peektoken() == '#'){
                while (peektoken() != '\n' || peektoken() != '\0'){
                    nexttoken();
                }
            }
            else if (peektoken() == '\t' || peektoken() == ' ') {
                nexttoken();
            }
            else if (peektoken() == '\\'){
                while (peektoken() != '\n' || peektoken() != '\0'){
                    if (peektoken() != ' ' || peektoken() != '\t') {
                        throw std::invalid_argument("Escape character expected");
                    }
                }
            }
            else{
                throw std::invalid_argument("Unexpected token");
            }
        }
        token tok;
        tok.type = TokenType::ENDMARKER;
        tok.value = std::string();
        tokens.push_back(tok);
        return tokens;
    }

private:
    uint64_t TokenPos = 0;
    std::string Text = std::string();
    std::vector<uint64_t> IndentStack = {0};
    uint64_t BracketDepth = 0;

    std::vector<token> parse_indent(std::vector<token> tokens){
        uint64_t indent = 0;
        while (peektoken() == ' ' || peektoken() == '\t'){
            char c = nexttoken();
            if (c == '\t'){
                int tab_width = 8;
                indent += tab_width;
            }
            else{
                int space_width = 1;
                indent += space_width;
            }
        }
        if (indent > IndentStack.back()){
            IndentStack.push_back(indent);
            token tok;
            tok.type = TokenType::INDENT;
            tok.value = std::string();
            tokens.push_back(tok);
        }
        else if (indent < IndentStack.back()){
            while (indent < IndentStack.back()){
                IndentStack.pop_back();

                token tok;
                tok.type = TokenType::DEDENT;
                tok.value = std::string();
                tokens.push_back(tok);
            }

            if (IndentStack.back() != indent){
                throw std::invalid_argument("Indent does not match indentation");
            }
        }
        return tokens;
    }

    std::string parse_number(bool is_float_in = false, bool is_after_e_in = false){
        bool is_float = is_float_in;
        bool is_after_e = is_after_e_in;
        std::string number = std::string();
        std::string exponent;
        while (std::isdigit(peektoken()) || peektoken() == '.' || peektoken() == 'e' || peektoken() == 'E' || peektoken() == '_' || (peektoken() == '-' && is_after_e)){
            char c = nexttoken();
            if (c == '.'){
                if (is_float){
                    throw std::invalid_argument("Two dots in a number");
                }
                is_float = true;
                number.push_back(c);
            }
            else if (c == 'e' || c == 'E'){
                exponent = parse_number(false,true);
                break;
            }
            else{
                number.push_back(c);
            }
        }

        bool is_last_dash = false;
        for (const char& c : number){
            if (c == '_'){
                if (is_last_dash){
                    throw std::invalid_argument("Invalid number");
                }
                is_last_dash = true;
            }
        }
        if (number.ends_with("_") || number.starts_with('_')){
            throw std::invalid_argument("Invalid number");
        }
        if (!exponent.empty()){
            double exp = atof(exponent.c_str());
            double num = atof(number.c_str());
            float result = std::pow(10, exp);
            result = result * num;
            number = std::to_string(result);
        }

        return number;
    }

    TokenType get_literal_token_type(){
        for (const auto& [type, text] : token_type_char){
            bool match = true;

            for (size_t i = 0; i < text.size(); ++i){
                if (peektoken(i) != text[i]){
                    match = false;
                    break;
                }
            }

            if (match){
                for (size_t i = 0; i < text.size(); ++i){
                    nexttoken();
                }
                return type;
            }
        }

        return TokenType::EMPTY;
    }

    std::string ident_getter(){
        std::string ident = std::string();
        while (std::isalnum(peektoken()) || peektoken() == '_'){
            char c = nexttoken();
            ident.push_back(c);
        }
        return ident;
    }

    KeyWord static check_keyword(const std::string& keyword){
        for (std::pair<std::string, KeyWord> key : keyword_char){
            if (key.first == keyword){
                return key.second;
            }
        }
        return KeyWord::EMPTY;
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
        if (!str_type.f_string && !str_type.b_string){
            tok.type = TokenType::STRING;
        }
        else if (!str_type.f_string){
            tok.type = TokenType::BYTES;
        }
        else{
            tok.type = TokenType::FSTRING_START;
            tok.value = std::string();
            tokens.push_back(tok);
        }
        std::string str = std::string();
        if (!is_multiline){
            while (end_char != peektoken()){
                char c = nexttoken();
                if (str_type.b_string){
                    if (!str_type.r_string && c == '\\') {
                        char after = nexttoken();

                        if (after == '\\') str.push_back('\\');
                        else if (after == '"') str.push_back('"');
                        else if (after == '\'') str.push_back('\'');
                        else if (after == 'n') str.push_back('\n');
                        else if (after == 'r') str.push_back('\r');
                        else if (after == 't') str.push_back('\t');
                        else if (after == 'b') str.push_back('\b');
                        else if (after == 'f') str.push_back('\f');
                        else if (after == 'v') str.push_back('\v');
                        else if (after == 'a') str.push_back('\a');

                        else if (after >= '0' && after <= '7') { // octal
                            int val = after - '0';
                            for (int i = 0; i < 2; ++i) {
                                char p = peektoken();
                                if (p >= '0' && p <= '7') {
                                    val = val * 8 + (p - '0');
                                    nexttoken();
                                } else break;
                            }
                            str.push_back(static_cast<char>(val));
                        }

                        else if (after == 'x') { // hex
                            int val = 0;
                            for (int i = 0; i < 2; ++i) {
                                char h = nexttoken();
                                if (h >= '0' && h <= '9') val = val * 16 + (h - '0');
                                else if (h >= 'a' && h <= 'f') val = val * 16 + (h - 'a' + 10);
                                else if (h >= 'A' && h <= 'F') val = val * 16 + (h - 'A' + 10);
                                else throw std::runtime_error("Invalid hex escape");
                            }
                            str.push_back(static_cast<char>(val));
                        }

                        else
                            throw std::runtime_error("Invalid escape in b-string");
                    }
                    else {
                        if ((unsigned char)c > 127)
                            throw std::runtime_error("Non-ASCII in b-string");
                        str.push_back(c);
                    }

                }
                if (c == '{' && str_type.f_string && !str_type.b_string){
                    token f_mid_tok;
                    f_mid_tok.type = TokenType::FSTRING_MIDDLE;
                    f_mid_tok.value = str;
                    tokens.push_back(f_mid_tok);
                    str = std::string();
                    std::string sub_parse_fstring;
                    while (peektoken() != '}'){
                        char c1 = nexttoken();
                        sub_parse_fstring.push_back(c1);
                    }
                    bool is_fstring = true;
                    std::vector<token> sub_tokens = tokenise(sub_parse_fstring, is_fstring);
                    for (const token& sub_tok : sub_tokens){
                        tokens.push_back(sub_tok);
                    }
                }
                if (c == '\\' && !str_type.r_string && !str_type.b_string){
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
            nexttoken();
        }
        else{
            while (!(peektoken() == end_char && peektoken(1) == end_char && peektoken(2) == end_char)){
                char c = nexttoken();
                if (str_type.b_string){
                    if (!str_type.r_string && c == '\\') {
                        char after = nexttoken();

                        if (after == '\\') str.push_back('\\');
                        else if (after == '"') str.push_back('"');
                        else if (after == '\'') str.push_back('\'');
                        else if (after == 'n') str.push_back('\n');
                        else if (after == 'r') str.push_back('\r');
                        else if (after == 't') str.push_back('\t');
                        else if (after == 'b') str.push_back('\b');
                        else if (after == 'f') str.push_back('\f');
                        else if (after == 'v') str.push_back('\v');
                        else if (after == 'a') str.push_back('\a');

                        else if (after >= '0' && after <= '7') { // octal
                            int val = after - '0';
                            for (int i = 0; i < 2; ++i) {
                                char p = peektoken();
                                if (p >= '0' && p <= '7') {
                                    val = val * 8 + (p - '0');
                                    nexttoken();
                                } else break;
                            }
                            str.push_back(static_cast<char>(val));
                        }

                        else if (after == 'x') { // hex
                            int val = 0;
                            for (int i = 0; i < 2; ++i) {
                                char h = nexttoken();
                                if (h >= '0' && h <= '9') val = val * 16 + (h - '0');
                                else if (h >= 'a' && h <= 'f') val = val * 16 + (h - 'a' + 10);
                                else if (h >= 'A' && h <= 'F') val = val * 16 + (h - 'A' + 10);
                                else throw std::runtime_error("Invalid hex escape");
                            }
                            str.push_back(static_cast<char>(val));
                        }

                        else
                            throw std::runtime_error("Invalid escape in b-string");
                    }
                    else {
                        if ((unsigned char)c > 127)
                            throw std::runtime_error("Non-ASCII in b-string");
                        str.push_back(c);
                    }

                }
                if (c == '{' && str_type.f_string && !str_type.b_string){
                    token f_mid_tok;
                    f_mid_tok.type = TokenType::FSTRING_MIDDLE;
                    f_mid_tok.value = str;
                    tokens.push_back(f_mid_tok);
                    str = std::string();
                    std::string sub_parse_fstring;
                    while (peektoken() != '}'){
                        char c1 = nexttoken();
                        sub_parse_fstring.push_back(c1);
                    }
                    bool is_fstring = true;
                    std::vector<token> sub_tokens = tokenise(sub_parse_fstring, is_fstring);
                    for (const token& sub_tok : sub_tokens){
                        tokens.push_back(sub_tok);
                    }
                }
                if (c == '\\' && !str_type.r_string && !str_type.b_string){
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
            nexttoken();
            nexttoken();
            nexttoken();
        }
        if (str_type.f_string){
            if (!str.empty()){
                token last_fstring_mid_tok;
                last_fstring_mid_tok.type = TokenType::FSTRING_MIDDLE;
                last_fstring_mid_tok.value = str;
                tokens.push_back(last_fstring_mid_tok);
            }
            token last_fstring_tok;
            last_fstring_tok.type = TokenType::FSTRING_END;
            last_fstring_tok.value = std::string();
            tokens.push_back(last_fstring_tok);
        }
        else{
            tok.value = str;
            tokens.push_back(tok);
        }
    }

    static string_type string_type_parser(const std::string& ident) {
        string_type str_type = {};
        if (ident.contains('f') || ident.contains('F')){
            str_type.f_string = true;
        }
        if (ident.contains('b') || ident.contains('B')){
            if (str_type.f_string){
                throw std::runtime_error("f and b are not supported");
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
        if (Text.size() <= TokenPos + look_ahead) return '\0';
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
        IndentStack = {0};
        BracketDepth = 0;
    }
};

#endif //SUPERBUILD_TOKENISER_H