#ifndef EVENT_STRUCT_SIMPLEASTGMAKE_H
#define EVENT_STRUCT_SIMPLEASTGMAKE_H

#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>
#include "Tokens.h"
#include <filesystem>
#include <iostream>

struct ASTNode {
    virtual ~ASTNode() = default;
};

struct IdentNode : public ASTNode{
    std::string Ident;
};

struct FunctionNode : public ASTNode{
    IdentNode Ident;
    std::vector<std::vector<IdentNode>> Args;  // Changed: just store IdentNodes directly
};

class ASTGMAKE{
    std::vector<Token> tokens;
    int currentToken;

public:
    ASTGMAKE(const std::vector<Token> &input_tokens){
        tokens = input_tokens;
        currentToken = 0;
    }

    std::vector<std::unique_ptr<ASTNode>> getNodes(){  // Changed: use unique_ptr
        std::vector<std::unique_ptr<ASTNode>> nodes;
        while (currentToken < tokens.size()){
            Token token = getNextToken();
            if (token.type == TokenType::Identifier){
                auto node = std::make_unique<FunctionNode>();  // Changed
                Token left_bracket = getNextToken();
                if (left_bracket.type != TokenType::LeftBracket){
                    throw_ast_error("Expected '(' after function name");
                }
                bool func_end = false;
                std::vector<std::vector<IdentNode>> func_args;  // Changed
                while (!func_end){
                    Token next_token = getNextToken();
                    if (next_token.type == TokenType::Identifier || next_token.type == TokenType::Slash){
                        std::vector<IdentNode> ident_nodes;  // Changed
                        IdentNode ident_node;
                        ident_node.Ident = next_token.value;
                        ident_nodes.push_back(ident_node);
                        bool ident_end = false;  // Fixed typo
                        while (!ident_end){
                            next_token = getNextToken();
                            if (next_token.type == TokenType::Comma){
                                ident_end = true;
                            }
                            else if (next_token.type == TokenType::RightBracket){
                                func_end = true;
                                ident_end = true;
                            }
                            else if (next_token.type == TokenType::Identifier || next_token.type == TokenType::Slash){
                                IdentNode ident_node_internal;
                                ident_node_internal.Ident = next_token.value;
                                ident_nodes.push_back(ident_node_internal);
                            }
                            else{
                                throw_ast_error("Expected a 'identifier' or a ',' for end argument");
                            }
                        }
                        func_args.push_back(ident_nodes);
                    }
                    else if (next_token.type == TokenType::RightBracket){
                        func_end = true;
                    }
                }
                IdentNode identifier;
                identifier.Ident = token.value;
                node->Ident = identifier;  // Changed: use ->
                node->Args = func_args;
                nodes.push_back(std::move(node));  // Changed
            }
            else if (token.type != TokenType::Semicolon){
                throw_ast_error("Expected ';' after function name");
            }
        }
        return nodes;
    }

private:
    Token getNextToken(){
        if (currentToken < tokens.size()){
            Token token = tokens[currentToken];
            currentToken++;
            return token;
        }
        throw_ast_error("EOF error");  // Fixed typo
        return Token{};
    }

    static void throw_ast_error(const std::string& message){
        std::cerr << "ast error" << std::endl;
        throw std::runtime_error(message);
    }
};

#endif //EVENT_STRUCT_SIMPLEASTGMAKE_H