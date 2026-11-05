#include "ExprNodes.h"

unsigned endOfExpr(std::vector<Token> &tokens, unsigned pos) {
    int depth = 0;
    while (pos < tokens.size()) {
        TokenType token = peek(tokens, pos).type;
        if (depth == 0 && (token == EOF_TOKEN || token == SEMICOLON || token == COMMA
                           || token == RPAREN || token == RBRACKET
                           || token == LBRACE || token == RBRACE
                           || token == EQ || token == NEQ
                           || token == LT || token == LE
                           || token == GT || token == GE)) {
            return pos;
        }
        if (token == LPAREN || token == LBRACKET || token == LBRACE)
            depth++;
        else if (token == RPAREN || token == RBRACKET || token == RBRACE)
            depth--;
        
        pos++;
    }
    return pos;
}

int findLowestOp(std::vector<Token> &tokens, unsigned pos, unsigned end) {
    int depth = 0;
    int lastAdd = -1;
    int lastMul = -1;

    for (unsigned i = 0; end >= pos && i < end - pos; i++) {
        TokenType token = peek(tokens, pos + i).type;

        if (token == LPAREN || token == LBRACKET || token == LBRACE) { depth++; continue; }
        if (token == RPAREN || token == RBRACKET || token == RBRACE) { depth--; continue; }
        if (depth != 0) continue;

        if (token == PLUS || token == MINUS) {
            if (i == 0) continue;
            
            TokenType prev = peek(tokens, pos + i - 1).type;
            if (prev == PLUS || prev == MINUS || prev == STAR || prev == SLASH ||
                prev == LPAREN || prev == LBRACKET || prev == COMMA || 
                prev == ASSIGN || (prev >= EQ && prev <= GE)) {
                continue;
            }
            
            lastAdd = i;
        }
        else if (token == STAR || token == SLASH)
            lastMul = i;
    }
    if (lastAdd != -1) return lastAdd;
    return lastMul;
}

ExprNode *parseExpr(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, unsigned end);

ExprNode *parseAtom(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, unsigned end) {
    if (pos >= end) error(tokens, pos, "expected expression");

    TokenType token = peek(tokens, pos).type;
    switch (token) {
        case NUMBER:
            return ctx.make<NumberNode>(tokens, pos);

        case MINUS:
            return ctx.make<UnaryOpNode>(tokens, pos, ctx);

        case LPAREN: {
            advance(tokens, pos);
            unsigned exprEnd = endOfExpr(tokens, pos);
            ExprNode *expr = parseExpr(tokens, pos, ctx, exprEnd);
            expect(tokens, pos, RPAREN, ")");
            return expr;
        }
        case IDENT:
            if (pos + 1 < end && peek(tokens, pos + 1).type == LPAREN)
                return ctx.make<CallNode>(tokens, pos, ctx);
            if (pos + 1 < end && peek(tokens, pos + 1).type == LBRACKET)
                return ctx.make<ArrayAccessNode>(tokens, pos, ctx);
            return ctx.make<VarNode>(tokens, pos);

        default:
            error(tokens, pos, "expected expression");
    }
}

ExprNode *parseExpr(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, unsigned end) {
    int relativeOpPos = findLowestOp(tokens, pos, end);
    if (relativeOpPos == -1) return parseAtom(tokens, pos, ctx, end);

    unsigned opPos = pos + relativeOpPos;
    char op = peek(tokens, opPos).value[0];

    unsigned leftPos = pos;
    ExprNode *left = parseExpr(tokens, leftPos, ctx, opPos);

    unsigned rightPos = opPos + 1;
    ExprNode *right = parseExpr(tokens, rightPos, ctx, end);

    pos = end;
    return ctx.make<BinOpNode>(op, left, right);
}

ExprNode *ExprNode::create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    unsigned end = endOfExpr(tokens, pos);
    return parseExpr(tokens, pos, ctx, end);
}

AssignableNode *AssignableNode::create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    if (peek(tokens, pos + 1).type == LBRACKET) return ctx.make<ArrayAccessNode>(tokens, pos, ctx);
    return ctx.make<VarNode>(tokens, pos);
}

VarNode::VarNode(std::vector<Token> &tokens, unsigned &pos) {
    ident = expect(tokens, pos, IDENT).value;
}

ArrayAccessNode::ArrayAccessNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    ident = expect(tokens, pos, IDENT).value;
    while (peek(tokens, pos).type == LBRACKET) {
        advance(tokens, pos);
        indexes.push_back(ExprNode::create(tokens, pos, ctx));
        expect(tokens, pos, RBRACKET);
    }
}

NumberNode::NumberNode(std::vector<Token> &tokens, unsigned &pos) {
    value = std::stoul(expect(tokens, pos, NUMBER, "number").value);
}

UnaryOpNode::UnaryOpNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    op = '-';
    advance(tokens, pos);
    operand = parseAtom(tokens, pos, ctx, endOfExpr(tokens, pos));
}

CallNode::CallNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    ident = expect(tokens, pos, IDENT).value;
    expect(tokens, pos, LPAREN, "(");

    if (peek(tokens, pos).type != RPAREN) {
        args.push_back(ExprNode::create(tokens, pos, ctx));
        while (peek(tokens, pos).type == COMMA) {
            advance(tokens, pos);
            args.push_back(ExprNode::create(tokens, pos, ctx));
        }
    }
    expect(tokens, pos, RPAREN, ")");
}