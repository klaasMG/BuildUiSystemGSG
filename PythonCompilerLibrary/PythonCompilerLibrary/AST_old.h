#ifndef SUPERBUILD_AST_H
#define SUPERBUILD_AST_H
#include <string>
#include <variant>
#include <vector>
#include <memory>
#include <utility> // for std::pair

// Forward declare all nodes
struct NumberNode;
struct StringNode;
struct BinaryNode;
struct IdentNode;
struct UnaryExprNode;
struct BinaryExprNode;
struct CallExprNode;
struct AssignNode;
struct CollectionNode;
struct LineNode;
struct ProgramNode;
struct FunctionNode;
struct ClassNode;
struct IfNode;
struct WhileNode;
struct ForNode;
struct CaseNode;
struct MatchNode;
struct TryNode;

// ===== Pointers for recursion =====
using NodePtr = std::unique_ptr<struct NodeHolder>;
using CollectionNodePtr = std::unique_ptr<CollectionNode>;
using CallExprNodePtr = std::unique_ptr<CallExprNode>;
using AssignNodePtr = std::unique_ptr<AssignNode>;
using FunctionNodePtr = std::unique_ptr<FunctionNode>;
using ClassNodePtr = std::unique_ptr<ClassNode>;

// ===== Leaf / literal nodes =====
struct NumberNode { float number; };
struct StringNode { std::string str; };
struct BinaryNode { std::string numbr; };
struct IdentNode { std::string name; };

// ===== Expression nodes =====
struct UnaryExprNode { std::string operator_node; NodePtr value; };
struct BinaryExprNode { std::string operator_node; NodePtr left; NodePtr right; };

// ===== Collection nodes =====
struct CollectionNode; // forward declare
struct NodeHolder;     // forward declare

using LiteralNode = std::variant<
    NumberNode, StringNode, BinaryNode, CollectionNodePtr
>;

using ExprNode = std::variant<UnaryExprNode, BinaryExprNode>;

// HolderNode can be literal, expression, call, or collection
using HolderNode = std::variant<
    LiteralNode, ExprNode, CallExprNodePtr, CollectionNodePtr
>;

// Collections can be single values or key/value pairs
using CollectionHolderNode = std::variant<
    std::pair<HolderNode, HolderNode>, // dict item
    HolderNode                          // list/set/tuple element
>;

struct CollectionNode {
    std::vector<CollectionHolderNode> collection_nodes;
};

// ===== Call / assignment =====
struct CallExprNode {
    std::string name;
    std::vector<NodePtr> args; // recursive
};

struct AssignNode {
    std::string name;
    NodePtr value; // can be expression or collection
};

// ===== Line / Program =====
struct LineNode {
    std::vector<NodePtr> line;
};

struct ProgramNode {
    std::vector<LineNode> lines;
};

// ===== Function / Class =====
struct FunctionNode {
    std::string name;
    std::vector<NodePtr> args;
    std::vector<IdentNode> decorators;
    ProgramNode body;
};

struct ClassNode {
    std::string name;
    std::vector<NodePtr> args;
    std::vector<IdentNode> decorators;
    ProgramNode body;
};

// ===== Control flow =====
struct IfNode {
    NodePtr condition;
    ProgramNode if_body;
    bool has_else;
    ProgramNode else_body;
};

struct WhileNode {
    NodePtr condition;
    ProgramNode while_body;
    bool has_else;
    ProgramNode else_body;
};

struct ForNode {
    NodePtr iterator;
    ProgramNode for_body;
    bool has_else;
    ProgramNode else_body;
};

// ===== Match / Case =====
struct CaseNode {
    ProgramNode case_body;
    NodePtr case_line;
    std::unique_ptr<IfNode> guard; // optional
    std::vector<IdentNode> bindings;
};

struct MatchNode {
    std::vector<CaseNode> cases;
    NodePtr variable;
};

// ===== Try / Except =====
struct TryNode {
    ProgramNode try_body;
    bool has_except;
    ProgramNode except_body;
};

// ===== NodeHolder (top-level AST variant) =====
struct NodeHolder {
    using NodeVariant = std::variant<
        NumberNode,
        StringNode,
        BinaryNode,
        IdentNode,
        UnaryExprNode,
        BinaryExprNode,
        CallExprNodePtr,
        AssignNodePtr,
        CollectionNodePtr,
        FunctionNodePtr,
        ClassNodePtr,
        IfNode,
        WhileNode,
        ForNode,
        MatchNode,
        TryNode
    >;

    NodeVariant value;
};

#endif // SUPERBUILD_AST_H