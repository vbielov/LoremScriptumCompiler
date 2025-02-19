#include "Parser.hpp"

Parser::Parser(Lexer &lexer)
    : m_lexer(&lexer) {}

int Parser::getTokenPrecedence() {    
    return 0;
}

std::unique_ptr<AST> Parser::sieveToken([[maybe_unused]] const Token& token) const {
    return nullptr;
}
