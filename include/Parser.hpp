#pragma once
#include "Lexer.hpp"
#include "AST.hpp"

class Parser {
private:
    Lexer* m_lexer;
    Token currentToken;

public:
    Parser(Lexer& lexer);

private:
    /// @brief Get the precedence of the pending binary operator token.
    int getTokenPrecedence();
    AST sieveToken(const Token& token) const;

    // TODO: Based on token do:
    // ParseExpression();
    // ParseIdentifierExpr();
    // ParseParenExpr();
};