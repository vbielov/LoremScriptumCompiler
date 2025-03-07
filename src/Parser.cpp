#include "Parser.hpp"

Parser::Parser(Lexer& lexer) 
    : m_lexer(&lexer)
    , m_currentToken()
    , m_loopCount(0)
    , m_blockCount(-1)
    , m_isValid(true)
    , m_isTest(false) {}

Parser::Parser(Lexer& lexer, bool isTest) 
    : m_lexer(&lexer)
    , m_currentToken()
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

    auto pseudoFunctionPrototype = std::make_unique<FunctionPrototypeAST>(u8"numerus", u8"main", std::vector<std::unique_ptr<VariableDeclarationAST>>(), false, -1);
    auto pseudoFunction = std::make_unique<FunctionAST>(std::move(pseudoFunctionPrototype), std::move(pseudoBlock));

    m_topLevelDeclarations.push_back(std::move(pseudoFunction));
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
    return m_currentToken.type == type;
}

bool Parser::isToken(std::u8string value) {
    return m_currentToken.value == value;
}

bool Parser::isToken(TokenType type, std::u8string value) {
    return m_currentToken.type == type && m_currentToken.value == value;
}

bool Parser::isUnaryOperator() {
    return isToken(TokenType::OPERATOR, u8"+") || isToken(TokenType::OPERATOR, u8"-") || isToken(TokenType::OPERATOR, u8"¬");
}

void Parser::printUnknownTokenError() {
    if (m_isTest) return;
    std::cerr << RED << "Error: Unknown token: " << TOKEN_TYPE_LABELS[(int)(m_currentToken.type)]
              << " " << (const char*)(m_currentToken.value.c_str()) << RESET << std::endl;
}

void Parser::printError(std::string error) {
    if (m_isTest) return;
    std::cerr << RED << error << RESET << std::endl;
}

Token& Parser::getNextToken() {
    return m_currentToken = m_lexer->getNextToken();
}
