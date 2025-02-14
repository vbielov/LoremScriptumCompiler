#pragma once
#include "Token.hpp"

/// @brief Abstract Syntax Tree: Base class for all expression node.
/// NOTE: es ist interface
class AST {
private:
    AST* m_left;
    Token m_token;
    AST* m_right;
public:
    virtual ~AST() = default;
};

class NumberAST : public AST {
private:
    int m_value;
public:
    NumberAST(int value);
};