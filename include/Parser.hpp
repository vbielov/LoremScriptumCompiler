#pragma once
#include <unordered_map>
#include "AST.hpp"
#include "Lexer.hpp"
#include "RomanNumber.hpp"

// TODO(Vlad): Just followed tutorial here, need to add types, single operators, controll flow and so much more.
// https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl02.html

class Parser {
   private:
    Lexer* m_lexer;
    Token m_currentToken;

    int m_loopCount;
    int m_blockCount;
    bool m_isValid;
    bool m_isTest;

    // source: https://en.wikipedia.org/wiki/Order_of_operations
    // smaller number means higher priority
    inline static const std::unordered_map<std::u8string, int> BINARY_OPERATION_PRIORITY = {
        {u8"!", 2},
        {u8"¬", 2},
        {u8"-", 2},
        {u8"+", 2},
        {u8"^", 2},

        {u8"*", 3},
        {u8"/", 3},
        {u8"%", 3},

        {u8"+", 4},
        {u8"-", 4},

        {u8"<", 6},
        {u8"≥", 6},
        {u8"≤", 6},
        {u8">", 6},

        {u8"⇔", 7},
        {u8"≠", 7},

        {u8"∧", 11},

        {u8"∨", 12},

        {u8"=", 14},
    };

   public:
    Parser(Lexer& lexer);
    Parser(Lexer& lexer, bool isTest);

    bool isValid();
    std::unique_ptr<BlockAST> parseBlock();

   private:
    /// @brief Get the precedence of the pending binary operator token.
    int getTokenPrecedence() const;

    Token& getNextToken();

    bool isInsideLoop();
    bool isFinishedBlock();
    bool isExpressionEnd();
    bool isToken(TokenType type);
    bool isToken(TokenType type, std::u8string value);
    bool isToken(std::u8string value);

    void printError(std::string error);
    void printUnknownTokenError();

    // --- Statement section ---
    std::unique_ptr<AST> parseStatement();
    std::unique_ptr<AST> parseStatementFlow();
    std::unique_ptr<IfAST> parseStatementBranching();
    std::unique_ptr<AST> parseStatementLooping();

    // --- Instruction section ---
    std::unique_ptr<AST> parseInstruction();
    std::unique_ptr<AST> parseInstructionAssignment(const std::u8string& identifier);
    std::unique_ptr<AST> parseInstructionArrayAssignment(const std::u8string& identifier);
    std::unique_ptr<AST> parseInstructionDeclaration();

    std::unique_ptr<FunctionPrototypeAST> parseInstructionPrototype(const std::u8string& type, const std::u8string& identifier);
    std::unique_ptr<FunctionAST> parseInstructionFunction(const std::u8string& type, const std::u8string& identifier);

    // --- Expression section ---
    std::unique_ptr<AST> parseExpression();
    std::unique_ptr<AST> parseExpressionSingle();

    std::unique_ptr<FunctionPrototypeAST> parsePrototype(const std::u8string& returnType, const std::u8string& funcName);
    std::unique_ptr<FuncCallAST> parseExpressionFunctionCall(const std::u8string& identifier);
};