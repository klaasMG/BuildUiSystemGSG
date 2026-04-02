//
// Created by klaas on 3/25/2026.
//

#ifndef SUPERBUILD_GMAKEPYTHONPARSER_H
#define SUPERBUILD_GMAKEPYTHONPARSER_H
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
    std::variant<double , int64_t> number;
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

struct BodyNode{
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
    std::vector<size_t> import;
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
                          ContinueNode, AssertNode, AnnotationNode, BoolNode, ArgNode, BodyNode>;

class Parser{
public:
    Parser();
    std::vector<Node> program_parse_ast(const std::vector<token>& tokens);
private:
    size_t Node_pos = 0;
    std::vector<Node> AST = {};
    uint64_t IndentLevel = 0;
    std::vector<token> Tokens = {};
    uint64_t TokenPos = 0;
    std::vector<Node> parse_code();
    size_t parse_body();
    size_t parse_statement();
    size_t parse_expression();
    size_t parse_class();
    size_t parse_import();
    std::vector<size_t> parse_arguments();
    size_t parse_assignment();
    size_t parse_factor();
    size_t parse_identifier(bool is_not_atribute_allowed = false);
    size_t push_node(const Node& node);
    void reset_parser();
    token get_token();
    token peek_token(const int& look_ahead = 0);
    bool match_token(const TokenType& type);
    token expect_token(TokenType type);
};

#endif //SUPERBUILD_GMAKEPYTHONPARSER_H