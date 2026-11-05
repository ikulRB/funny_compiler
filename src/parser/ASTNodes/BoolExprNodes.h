#ifndef BOOL_EXPR_NODES_H
#define BOOL_EXPR_NODES_H

#include <string>
#include <vector>
#include <utility>

#include "BaseNode.h"
#include "TypeNodes.h"
#include "ExprNodes.h"

struct BoolExprNode : ASTNode { 
    virtual ~BoolExprNode() = default;

    static BoolExprNode *create(std::vector<Token> &tokens, unsigned &pos,
                                ASTContext &ctx, bool allowQuantifiers);
};

struct BoolNode : BoolExprNode {
    bool value;

    BoolNode(bool value) : value(value) {}
};

struct LogicNode : BoolExprNode {
    enum LogicOperator { NOT, AND, OR, IMPLICATION };
    LogicOperator op;
    std::vector<BoolExprNode*> operands;

    LogicNode(LogicOperator op, BoolExprNode *operand);
    LogicNode(LogicOperator op, BoolExprNode *left, BoolExprNode *right);
};

struct ComparisonNode : BoolExprNode {
    enum CompareOperator { EQ, NEQ, LT, LE, GT, GE };
    CompareOperator op;
    ExprNode *left;
    ExprNode *right;

    ComparisonNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct QuantifierNode : BoolExprNode {
    enum Quantifier { EXISTS, FORALL };
    Quantifier quantifier;
    ParamNode *var;
    BoolExprNode *predicate;

    QuantifierNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct FormulaRefNode : BoolExprNode {
    std::string ident;
    std::vector<ExprNode*> args;

    FormulaRefNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

#endif //BOOL_EXPR_NODES_H