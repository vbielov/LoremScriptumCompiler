#include "Parser.hpp"

Parser::Parser(Lexer &lexer)
    : m_lexer(&lexer) {}

int Parser::getTokenPrecedence() {    
    return 0;
}
