#include "StatementNodes.h"

StatementNode *StatementNode::create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    TokenType token = peek(tokens, pos).type;
    switch (token) {
        case IF:
            return ctx.make<IfNode>(tokens, pos, ctx);
        case WHILE:
            return ctx.make<WhileNode>(tokens, pos, ctx);
        case LBRACE:
            return ctx.make<BlockNode>(tokens, pos, ctx);
        case ASSERT:
            return ctx.make<AssertNode>(tokens, pos, ctx);
        case ASSUME:
            return ctx.make<AssumeNode>(tokens, pos, ctx);
        case IDENT:
            return ctx.make<AssignNode>(tokens, pos, ctx);
        default:
            error(tokens, pos, "expected statement");
    }
}

BlockNode::BlockNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, LBRACE, "{");
    while (peek(tokens, pos).type != RBRACE) {
        nodes.push_back(StatementNode::create(tokens, pos, ctx));
    }
    expect(tokens, pos, RBRACE, "}");
}

AssignNode::AssignNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    target.push_back(AssignableNode::create(tokens, pos, ctx));
    while (peek(tokens, pos).type == COMMA) {
        advance(tokens, pos);
        target.push_back(AssignableNode::create(tokens, pos, ctx));
    }

    expect(tokens, pos, ASSIGN, "=");
    if (target.size() > 1)
        value = ctx.make<CallNode>(tokens, pos, ctx);
    else
        value = ExprNode::create(tokens, pos, ctx);

    expect(tokens, pos, SEMICOLON, ";");
}

IfNode::IfNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, IF, "if");

    expect(tokens, pos, LPAREN, "(");
    condition = BoolExprNode::create(tokens, pos, ctx, false);
    expect(tokens, pos, RPAREN, ")");
    thenNode = StatementNode::create(tokens, pos, ctx);

    if (peek(tokens, pos).type == ELSE) {
        advance(tokens, pos);
        elseNode = StatementNode::create(tokens, pos, ctx);
    }
}

WhileNode::WhileNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, WHILE, "while");

    expect(tokens, pos, LPAREN, "(");
    condition = BoolExprNode::create(tokens, pos, ctx, false);
    expect(tokens, pos, RPAREN, ")");

    if (peek(tokens, pos).type == INVARIANT) {
        advance(tokens, pos);
        invariant = BoolExprNode::create(tokens, pos, ctx, true);
    }

    body = StatementNode::create(tokens, pos, ctx);
}

AssertNode::AssertNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, ASSERT, "assert");
    predicate = BoolExprNode::create(tokens, pos, ctx, true);
    expect(tokens, pos, SEMICOLON, ";");
}

AssumeNode::AssumeNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, ASSUME, "assume");
    predicate = BoolExprNode::create(tokens, pos, ctx, true);
    expect(tokens, pos, SEMICOLON, ";");
}