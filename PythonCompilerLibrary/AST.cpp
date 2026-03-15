#include "AST.h"


Parser::Parser(){
    TokenPos = 0;
    Tokens = {};
    IndentLevel = 0;
}

void Parser::reset_parser(){
    TokenPos = 0;
    Tokens = {};
    IndentLevel = 0;
}

std::vector<Node> Parser::program_parse_ast(const std::vector<token>& tokens){
    Tokens = tokens;
    std::vector<Node> AST;
    while(TokenPos < Tokens.size()){}
    reset_parser();
    return AST;
}



Node Parser::parse_factor(){
    Node factor;
    token tok = get_token();
    if (tok.type == TokenType::NUMBER || tok.type == TokenType::STRING || tok.type == TokenType::KEYWORD){
        if (tok.type == TokenType::NUMBER){
            NumberNode number{.number = std::stof(std::get<std::string>(tok.value))};
            factor = number;
        }
        else if (tok.type == TokenType::STRING){
            StringNode string;
            string.string = std::get<std::string>(tok.value);
            factor = string;
        }
        else{
            KeyWord value = std::get<KeyWord>(tok.value);
            if (value != KeyWord::TRUE && value != KeyWord::FALSE){
                return {};
            }
            bool bool_value;
            if (KeyWord::TRUE == value){
                bool_value = true;
            }
            else{
                bool_value = false;
            }
            BoolNode boolean{.boolean = bool_value};
            factor = boolean;
        }
    }
    return factor;
}

token Parser::get_token(){
    token tok = peek_token();
    TokenPos++;
    return tok;
}

token Parser::peek_token(){
    token tok;
    if (TokenPos > Tokens.size()){
        tok = {};
    }
    else{
        tok = Tokens[TokenPos];
    }
    return tok;
}

bool Parser::match_token(const TokenType& type){
    return peek_token().type == type;
}

void Parser::expect_token(TokenType type){
    bool is_expected = match_token(type);
    if (!is_expected){
        throw std::runtime_error("this is not a valid token");
    }
    get_token();
}
