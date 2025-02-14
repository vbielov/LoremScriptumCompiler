#include "Lexer.hpp"

Lexer::Lexer(const char* sourceCode) 
    : m_souceCode(sourceCode) {}

Token Lexer::getNextToken() {
    return Token{TokenType::EOF_TOKEN, ""};
};
