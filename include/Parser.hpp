#pragma once
#include "Lexer.hpp"
#include "AST.hpp"
#include "RomanNumber.hpp"
#include <unordered_map>

// TODO(Vlad): Just followed tutorial here, need to add types, single operators, controll flow and so much more.
// https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html

class Parser {
private:
    Lexer* m_lexer;
    Token m_currentToken;

    // https://en.wikipedia.org/wiki/Order_of_operations
    inline static const std::unordered_map<std::u8string, int> BINARY_OPERATION_PRECEDENCE = {
        {u8"==", 1},
        {u8"≠" , 1},

        {u8"<", 10},
        {u8">", 10},
        {u8">=", 10},
        {u8"<=", 10},

        {u8"+", 20},
        {u8"-", 20},

        {u8"*", 40},
        {u8"/", 40},
        {u8"%", 40},

        {u8"^", 60},
    };

public:
    Parser(Lexer& lexer);

    /// @brief Main loop
    void parse();

private:
    /// @brief Get the precedence of the pending binary operator token.
    int getTokenPrecedence() const;

    Token& getNextToken();

    // any expression
    std::unique_ptr<AST> parseExpression(); 
    
    std::unique_ptr<AST> parseNumberExpr();
    
    std::unique_ptr<AST> parseParenExpr(); // ( args )

    // identifier() => function
    // identifier => variable
    std::unique_ptr<AST> parseIdentifierExpr(); 

    // identifier-, number-, paren- expression
    std::unique_ptr<AST> parsePrimary();

    std::unique_ptr<AST> parseBinOpRHS(int exprPrec, std::unique_ptr<AST> LHS);

    std::unique_ptr<AST> parseAssigment();

    // Anonymous functions that are just expresions
    std::unique_ptr<FunctionAST> parseTopLevelExpr();
};