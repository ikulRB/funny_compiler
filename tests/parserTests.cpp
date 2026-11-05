#include <iostream>
#include <string>
#include <vector>

#include "TestFramework.h"

#include "../src/lexer/lexer.h"
#include "../src/parser/tokenParser.h"
#include "../src/parser/ASTNodes/ASTContext.h"
#include "../src/parser/ASTNodes/TopLevelNodes.h"

// ожидание: разбор без ошибки
void expectParse(const std::string &source) {
    auto tokens = tokenize(source);
    ASTContext ctx;
    unsigned pos = 0;
    try {
        ctx.make<ModuleNode>(tokens, pos, ctx);
    } catch (const std::exception &e) {
        throw std::runtime_error("unexpected error: " + std::string(e.what()));
    }
}

// ожидание: разбор с ошибкой (проверяем, что упало)
void expectError(const std::string &source) {
    try {
        auto tokens = tokenize(source);
        ASTContext ctx;
        unsigned pos = 0;
        ctx.make<ModuleNode>(tokens, pos, ctx);
    } catch (const std::exception &) {
        return;
    }
    throw std::runtime_error("expected error, but parsed successfully");
}

int main() {
    TestRunner t;

    // ------- позитивные -------
    t.test("empty module", [] { expectParse(""); });
    t.test("simple function", [] { expectParse("function f() returns x: int { x = 1; }"); });
    t.test("function with params", [] {
        expectParse("function add(a: int, b: int) returns c: int { c = a + b; }");
    });
    t.test("requires / ensures", [] {
        expectParse("function f(a: int) requires a >= 0 returns r: int ensures r == a { r = a; }");
    });
    t.test("uses", [] {
        expectParse("function f(a: int) returns r: int uses tmp: int { tmp = a; r = tmp; }");
    });
    t.test("if with else", [] {
        expectParse("function f(x: int) returns r: int { if (x > 0) r = 1; else r = 0; }");
    });
    t.test("if without else", [] {
        expectParse("function f(x: int) returns r: int { if (x > 0) r = 1; }");
    });
    t.test("while with invariant", [] {
        expectParse("function f(n: int) returns s: int { "
                    "s = 0; i = 0; "
                    "while (i < n) invariant 0 <= i and i <= n { s = s + i; i = i + 1; } "
                    "}");
    });
    t.test("assert", [] {
        expectParse("function f(x: int) returns r: int { assert x >= 0; r = x; }");
    });
    t.test("assume", [] {
        expectParse("function f(x: int) returns r: int { assume x >= 0; r = x; }");
    });
    t.test("array access", [] {
        expectParse("function f(a: int[], i: int) returns r: int { r = a[i]; }");
    });
    t.test("array update as sugar", [] {
        expectParse("function f(a: int[], i: int) returns r: int { a[i] = 42; r = a[i]; }");
    });
    t.test("multiple returns", [] {
        expectParse("function f(a: int) returns x: int, y: int { x = a; y = a; }");
    });
    t.test("tuple assignment", [] {
        expectParse("function f() returns x: int, y: int { x, y = g(); }");
    });
    t.test("expression precedence", [] {
        expectParse("function f() returns r: int { r = 1 + 2 * 3; }");
    });
    t.test("unary minus", [] {
        expectParse("function f(x: int) returns r: int { r = -x + 1; }");
    });
    t.test("call expression", [] {
        expectParse("function f(x: int) returns r: int { r = g(x) + 1; }");
    });
    t.test("formula", [] {
        expectParse("formula nonNegative(x: int) => x >= 0");
    });
    t.test("quantifier in requires", [] {
        expectParse("function f(a: int[], n: int) requires forall(i: int | 0 <= i and i < n -> a[i] >= 0) "
                    "returns r: int { r = 0; }");
    });
    t.test("nested quantifier", [] {
        expectParse("function f(a: int[], n: int) "
                    "requires forall(i: int | 0 <= i and i < n -> exists(j: int | i < j and a[j] >= 0)) "
                    "returns r: int { r = 0; }");
    });
    t.test("formula ref in requires", [] {
        expectParse("formula nonNegative(x: int) => x >= 0\n"
                    "function f(a: int) requires nonNegative(a) returns r: int { r = a; }");
    });

    // ------- дополнительные позитивные -------
    t.test("empty function block", [] {
        expectParse("function f() returns x: int { }");
    });
    t.test("nested if", [] {
        expectParse("function f(x: int, y: int) returns r: int { "
                    "if (x > 0) if (y > 0) r = 1; else r = 2; "
                    "}");
    });
    t.test("if with block", [] {
        expectParse("function f(x: int) returns r: int { "
                    "if (x > 0) { r = 1; r = r + 1; } "
                    "}");
    });
    t.test("empty while block", [] {
        expectParse("function f(n: int) returns r: int { "
                    "while (r < n) invariant r <= n { } "
                    "}");
    });
    t.test("deeply nested parens", [] {
        expectParse("function f() returns r: int { r = ((((1 + 2)))); }");
    });
    t.test("multiple arithmetic ops", [] {
        expectParse("function f(a: int, b: int, c: int) returns r: int { r = a + b * c - a / b; }");
    });
    t.test("unary minus on call", [] {
        expectParse("function f() returns r: int { r = -g(1, 2); }");
    });
    t.test("unary minus on array", [] {
        expectParse("function f(a: int[]) returns r: int { r = -a[0] + 1; }");
    });
    t.test("complex logic in requires", [] {
        expectParse("function f(a: int, b: int) "
                    "requires not (a > 0) or (b == 0 and a < 10) "
                    "returns r: int { r = 0; }");
    });
    t.test("implication in ensures", [] {
        expectParse("function f(a: int, b: int) returns r: int "
                    "ensures a == 0 -> b == 0 { r = 0; }");
    });
    t.test("quantifier with block", [] {
        // ИСПРАВЛЕНО: requires стоит ДО returns
        expectParse("function f(a: int[]) "
                    "requires exists(i: int | 0 <= i and i < length(a) and a[i] == 0) "
                    "returns r: int { r = 0; }");
    });
    t.test("multi-arg call", [] {
        expectParse("function f() returns r: int { r = g(1, 2, 3); }");
    });
    t.test("multi-dim array access", [] {
        expectParse("function f(a: int[]) returns r: int { r = a[a[0]]; }");
    });
    t.test("formula with multiple params", [] {
        expectParse("formula sum(x: int, y: int) => x + y == y + x");
    });
    t.test("formula with quantifier", [] {
        expectParse("formula allPositive(a: int[]) => forall(i: int | 0 <= i and i < length(a) -> a[i] > 0)");
    });
    t.test("only spaces", [] {
        expectParse("   \n\t  \n ");
    });
    t.test("only comments", [] {
        expectParse("// this is a comment\n// another comment");
    });

    // ------- негативные -------
    t.test("garbage input", [] { expectError("@#$"); });
    t.test("unclosed paren", [] { expectError("function f() returns r: int { r = (1 + 2; }"); });
    t.test("unclosed brace", [] { expectError("function f() returns r: int { r = 1; "); });
    t.test("missing semicolon", [] { expectError("function f() returns r: int { r = 1 }"); });
    t.test("missing returns", [] { expectError("function f() { }"); });
    t.test("invalid operator", [] { expectError("function f() returns r: int { r = 1 ** 2; }"); });
    t.test("extra tokens", [] { expectError("function f() returns r: int { r = 1; } @"); });
    t.test("quantifier in if", [] {
        expectError("function f(n: int) returns r: int { if (forall(i: int | i > 0)) r = 1; }");
    });
    t.test("unclosed bracket", [] {
        expectError("function f(a: int[], i: int) returns r: int { r = a[i; }");
    });
    t.test("if with number condition", [] {
        expectError("function f() returns r: int { if (1) r = 1; }");
    });

    // ------- дополнительные негативные -------
    t.test("quantifier missing pipe", [] {
        expectError("function f() requires forall(i: int 0 <= i) returns r: int { r = 0; }");
    });
    t.test("quantifier missing type", [] {
        expectError("function f() requires forall(i | 0 <= i) returns r: int { r = 0; }");
    });
    t.test("formula missing arrow", [] {
        expectError("formula f(x: int) x >= 0");
    });
    t.test("empty if condition", [] {
        expectError("function f() returns r: int { if () r = 1; }");
    });
    t.test("empty while condition", [] {
        expectError("function f() returns r: int { while () r = 1; }");
    });
    t.test("array access missing index", [] {
        expectError("function f(a: int[]) returns r: int { r = a[]; }");
    });
    t.test("array access missing bracket", [] {
        expectError("function f(a: int[]) returns r: int { r = a[0; }");
    });
    t.test("double comma in params", [] {
        expectError("function f(a: int,, b: int) returns r: int { r = 0; }");
    });
    t.test("double comma in args", [] {
        expectError("function f() returns r: int { r = g(1,, 2); }");
    });
    t.test("trailing comma in returns", [] {
        expectError("function f() returns r: int, { r = 0; }");
    });
    t.test("assign to number", [] {
        expectError("function f() returns r: int { 1 = r; }");
    });
    t.test("missing colon in param", [] {
        expectError("function f(a int) returns r: int { r = 0; }");
    });
    t.test("wrong type keyword", [] {
        expectError("function f(a: float) returns r: int { r = 0; }");
    });
    t.test("mismatched braces", [] {
        expectError("function f() returns r: int { { r = 0; }");
    });
    t.test("else without if", [] {
        expectError("function f() returns r: int { else r = 0; }");
    });
    t.test("unclosed function", [] {
        expectError("function f() returns r: int { r = 0;");
    });
    t.test("missing function name", [] {
        expectError("function () returns r: int { r = 0; }");
    });
    t.test("missing returns keyword", [] {
        expectError("function f() r: int { r = 0; }");
    });
    t.test("keyword as ident", [] {
        expectError("function if() returns r: int { r = 0; }");
    });
    t.test("invalid character in expr", [] {
        expectError("function f() returns r: int { r = 1 $ 2; }");
    });
    t.test("missing expr after plus", [] {
        expectError("function f() returns r: int { r = 1 + ; }");
    });
    t.test("missing expr after minus", [] {
        expectError("function f() returns r: int { r = - ; }");
    });

    return t.summary();
}