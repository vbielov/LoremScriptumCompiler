#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) 
    : m_tokens(tokens)
    , m_currentToken(nullptr)
    , m_loopCount(0)
    , m_blockCount(-1)
    , m_isValid(true)
    , m_isTest(false) {}

Parser::Parser(const std::vector<Token>& tokens, bool isTest) 
    : m_tokens(tokens)
    , m_currentToken(nullptr)
    , m_loopCount(0)
    , m_blockCount(-1)
    , m_isValid(true)
    , m_isTest(isTest) {}


std::unique_ptr<BlockAST> Parser::parse() {
    if (m_isTest) {
        // Do not make a main function wrapper for parse tests -> Easier to debug
        return parseBlock();
    }

    auto block = parseBlock();

    std::unique_ptr<AST> pseudoReturnValue = std::make_unique<NumberAST>(0);
    auto pseudoReturn = std::make_unique<ReturnAST>(std::move(pseudoReturnValue));

    auto pseudoBlockInstr = std::vector<std::unique_ptr<AST>>();
    pseudoBlockInstr.push_back(std::move(block));
    pseudoBlockInstr.push_back(std::move(pseudoReturn));
    auto pseudoBlock = std::make_unique<BlockAST>(std::move(pseudoBlockInstr));

    std::unique_ptr<IDataType> mainReturnType = std::make_unique<PrimitiveDataType>(PrimitiveType::INT);
    auto pseudoFunctionPrototype = std::make_unique<FunctionPrototypeAST>(
        u8"main", 
        std::move(mainReturnType),
        std::vector<std::unique_ptr<TypeIdentifierPair>>(),
        true
    );
    auto pseudoFunction = std::make_unique<FunctionAST>(std::move(pseudoFunctionPrototype), std::move(pseudoBlock));

    m_topLevelDeclarations.push_back(std::move(pseudoFunction));

    // NOTE(Vlad):  maybe it's not needed anymore, 
    //              because I have figured out how to set insert block to nullptr

    // std::sort(m_topLevelDeclarations.begin(), m_topLevelDeclarations.end(), [](const std::unique_ptr<AST>& a, const std::unique_ptr<AST>& b) {
    //     int aPriority = 0;
    //     int bPriority = 0;
    //     if (dynamic_cast<VariableDeclarationAST*>(a.get()))
    //         aPriority += 2;
    //     else if (dynamic_cast<FunctionAST*>(a.get()) || dynamic_cast<FunctionPrototypeAST*>(a.get()))
    //         aPriority += 1;
    //     if (dynamic_cast<VariableDeclarationAST*>(b.get()))
    //         bPriority += 2;
    //     else if (dynamic_cast<FunctionAST*>(b.get()) || dynamic_cast<FunctionPrototypeAST*>(b.get()))
    //         bPriority += 1;
    //     return aPriority > bPriority;
    // });

    return std::make_unique<BlockAST>(std::move(m_topLevelDeclarations));
}


bool Parser::isValid() {
    return m_isValid;
}

bool Parser::isInsideLoop() {
    return m_loopCount > 0;
}

bool Parser::isExpressionEnd() {
    return isToken(TokenType::NEW_LINE) || isToken(TokenType::EOF_TOKEN) || isToken(TokenType::PUNCTUATION);
}

bool Parser::isToken(TokenType type) {
    return m_currentToken->type == type;
}

bool Parser::isToken(const std::u8string_view& value) {
    return m_currentToken->value == value;
}

bool Parser::isToken(TokenType type, const std::u8string_view& value) {
    return m_currentToken->type == type && m_currentToken->value == value;
}

bool Parser::isUnaryOperator() {
    return isToken(TokenType::OPERATOR, operators::PLUS) || isToken(TokenType::OPERATOR, operators::MINUS) || isToken(TokenType::OPERATOR, operators::NOT);
}

void Parser::printUnknownTokenError() {
    if (m_isTest) return;
    std::cerr << RED << "Error: Unknown token: " << TOKEN_TYPE_LABELS[(int)(m_currentToken->type)]
              << " " << (const char*)(m_currentToken->value.c_str()) << RESET << std::endl;
}

void Parser::printError(std::string error) {
    if (m_isTest) return;
    std::cerr << RED << error << RESET << std::endl;
}

const Token& Parser::getNextToken() {
    // Get first token
    if (m_currentToken.base() == nullptr) {
        m_currentToken = m_tokens.begin();
        return *m_currentToken;
    }
    m_currentToken++;
    return *m_currentToken; 
}
