#include "BoolExprNodes.h"

unsigned endOfBoolExpr(std::vector<Token> &tokens, unsigned pos) {
    int depth = 0;
    while (pos < tokens.size()) {
        TokenType token = peek(tokens, pos).type;
        if (depth == 0 && (token == EOF_TOKEN || token == SEMICOLON || token == COMMA
                           || token == RPAREN || token == RBRACKET
                           || token == LBRACE || token == RBRACE
                           || token == RETURNS)) {
            return pos;
        }
        if (token == LPAREN || token == LBRACKET || token == LBRACE) depth++;
        else if (token == RPAREN || token == RBRACKET || token == RBRACE) depth--;
        
        pos++;
    }
    return pos;
}

int findLowestBoolOp(std::vector<Token> &tokens, unsigned pos, unsigned end) {
    int depth = 0;
    int firstImp = -1;
    int lastOr = -1;
    int lastAnd = -1;

    for (unsigned i = 0; end >= pos && i < end - pos; i++) {
        TokenType token = peek(tokens, pos + i).type;

        if (token == LPAREN || token == LBRACKET || token == LBRACE) { depth++; continue; }
        if (token == RPAREN || token == RBRACKET || token == RBRACE) { depth--; continue; }
        if (depth != 0) continue;

        if (token == ARROW) { if (firstImp == -1) firstImp = i; }
        else if (token == OR) lastOr = i;
        else if (token == AND) lastAnd = i;
    }

    if (firstImp != -1) return firstImp;
    if (lastOr != -1) return lastOr;
    return lastAnd;
}

BoolExprNode *parseBoolExpr(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, unsigned end, bool allowQuantifiers);

BoolExprNode *parseBoolAtom(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, unsigned end, bool allowQuantifiers) {
    if (pos >= end) error(tokens, pos, "expected expression");

    TokenType token = peek(tokens, pos).type;
    switch (token) {
        case TRUE:
            advance(tokens, pos);
            return ctx.make<BoolNode>(true);
        case FALSE:
            advance(tokens, pos);
            return ctx.make<BoolNode>(false);

        case NOT: {
            advance(tokens, pos);
            BoolExprNode *operand = parseBoolAtom(tokens, pos, ctx, end, allowQuantifiers);
            return ctx.make<LogicNode>(LogicNode::NOT, operand);
        }

        case LPAREN: {
            advance(tokens, pos);
            unsigned innerEnd = endOfBoolExpr(tokens, pos);
            BoolExprNode *expr = parseBoolExpr(tokens, pos, ctx, innerEnd, allowQuantifiers);
            expect(tokens, pos, RPAREN, ")");
            return expr;
        }

        case FORALL:
        case EXISTS:
            if (allowQuantifiers) return ctx.make<QuantifierNode>(tokens, pos, ctx);
            error(tokens, pos, "quantifier not allowed here");

        case IDENT:
            if (pos + 1 < end && peek(tokens, pos + 1).type == LPAREN)
                return ctx.make<FormulaRefNode>(tokens, pos, ctx);
            return ctx.make<ComparisonNode>(tokens, pos, ctx);

        case NUMBER:
        case MINUS:
            return ctx.make<ComparisonNode>(tokens, pos, ctx);

        default:
            error(tokens, pos, "expected boolean expression");
    }
}

BoolExprNode *parseBoolExpr(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, unsigned end, bool allowQuantifiers) {
    int relativeOpPos = findLowestBoolOp(tokens, pos, end);
    if (relativeOpPos == -1) return parseBoolAtom(tokens, pos, ctx, end, allowQuantifiers);

    unsigned opPos = pos + relativeOpPos;
    TokenType opType = peek(tokens, opPos).type;

    unsigned leftPos = pos;
    BoolExprNode *left = parseBoolExpr(tokens, leftPos, ctx, opPos, allowQuantifiers);

    unsigned rightPos = opPos + 1;
    BoolExprNode *right = parseBoolExpr(tokens, rightPos, ctx, end, allowQuantifiers);

    pos = end;
    
    switch (opType) {
        case ARROW: return ctx.make<LogicNode>(LogicNode::IMPLICATION, left, right);
        case AND:   return ctx.make<LogicNode>(LogicNode::AND, left, right);
        case OR:    return ctx.make<LogicNode>(LogicNode::OR, left, right);
        default:    error(tokens, pos, "unexpected operator");
    }
}

BoolExprNode *BoolExprNode::create(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx, bool allowQuantifiers) {
    unsigned end = endOfBoolExpr(tokens, pos);
    return parseBoolExpr(tokens, pos, ctx, end, allowQuantifiers);
}

LogicNode::LogicNode(LogicOperator op, BoolExprNode *operand) : op(op) {
    operands.push_back(operand);
}

LogicNode::LogicNode(LogicOperator op, BoolExprNode *left, BoolExprNode *right) : op(op) {
    operands.push_back(left);
    operands.push_back(right);
}

ComparisonNode::ComparisonNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    left = ExprNode::create(tokens, pos, ctx);

    Token token = peek(tokens, pos);
    switch (token.type) {
        case TokenType::EQ:  op = CompareOperator::EQ;  break;
        case TokenType::NEQ: op = CompareOperator::NEQ; break;
        case TokenType::LT:  op = CompareOperator::LT;  break;
        case TokenType::LE:  op = CompareOperator::LE;  break;
        case TokenType::GT:  op = CompareOperator::GT;  break;
        case TokenType::GE:  op = CompareOperator::GE;  break;

        default:
            error(tokens, pos, "expected comparasion operator");
    }
    advance(tokens, pos);
    right = ExprNode::create(tokens, pos, ctx);
}

QuantifierNode::QuantifierNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
    Token token = peek(tokens, pos);
    switch (token.type) {
        case TokenType::FORALL: quantifier = Quantifier::FORALL; break;
        case TokenType::EXISTS: quantifier = Quantifier::EXISTS; break;

        default:
            error(tokens, pos, "expected quantifier");
    }
    advance(tokens, pos);

    expect(tokens, pos, LPAREN, "(");
    var = ctx.make<ParamNode>(tokens, pos, ctx);
    expect(tokens, pos, PIPE, "|");
    predicate = BoolExprNode::create(tokens, pos, ctx, true);
    expect(tokens, pos, RPAREN, ")");
}

FormulaRefNode::FormulaRefNode(std::vector<Token> &tokens, unsigned &pos, ASTContext &ctx) {
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