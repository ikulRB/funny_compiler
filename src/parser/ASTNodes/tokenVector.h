#ifndef TOKEN_VECTOR_H
#define TOKEN_VECTOR_H

#include "lexer.h"

Token &peek(std::vector<Token> &tokens, unsigned pos);
void advance(std::vector<Token> &tokens, unsigned &pos);
Token expect(std::vector<Token> &tokens, unsigned &pos, TokenType type, const std::string &value = "");
[[noreturn]] void error(std::vector<Token> &tokens, unsigned pos, const std::string &msg);

#endif //TOKEN_VECTOR_H