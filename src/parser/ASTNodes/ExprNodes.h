#ifndef EXPR_NODES_H
#define EXPR_NODES_H

#include <string>
#include <vector>
#include <utility>

#include "BaseNode.h"
#include "TypeNodes.h"

struct ExprNode : ASTNode { 
    virtual ~ExprNode() = default; 

    static ExprNode *create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct AssignableNode : ExprNode { 
    virtual ~AssignableNode() = default; 

    static AssignableNode *create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct VarNode : AssignableNode {
    std::string ident;

    VarNode(std::vector<Token> &tokens, unsigned &pos);
};

struct ArrayAccessNode : AssignableNode {
    std::string ident;
    std::vector<ExprNode*> indexes;

    ArrayAccessNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct NumberNode : ExprNode {
    unsigned value;

    NumberNode(std::vector<Token> &tokens, unsigned &pos);
};

struct BinOpNode : ExprNode {
    char op;
    ExprNode *left;
    ExprNode *right;

    BinOpNode(char op, ExprNode *left, ExprNode *right)
              : op(op), left(left), right(right) {}
};

struct UnaryOpNode : ExprNode {
    char op;
    ExprNode *operand;

    UnaryOpNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct CallNode : ExprNode {
    std::string ident;
    std::vector<ExprNode*> args;

    CallNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

#endif //EXPR_NODES_H