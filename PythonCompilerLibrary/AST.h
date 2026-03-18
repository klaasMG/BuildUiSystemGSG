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
    bool is_await;
    size_t name;
    std::vector<size_t> args;
};
struct AssignNode{
    size_t identifier;
    size_t value;
    size_t annotation;
};

struct ArgNode{
    size_t identifier;
    size_t default_value;
    size_t annotation;
    bool is_args = false;
    bool is_kwargs = false;
};

struct CollectionNode{
    CollectionType type;
    std::vector<size_t> values;
};
struct ProgramNode{
    std::vector<size_t> lines;
};
struct FunctionNode{
    bool is_await;
    std::string name;
    std::vector<size_t> args;
    size_t annotation;
    size_t decorator;
    size_t body;
    size_t return_node;
};
struct ClassNode{
    std::string name;
    std::vector<size_t> args;
    size_t body;
    size_t annotation;
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

struct BoolNode{
    bool boolean;
};

struct AnnotationNode{
    size_t annotation;
};

using ExprNode = std::variant<BinaryExprNode, UnaryExprNode>;

using Node = std::variant<NumberNode, StringNode, BinaryNode, IdentNode, UnaryExprNode, BinaryExprNode, CallExprNode,
                          AssignNode, CollectionNode ,ProgramNode ,FunctionNode,ClassNode ,IfNode ,WhileNode ,ForNode ,CaseNode ,MatchNode ,TryNode, WithNode, ImportNode, FStringNode, BreakNode,
                          ContinueNode, AssertNode, AnnotationNode, BoolNode, ArgNode>;

class Parser{
public:
    Parser();
    std::vector<Node> program_parse_ast(const std::vector<token>& tokens);
private:
    size_t Node_pos;
    std::vector<Node> AST;
    uint64_t IndentLevel;
    std::vector<token> Tokens;
    uint64_t TokenPos;
    size_t parse_factor();
    size_t parse_exponent();
    size_t parse_muldiv();
    size_t parse_addsub();
    size_t parse_shift();
    size_t parse_and();
    size_t parse_xor();
    size_t parse_or();
    size_t parse_comparison();
    size_t parse_keyword_and();
    size_t parse_keyword_or();
    size_t parse_expression();
    size_t push_node(const Node& node);
    void reset_parser();
    token get_token();
    token peek_token(const int& look_ahead = 0);
    bool match_token(const TokenType& type);
    void expect_token(TokenType type);
};
#endif //SUPERBUILD_AST_H