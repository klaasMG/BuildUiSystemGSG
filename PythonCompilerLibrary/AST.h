//
// Created by klaas on 3/9/2026.
//

#ifndef SUPERBUILD_AST_H
#define SUPERBUILD_AST_H
#include <string>
#include <variant>
#include <vector>

enum class CollectionType{
    Dict,
    List,
    Set,
    Tuple,
};

struct NumberNode{
    float number;
};
struct StringNode{
    std::string string;
};
struct BinaryNode{
    std::string binary;
};
struct IdentNode{
    std::string ident;
};
struct UnaryExprNode{
    std::string opp;
    size_t value;
};
struct BinaryExprNode{
    std::string opp;
    size_t left;
    size_t right;
};
struct CallExprNode{
    std::string name;
    std::vector<size_t> args;
};
struct AssignNode{
    std::string asinged;
    size_t value;
};
struct CollectionNode{
    CollectionType type;
    std::vector<size_t> values;
};
struct ProgramNode{
    std::vector<size_t> lines;
};
struct FunctionNode;
struct ClassNode;
struct IfNode;
struct WhileNode;
struct ForNode;
struct CaseNode;
struct MatchNode;
struct TryNode;

using Node = std::variant<NumberNode, StringNode, BinaryNode, IdentNode, UnaryExprNode, BinaryExprNode, CallExprNode,
                          AssignNode, CollectionNode ,ProgramNode ,FunctionNode,ClassNode ,IfNode ,WhileNode ,ForNode ,CaseNode ,MatchNode ,TryNode >;

#endif //SUPERBUILD_AST_H