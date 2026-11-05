#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>

#include "lexer.h"
#include "tokenParser.h"
#include "ASTNodes/TopLevelNodes.h"

int main(int argc, char* argv[]) {
    if (argc < 2)
        throw std::runtime_error("Bad argument line: expected file name");

    std::ifstream file(argv[1]);
    if (!file.is_open())
        throw std::runtime_error(std::string("File ") + argv[1] + " does not exist");

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string text = buffer.str();
    file.close();

    std::vector<Token> tokens = tokenize(text);
    ASTContext ctx;
    ModuleNode *AST = parseToAST(tokens, ctx);

    return 0;
}
