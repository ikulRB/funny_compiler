#include "TypeNodes.h"

TypeNode::TypeNode(std::vector<Token> &tokens, unsigned &pos) {
    expect(tokens, pos, INT, "int");

    if (peek(tokens, pos).type == LBRACKET) {
        advance(tokens, pos);
        expect(tokens, pos, RBRACKET, "]");
        type = ARRAY_INTEGER;
    }
    else type = INTEGER;
}

ParamNode::ParamNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    ident = expect(tokens, pos, IDENT).value;
    expect(tokens, pos, COLON, ":");
    type = ctx.make<TypeNode>(tokens, pos);
}

LocalVarNode::LocalVarNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    ident = expect(tokens, pos, IDENT).value;

    if (peek(tokens, pos).type == COLON) {
        advance(tokens, pos);
        type = ctx.make<TypeNode>(tokens, pos);
    }
}