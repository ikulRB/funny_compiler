#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

#define TOKEN_LIST(X) \
    /* dynamic tokens */ \
    X(IDENT,       "IDENT") \
    X(NUMBER,      "NUMBER") \
    /* keywords - types */ \
    X(INT,         "int") \
    /* keywords - functional words */ \
    X(FUNCTION,    "function") \
    X(FORMULA,     "formula") \
    X(REQUIRES,    "requires") \
    X(RETURNS,     "returns") \
    X(ENSURES,     "ensures") \
    X(USES,        "uses") \
    /* keywords - control structs */ \
    X(IF,          "if") \
    X(ELSE,        "else") \
    X(WHILE,       "while") \
    X(INVARIANT,   "invariant") \
    /* keywords - verify operators */ \
    X(ASSERT,      "assert") \
    X(ASSUME,      "assume") \
    /* keywords - logic */ \
    X(TRUE,        "true") \
    X(FALSE,       "false") \
    X(NOT,         "not") \
    X(AND,         "and") \
    X(OR,          "or") \
    X(FORALL,      "forall") \
    X(EXISTS,      "exists") \
    /* separators */ \
    X(LPAREN,      "(") \
    X(RPAREN,      ")") \
    X(LBRACE,      "{") \
    X(RBRACE,      "}") \
    X(LBRACKET,    "[") \
    X(RBRACKET,    "]") \
    X(COLON,       ":") \
    X(PIPE,        "|") \
    X(COMMA,       ",") \
    X(SEMICOLON,   ";") \
    /* arithmetic operators */ \
    X(PLUS,        "+") \
    X(MINUS,       "-") \
    X(STAR,        "*") \
    X(SLASH,       "/") \
    /* compare operators*/ \
    X(EQ,          "==") \
    X(NEQ,         "!=") \
    X(LT,          "<") \
    X(LE,          "<=") \
    X(GT,          ">") \
    X(GE,          ">=") \
    /* assign operators */ \
    X(ASSIGN,      "=") \
    /* arrows */ \
    X(ARROW,       "->") \
    X(FAT_ARROW,   "=>") \
    /* markers */ \
    X(EOF_TOKEN,   "EOF")

typedef enum TokenType {
    #define GENERATE_ENUM(name, string) name,
    TOKEN_LIST(GENERATE_ENUM)
    #undef GENERATE_ENUM
} TokenType;

inline const std::string TokenTypeNames[] = {
    #define GENERATE_STRING(name, string) string,
    TOKEN_LIST(GENERATE_STRING)
    #undef GENERATE_STRING
};

typedef struct Token {
    TokenType type;
    std::string value;
    unsigned line, column;
} Token;

enum DFAState {
    S_START,
    S_IDENT,
    S_NUMBER,
    S_SLASH,
    S_COMMENT,
    S_EQ,
    S_NEQ,
    S_LT,
    S_GT,
    S_MINUS,
    S_TRAP
};

inline std::unordered_map<std::string, TokenType> tokensMap;

std::vector<Token> tokenize(std::string text);

#endif //LEXER_H