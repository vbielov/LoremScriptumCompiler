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
        {u8"=", 1},
        {u8"≠" , 2},
        {u8"==", 2},

        {u8">=", 10},
        {u8"<=", 10},
        {u8"<", 10},
        {u8">", 10},

        {u8"+", 20},
        {u8"-", 20},

        {u8"*", 40},
        {u8"/", 40},
        {u8"%", 40},

        {u8"^", 60},
    };

public:
    Parser(Lexer& lexer);
    std::unique_ptr<BlockAST> parseProgram();

private:
    std::unique_ptr<BlockAST> parseBlock();

    /// @brief Get the precedence of the pending binary operator token.
    int getTokenPrecedence() const;

    Token& getNextToken();

    // Anything that starts with a type is some declaration of something
    std::unique_ptr<AST> parseType();

    // It can be veriable reference or a function call
    std::unique_ptr<AST> parseIdentifier();

    // It can be for-loop, if statement or any other.
    std::unique_ptr<AST> parseKeyword();

    // It can be Identifier, constant value or some parantecies/binary operation
    std::unique_ptr<AST> parseExpression();

    std::unique_ptr<AST> parseBinOpRHS(int exprPrec, std::unique_ptr<AST> lhs);

    std::unique_ptr<FunctionPrototypeAST> parsePrototype(const std::u8string& returnType, const std::u8string& funcName);

    // It is a function
    std::unique_ptr<FunctionAST> parseFunction(const std::u8string& returnType, const std::u8string& funcName);

    std::unique_ptr<IfAST> parseIf();

    std::unique_ptr<ForAST> parseFor();
};