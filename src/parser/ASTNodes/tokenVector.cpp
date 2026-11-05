#include <stdexcept>
#include "tokenVector.h"

Token &peek(std::vector<Token> &tokens, unsigned pos) {
    return tokens[pos];
}

void advance(std::vector<Token> &tokens, unsigned &pos) {
    if (pos < tokens.size() - 1) pos++;
}

Token expect(std::vector<Token> &tokens, unsigned &pos, TokenType type, const std::string &value) {
    Token &t = tokens[pos];
    if (t.type != type) {
        throw std::runtime_error(
            "expected " + (value.empty() ? "another token" : "\"" + value + "\"") +
            " at " + std::to_string(t.line) + ":" + std::to_string(t.column)
        );
    }
    Token copy = t;
    pos++;
    return copy;
}

void error(std::vector<Token> &tokens, unsigned pos, const std::string &msg) {
    Token &t = tokens[pos];
    throw std::runtime_error(
        msg + " at " + std::to_string(t.line) + ":" + std::to_string(t.column)
    );
}
