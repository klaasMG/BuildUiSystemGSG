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
    std::vector<IdentNode> Args;
};

class ASTGMAKE{
    std::vector<Token> tokens;
    int currentToken;

public:
    ASTGMAKE(const std::vector<Token> &input_tokens);

    std::vector<std::unique_ptr<ASTNode>> getNodes();

private:
    Token getNextToken();
    static void throw_ast_error(const std::string& message);
};

#endif
