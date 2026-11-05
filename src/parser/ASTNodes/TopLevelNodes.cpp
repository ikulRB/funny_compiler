#include "TopLevelNodes.h"

FunctionNode::FunctionNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, FUNCTION, "function");
    ident = expect(tokens, pos, IDENT).value;

    expect(tokens, pos, LPAREN, "(");
    while (peek(tokens, pos).type != RPAREN) {
        params.push_back(ctx.make<ParamNode>(tokens, pos, ctx));
        if (peek(tokens, pos).type == COMMA)
            advance(tokens, pos);
        else
            break;
    }
    expect(tokens, pos, RPAREN, ")");

    Token token = peek(tokens, pos);
    if (token.type == REQUIRES) {
        advance(tokens, pos);
        requires_ = BoolExprNode::create(tokens, pos, ctx, true);
    }

    expect(tokens, pos, RETURNS, "returns");
    returns.push_back(ctx.make<ParamNode>(tokens, pos, ctx));
    while (peek(tokens, pos).type == COMMA) {
        advance(tokens, pos);
        returns.push_back(ctx.make<ParamNode>(tokens, pos, ctx));
    }
    
    token = peek(tokens, pos);
    if (token.type == ENSURES) {
        advance(tokens, pos);
        ensures = BoolExprNode::create(tokens, pos, ctx, true);
    }

    token = peek(tokens, pos);
    if (token.type == USES) {
        advance(tokens, pos);
        uses.push_back(ctx.make<LocalVarNode>(tokens, pos, ctx));
        while (peek(tokens, pos).type == COMMA) {
            advance(tokens, pos);
            uses.push_back(ctx.make<LocalVarNode>(tokens, pos, ctx));
        }
    }

    body = StatementNode::create(tokens, pos, ctx);
}

FormulaNode::FormulaNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    expect(tokens, pos, FORMULA, "formula");
    ident = expect(tokens, pos, IDENT).value;

    expect(tokens, pos, LPAREN, "(");
    while (peek(tokens, pos).type != RPAREN) {
        params.push_back(ctx.make<ParamNode>(tokens, pos, ctx));
        if (peek(tokens, pos).type == COMMA)
            advance(tokens, pos);
        else
            break;
    }
    expect(tokens, pos, RPAREN, ")");

    expect(tokens, pos, FAT_ARROW, "=>");
    predicate = BoolExprNode::create(tokens, pos, ctx, true);
}

ModuleNode::ModuleNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    while (peek(tokens, pos).type != EOF_TOKEN) {
        TopLevelNode *node = nullptr;
        switch (peek(tokens, pos).type) {
            case FUNCTION: node = ctx.make<FunctionNode>(tokens, pos, ctx); break;
            case FORMULA:  node = ctx.make<FormulaNode>(tokens, pos, ctx);  break;
            default:       error(tokens, pos, "expected function or formula");
        }
        nodes.push_back(node);
    }
}