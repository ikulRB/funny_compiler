#include <iostream>
#include <cctype>
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

void countingTokenSize(const std::string &text, unsigned len, unsigned index, Token &token, unsigned &tokenSize) {
    while (index + tokenSize < len) {
        char ch = text[index + tokenSize];
        if (token.type == NUMBER && !std::isdigit(ch)) break;
        if (token.type == IDENT && !std::isalnum(ch) && ch != '_') break;
        tokenSize++;
    } 
}

Token parseNumber(const std::string &text, unsigned len, unsigned index, unsigned &tokenSize) {
    Token token;
    token.type = NUMBER;
    countingTokenSize(text, len, index, token, tokenSize);
    token.value = text.substr(index, tokenSize);
    return token;
}

Token parseIdentifier(const std::string &text, unsigned len, unsigned index, unsigned &tokenSize) {
    Token token;
    token.type = IDENT;
    countingTokenSize(text, len, index, token, tokenSize);
    token.value = text.substr(index, tokenSize);
    return token;
}

Token parseTokenType(const std::string &text, unsigned len, unsigned index, unsigned &tokenSize) {
    Token token;
    tokenSize = 0;
    char ch = text[index];

    if (index + 1 < len) {
        std::string op2 = text.substr(index, 2);
        auto foundOp2 = tokensMap.find(op2);
        if (foundOp2 != tokensMap.end()) {
            tokenSize = 2;
            token.type = foundOp2->second;
            token.value = op2;
            return token;
        }
    }

    if (!std::isalnum(ch) && ch != '_') {
        std::string op1 = text.substr(index, 1);
        auto foudOp1 = tokensMap.find(op1);
        if (foudOp1 != tokensMap.end()) {
            tokenSize = 1;
            token.type = foudOp1->second;
            token.value = op1;
            return token;
        }
    }

    if (std::isdigit(ch)) return parseNumber(text, len, index, tokenSize);

    if (std::isalpha(ch) || ch == '_') {
        Token identToken = parseIdentifier(text, len, index, tokenSize);
        
        auto it = tokensMap.find(identToken.value);
        if (it != tokensMap.end()) identToken.type = it->second;
        return identToken;
    }

    throw std::runtime_error("Syntax Error");
}

std::vector<Token> tokenize(std::string text) {
    initTokensMap();

    std::vector<Token> tokens;
    unsigned index = 0;
    unsigned len = text.length();
    
    unsigned currentLine = 1;
    unsigned currentColumn = 1;
    
    while (index < len) {
        char ch = text[index];
        
        if (ch == ' ' || ch == '\t') {
            index++;
            currentColumn++;
            continue;
        }
        if (ch == '\n') {
            index++;
            currentLine++;
            currentColumn = 1;
            continue;
        }
        if (ch == '/' && index + 1 < len && text[index + 1] == '/') {
            index += 2;
            while (index < len && text[index] != '\n') {
                index++;
            }
            continue;
        }
        
        try {
            unsigned tokenSize = 0;
            Token token = parseTokenType(text, len, index, tokenSize);
            
            token.line = currentLine;
            token.column = currentColumn;
            
            tokens.push_back(token);
            
            index += tokenSize;
            currentColumn += tokenSize;
        }
        catch (const std::runtime_error &e) {       
            throw std::runtime_error(std::string(e.what()) + ": line "+ std::to_string(currentLine)
                                     + ", column " + std::to_string(currentColumn));

        }
    }
    
    Token eofToken;
    eofToken.type = EOF_TOKEN;
    eofToken.value = "EOF";
    eofToken.line = currentLine;
    eofToken.column = currentColumn;
    tokens.push_back(eofToken);
    
    return tokens;
}
