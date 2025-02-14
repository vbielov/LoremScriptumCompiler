#pragma once
#include "Lexer.hpp"

class Parser {
private:
    Lexer* m_lexer;
    Token currentToken;

    /// @brief Get the precedence of the pending binary operator token.
    int getTokenPrecedence();

    // TODO: Based on token do:
    // ParseExpression();
    // ParseIdentifierExpr();
    // ParseParenExpr();

public:
    Parser(Lexer& lexer);
};