#include <iostream>
#include <cctype>
#include <stdexcept>

#include "tokenParser.h"

ModuleNode *parseToAST(std::vector<Token> &tokens, ASTContext &ctx) {
    unsigned pos = 0;
    ModuleNode *module = ctx.make<ModuleNode>(tokens, pos, ctx);
    return module;
}