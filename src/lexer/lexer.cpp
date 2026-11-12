#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include "lexer.h"

void initTokensMap() {    
    if (tokensMap.empty()) {
        for (int i = 0; i < EOF_TOKEN; i++) {
            if (i == IDENT || i == NUMBER) continue;
            tokensMap[TokenTypeNames[i]] = static_cast<TokenType>(i);
        }
    }
}

inline bool isDigit(char c) { return c >= '0' && c <= '9'; }
inline bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
inline bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }

DFAState transition(DFAState state, char c) {
    switch (state) {
        case S_START:
            if (isAlpha(c)) return S_IDENT;
            if (isDigit(c)) return S_NUMBER;
            if (c == '/')   return S_SLASH;
            if (c == '=')   return S_EQ;
            if (c == '!')   return S_NEQ;
            if (c == '<')   return S_LT;
            if (c == '>')   return S_GT;
            if (c == '-')   return S_MINUS;
            return S_TRAP;

        case S_IDENT:
            if (isAlpha(c) || isDigit(c)) return S_IDENT;
            return S_TRAP;

        case S_NUMBER:
            if (isDigit(c)) return S_NUMBER;
            return S_TRAP;

        case S_SLASH:
            if (c == '/') return S_COMMENT;
            return S_TRAP;
            
        case S_COMMENT:
            if (c == '\n') return S_TRAP;
            return S_COMMENT;

        case S_EQ:
            if (c == '=') return S_TRAP;
            if (c == '>') return S_TRAP;
            return S_TRAP;

        case S_NEQ:
            if (c == '=') return S_TRAP;
            return S_TRAP;

        case S_LT:
            if (c == '=') return S_TRAP;
            return S_TRAP;

        case S_GT:
            if (c == '=') return S_TRAP;
            return S_TRAP;

        case S_MINUS:
            if (c == '>') return S_TRAP;
            return S_TRAP;

        default:
            return S_TRAP;
    }
}

std::vector<Token> tokenize(std::string text) {
    initTokensMap();

    std::vector<Token> tokens;
    unsigned index = 0;
    unsigned len = text.length();
    
    unsigned currentLine = 1;
    unsigned currentColumn = 1;
    
    while (index < len) {
        char startChar = text[index];

        if (isSpace(startChar)) {
            if (startChar == '\n') {
                currentLine++;
                currentColumn = 1;
            }
            else
                currentColumn++;
            index++;
            continue;
        }

        DFAState state = S_START;
        unsigned startIndex = index;
        unsigned startLine = currentLine;
        unsigned startColumn = currentColumn;
        bool isComment = false;
        bool isSingleChar = false;

        std::string singleStr(1, startChar);
        if (tokensMap.find(singleStr) != tokensMap.end() && 
            startChar != '/' && startChar != '=' && startChar != '!' && 
            startChar != '<' && startChar != '>' && startChar != '-') {
            isSingleChar = true;
            index++;
            currentColumn++;
        } else {
            while (index < len) {
                char c = text[index];
                DFAState nextState = transition(state, c);

                if (nextState == S_COMMENT) {
                    while (index < len && text[index] != '\n') {
                        index++;
                    }
                    isComment = true;
                    break;
                }

                if (nextState == S_TRAP) {
                    if ((state == S_EQ && (c == '=' || c == '>')) ||
                        (state == S_NEQ && c == '=') ||
                        (state == S_LT && c == '=') ||
                        (state == S_GT && c == '=') ||
                        (state == S_MINUS && c == '>')) {
                        index++;
                        currentColumn++;
                    }
                    break; 
                }

                state = nextState;
                index++;
                currentColumn++;
            }
        }

        if (isComment) {
            continue;
        }

        if (index == startIndex) {
            throw std::runtime_error("Syntax Error: trap state at line " + 
                                     std::to_string(startLine) + ", column " + std::to_string(startColumn));
        }

        std::string lexeme = text.substr(startIndex, index - startIndex);
        Token token;
        token.line = startLine;
        token.column = startColumn;

        if (isSingleChar) {
            token.type = tokensMap[lexeme];
            token.value = lexeme;
        } else if (state == S_IDENT) {
            token.type = IDENT;
            auto it = tokensMap.find(lexeme);
            if (it != tokensMap.end()) token.type = it->second;
            token.value = lexeme;
        } 
        else if (state == S_NUMBER) {
            token.type = NUMBER;
            token.value = lexeme;
        }
        else {
            auto it = tokensMap.find(lexeme);
            if (it != tokensMap.end()) {
                token.type = it->second;
                token.value = lexeme;
            } else {
                throw std::runtime_error("Syntax Error: unknown token " + lexeme);
            }
        }

        tokens.push_back(token);
    }
    
    Token eofToken;
    eofToken.type = EOF_TOKEN;
    eofToken.value = "EOF";
    eofToken.line = currentLine;
    eofToken.column = currentColumn;
    tokens.push_back(eofToken);
    
    return tokens;
}