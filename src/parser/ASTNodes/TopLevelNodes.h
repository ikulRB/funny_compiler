#ifndef TOP_LEVEL_NODES_H
#define TOP_LEVEL_NODES_H

#include <string>
#include <vector>
#include <utility>

#include "lexer.h"

#include "BaseNode.h"
#include "TypeNodes.h"
#include "BoolExprNodes.h"
#include "StatementNodes.h"

struct TopLevelNode : ASTNode { virtual ~TopLevelNode() = default; };

struct FunctionNode : TopLevelNode {
    std::string ident;
    std::vector<ParamNode*> params;
    BoolExprNode *requires_;
    std::vector<ParamNode*> returns;
    BoolExprNode *ensures;
    std::vector<LocalVarNode*> uses;
    StatementNode *body;

    FunctionNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct FormulaNode : TopLevelNode {
    std::string ident;
    std::vector<ParamNode*> params;
    BoolExprNode *predicate;

    FormulaNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct ModuleNode : ASTNode {
    std::vector<TopLevelNode*> nodes;

    ModuleNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

#endif //TOP_LEVEL_NODES_H