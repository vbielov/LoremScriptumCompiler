#include "Parser.hpp"

bool Parser::isInsideLoop() {
    return m_loopCount > 0;
}

bool Parser::isExpressionEnd() {
    return isToken(TokenType::NEW_LINE) || isToken(TokenType::EOF_TOKEN) || isToken(TokenType::PUNCTUATION);
}

bool Parser::isToken(TokenType type) {
    return m_currentToken.type == type;
}

bool Parser::isToken(std::u8string value) {
    return m_currentToken.value == value;
}

bool Parser::isToken(TokenType type, std::u8string value) {
    return m_currentToken.type == type && m_currentToken.value == value;
}

void Parser::printUnknownTokenError() {
    std::cerr << RED << "Error: Unknown token: " << TOKEN_TYPE_LABELS[(int)(m_currentToken.type)]
              << " " << (const char*)(m_currentToken.value.c_str()) << RESET << std::endl;
}

void Parser::printError(std::string error) {
    std::cerr << RED << error << RESET << std::endl;
}

Token& Parser::getNextToken() {
    return m_currentToken = m_lexer->getNextToken();
}

Parser::Parser(Lexer& lexer) : m_lexer(&lexer) {}
