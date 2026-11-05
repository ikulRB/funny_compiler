#ifndef BASE_NODE_H
#define BASE_NODE_H

#include "ASTContext.h"
#include "lexer.h"
#include "tokenVector.h"

struct ASTNode {
    virtual ~ASTNode() = default;
};

#endif  // BASE_NODE_H