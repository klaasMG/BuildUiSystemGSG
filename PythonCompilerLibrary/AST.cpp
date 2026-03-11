#include "AST.h"

Parser::Parser(){
    TokenPos = 0;
    Tokens = {};
}

void Parser::reset_parser(){
    TokenPos = 0;
    Tokens = {};
}

std::vector<Node> Parser::parse_ast(const std::vector<token>& tokens){
    std::vector<Node> AST;
    while(TokenPos < tokens.size()){
        token tok = peek_token();
    }
    reset_parser();
    return AST;
}

token Parser::get_token(){
    token tok = peek_token();
    TokenPos++;
    return tok;
}

token Parser::peek_token(){
    token tok = Tokens[TokenPos];
    return tok;
}
