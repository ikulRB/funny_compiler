#ifndef TYPE_NODES_H
#define TYPE_NODES_H

#include <string>
#include <utility>

#include "BaseNode.h"

struct TypeNode : ASTNode {
    enum Type { INTEGER, ARRAY_INTEGER };
    Type type;

    TypeNode(std::vector<Token> &tokens, unsigned &pos);
};

struct ParamNode : ASTNode {
    std::string ident;
    TypeNode *type;

    ParamNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

struct LocalVarNode : ASTNode {
    std::string ident;
    TypeNode *type;

    LocalVarNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx);
};

#endif //TYPE_NODES_H