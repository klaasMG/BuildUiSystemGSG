#ifndef SUPERBUILD_AST_H
#define SUPERBUILD_AST_H
#include <variant>

struct NumberNode;
struct StringNode;
struct CallExprNode;
struct ExprNode;
struct AssignNode;
struct FunctionNode;
struct ClassNode;
struct IdentNode;
struct BinaryExprNode;
struct UnaryExprNode;
struct BinaryNode;
struct IfNode;
struct WhileNode;
struct ForNode;
struct MatchNode;
struct TryNode;

using Node = std::variant<NumberNode, StringNode, CallExprNode, ExprNode, AssignNode, FunctionNode, ClassNode, IdentNode, BinaryExprNode, UnaryExprNode, BinaryNode, IfNode, WhileNode, ForNode, MatchNode, TryNode>;

#endif //SUPERBUILD_AST_H