#pragma once
#include <format>
#include "AST.hpp"
#include "Token.hpp"
#include "RomanNumber.hpp"

class Parser {
private:
    const std::vector<Token>& m_tokens;
    std::vector<Token>::const_iterator m_currentToken;
    std::ostream &m_ostr;
    
    int m_loopCount;
    int m_blockCount;
    bool m_isValid;
    bool m_isTest;
    std::vector<std::unique_ptr<AST>> m_topLevelDeclarations;
    std::unordered_map<std::u8string, StructDataType*> m_structHashMap;

public:
    Parser(const std::vector<Token>& tokens);
    Parser(const std::vector<Token>& tokens, bool isTest, std::ostream &m_ostr);

    bool isValid();
    std::unique_ptr<BlockAST> parse();

    //for ErrorHandler
    size_t currentLine = 1;
    std::vector<size_t> lastOpenBlock;


private:
    const Token& getNextToken();

    bool isInsideLoop();
    bool isFinishedBlock();
    bool isExpressionEnd();
    bool isToken(TokenType type);
    bool isToken(TokenType type, const std::u8string_view& value);
    bool isToken(const std::u8string_view& value);
    bool isUnaryOperator();
    std::unique_ptr<IDataType> parseType();


    // --- Block section ---
    std::unique_ptr<BlockAST> parseBlock();

    // --- Statement section ---
    std::unique_ptr<AST> parseStatement();
    std::unique_ptr<AST> parseStatementFlow();
    std::unique_ptr<IfAST> parseStatementBranching();
    std::unique_ptr<AST> parseStatementLooping();
    
    // --- Instruction section ---
    std::unique_ptr<AST> parseInstruction();
    std::unique_ptr<AST> parseInstructionDeclaration();
    std::unique_ptr<AST> parseInstructionDeclarationStruct();
    std::unique_ptr<ArrayAST> parseArray();
    std::unique_ptr<AST> parseInstructionAssignment(const std::u8string& identifier);
    std::unique_ptr<AST> parseInstructionArrayAssignment(const std::u8string& identifier);
    std::unique_ptr<AST> parseInstructionShorthand(const std::u8string& identifier);

    std::unique_ptr<FunctionPrototypeAST> parseInstructionPrototype(const std::u8string& identifier, std::unique_ptr<IDataType> type);
    std::unique_ptr<AST> parseInstructionFunction(const std::u8string& identifier, std::unique_ptr<IDataType> type);

    // --- Expression section ---
    std::unique_ptr<AST> parseExpression();
    std::unique_ptr<AST> parseExpressionSingle();

    std::unique_ptr<FuncCallAST> parseExpressionFunctionCall(const std::u8string& identifier);
};