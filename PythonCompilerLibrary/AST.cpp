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

    }
    reset_parser();
    return AST;
}
