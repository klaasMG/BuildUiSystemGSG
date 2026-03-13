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
    std::vector<IdentNode> Args;  // Changed: removed nested vector
};

class ASTGMAKE{
    std::vector<Token> tokens;
    int currentToken;

public:
    ASTGMAKE(const std::vector<Token> &input_tokens){
        tokens = input_tokens;
        currentToken = 0;
    }

    std::vector<std::unique_ptr<ASTNode>> getNodes(){
        std::vector<std::unique_ptr<ASTNode>> nodes;
        while (currentToken < tokens.size()){
            Token token = getNextToken();
            if (token.type == TokenType::Identifier){
                auto node = std::make_unique<FunctionNode>();
                Token left_bracket = getNextToken();
                if (left_bracket.type != TokenType::LeftBracket){
                    throw_ast_error("Expected '(' after function name");
                }
                bool func_end = false;
                std::vector<IdentNode> func_args;  // Changed: single vector
                while (!func_end){
                    Token next_token = getNextToken();
                    if (next_token.type == TokenType::Identifier || next_token.type == TokenType::Slash){
                        IdentNode ident_node;  // Changed: single node
                        ident_node.Ident = next_token.value;
                        bool ident_end = false;
                        while (!ident_end){
                            next_token = getNextToken();
                            if (next_token.type == TokenType::Comma){
                                ident_end = true;
                            }
                            else if (next_token.type == TokenType::RightBracket){
                                func_end = true;
                                ident_end = true;
                            }
                            else if (next_token.type == TokenType::Identifier){
                                // Append to the current identifier node
                                ident_node.Ident += next_token.value;
                            }
                            else if (next_token.type == TokenType::Slash){
                                ident_node.Ident += "\\";
                            }
                            else{
                                throw_ast_error("Expected a 'identifier' or a ',' for end argument");
                            }
                        }
                        func_args.push_back(ident_node);  // Changed: push single node
                    }
                    else if (next_token.type == TokenType::RightBracket){
                        func_end = true;
                    }
                }
                IdentNode identifier;
                identifier.Ident = token.value;
                node->Ident = identifier;
                node->Args = func_args;
                nodes.push_back(std::move(node));
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
        throw_ast_error("EOF error");
        return Token{};
    }

    static void throw_ast_error(const std::string& message){
        std::cerr << "ast error" << std::endl;
        throw std::runtime_error(message);
    }
};

#endif //EVENT_STRUCT_SIMPLEASTGMAKE_H