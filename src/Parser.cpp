#include "Parser.hpp"

Parser::Parser(Lexer &lexer)
    : m_lexer(&lexer) {}

int Parser::getTokenPrecedence() {    
    return 0;
}

AST Parser::sieveToken(const Token &token) const
{
    return AST();
}
