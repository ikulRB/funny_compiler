#ifndef STATEMENT_NODES_H
#define STATEMENT_NODES_H

#include <string>
#include <vector>
#include <utility>

#include "BaseNode.h"
#include "ExprNodes.h"
#include "BoolExprNodes.h"

struct StatementNode : ASTNode {
    virtual ~StatementNode() = default;

    static StatementNode *create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct BlockNode : StatementNode {
    std::vector<StatementNode*> nodes;

    BlockNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct AssignNode : StatementNode {
    std::vector<AssignableNode*> target;
    ExprNode *value;

    AssignNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct IfNode : StatementNode {
    BoolExprNode *condition;
    StatementNode *thenNode;
    StatementNode *elseNode;

    IfNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct WhileNode : StatementNode {
    BoolExprNode *condition;
    BoolExprNode *invariant;
    StatementNode *body;

    WhileNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct AssertNode : StatementNode {
    BoolExprNode *predicate;

    AssertNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct AssumeNode : StatementNode {
    BoolExprNode *predicate;

    AssumeNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

#endif //STATEMENT_NODES_H