#ifndef SUPERBUILD_AST_H
#define SUPERBUILD_AST_H
#include <string>
#include <variant>
#include <vector>

#include "Tokeniser.h"

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
struct FunctionNode{
    std::string name;
    std::vector<size_t> args;
    size_t decorator;
    size_t body;
    size_t return_node;
};
struct ClassNode{
    std::string name;
    std::vector<size_t> args;
    size_t body;
};
struct IfNode{
    size_t condition;
    size_t if_body;
    size_t else_body;
};
struct WhileNode{
    size_t condition;
    size_t while_body;
    size_t else_body;
};
struct ForNode{
    size_t condition;
    size_t for_body;
    size_t else_body;
};
struct CaseNode{
    size_t patern;
    size_t guard;
    size_t body;
};
struct MatchNode{
    std::vector<size_t> cases;
};
struct TryNode{
    size_t try_body;
    size_t except_condition;
    size_t except_body;
    size_t finally_body;
    size_t else_body;
};

struct WithNode{
    size_t function;
    size_t allias;
    size_t body;
};

struct ImportNode{
    size_t import;
    size_t alias;
    size_t from;
};

struct FStringNode{
    std::vector<std::variant<size_t, std::string>> values;
};

struct BreakNode{};

struct ContinueNode{};

struct AssertNode {
    size_t test;
    size_t msg;
};

struct AnnotationNode{
    size_t annotation;
};

using Node = std::variant<NumberNode, StringNode, BinaryNode, IdentNode, UnaryExprNode, BinaryExprNode, CallExprNode,
                          AssignNode, CollectionNode ,ProgramNode ,FunctionNode,ClassNode ,IfNode ,WhileNode ,ForNode ,CaseNode ,MatchNode ,TryNode, WithNode, ImportNode, FStringNode, BreakNode,
                          ContinueNode, AssertNode, AnnotationNode>;

class Parser{
public:
    Parser();
    std::vector<Node> parse_ast(const std::vector<token>& tokens);
private:
    std::vector<token> Tokens;
    uint64_t TokenPos;
    void reset_parser();
    token get_token();
    token peek_token();
};
#endif //SUPERBUILD_AST_H