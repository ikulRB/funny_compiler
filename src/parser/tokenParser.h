#ifndef TOKEN_PARSER_H
#define TOKEN_PARSER_H

#include "lexer.h"
#include "ASTNodes/ASTContext.h"
#include "ASTNodes/TopLevelNodes.h"

ModuleNode *parseToAST(std::vector<Token> &tokens, ASTContext &ctx);

#endif //TOKEN_PARSER_H