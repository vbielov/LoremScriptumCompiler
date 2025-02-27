#include "Parser.hpp"

/**
 * A statement can be an instuction or a flow statement like for, if, break, return
 *
 * currentToken is at first token of statement
 *
 * Instructions - always start with IDENTIFIER or TYPE
 *  - numerus id = I
 *  - id = id + V
 *  - func()
 *
 * Flow Statement - always start with KEYWORD
 *  - retro
 *  - finio
 *  - si numerus == I: numerus = numerus + I ; nisi ... ni ...
 *  - ∑(∞): ... ;

 */
std::unique_ptr<AST> Parser::parseStatement() {
    switch (m_currentToken.type) {
        case TokenType::IDENTIFIER:
        case TokenType::TYPE:
            return parseInstruction();
        case TokenType::KEYWORD:
            return parseStatementFlow();
        default:
            return nullptr;
    }
}

/**
 *  A Flow Statement changes the instruction execution order
 *
 *  currentToken is at first token of flow statement. Type is always KEYWORD
 *
 *  - retro
 *  - finio
 *  - si numerus == I: numerus = numerus + I ; nisi ... ni ...
 *  - ∑(∞): ... ;
 */
std::unique_ptr<AST> Parser::parseStatementFlow() {
    if (isToken(u8"finio")) {
        if (m_loopCount == 0) {
            printError("finio can only be called inside loop");
            return nullptr;
        }

        // TODO: Add finio AST
        return nullptr;
    }

    if (isToken(u8"retro")) {
        getNextToken();
        return std::make_unique<ReturnAST>(parseExpression());
    }
    if (isToken(u8"si")) return parseStatementBranching();
    if (isToken(u8"∑(∞)")) return parseStatementLooping();
    return nullptr;
}

/**
 *  Branching always starts with if, contains many ifelse and can end with single else
 *
 *  currentToken is always at 'si' token
 *
 *  - si numerus == I: ... ;
 *  - si numerus == I: ... ; nisi numerus == II: ... ;
 *  - si numerus == I: ... ; nisi numerus == II: ... ; nisi numerus == III: ... ;
 *  - si numerus == I: ... ; nisi numerus == II: ... ; nisi numerus == III: ... ; ni: ... ;
 *  - si numerus == I: ... ; ni: ... ;
 *
 *  Remove elif trick to simplify AST:
 *      if [BLOCK1] elif [BLOCK2] else [BLOCK3]   -->   if [BLOCK1] else [ if [BLOCK2] else [BLOCK3] ]
 */
std::unique_ptr<IfAST> Parser::parseStatementBranching() {
    getNextToken();

    auto condition = parseExpression();
    if (condition == nullptr) return nullptr;

    if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
    auto ifBlock = parseBlock();
    if (!isToken(TokenType::PUNCTUATION, u8";") || ifBlock == nullptr) return nullptr;

    std::unique_ptr<BlockAST> elseBlock;

    do getNextToken();
    while (isToken(TokenType::NEW_LINE));

    if (isToken(TokenType::KEYWORD, u8"nisi")) {
        auto pseudoIf = std::vector<std::unique_ptr<AST>>();
        pseudoIf.emplace_back(parseStatementBranching());
        elseBlock = std::make_unique<BlockAST>(std::move(pseudoIf));  // TODO: Is this correct?
    } else if (isToken(TokenType::KEYWORD, u8"ni")) {
        getNextToken();

        if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
        elseBlock = parseBlock();
        if (!isToken(TokenType::PUNCTUATION, u8";") || elseBlock == nullptr) return nullptr;
    } else if (!isToken(TokenType::NEW_LINE)) {
        return nullptr;  // TODO: set default elseBlock
    }

    return std::make_unique<IfAST>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
}

/**
 *  Looping structure ∑(∞): ... ;
 *
 *  currentToken is always at '∑(∞)' token
 *
 *  - ∑(∞): [Block] ;
 *
 *  Side note: If we enter loop block we increase m_loopCount. If the exit loop block we decrease. Necessary, because 'finio' can only be called inside loop
 */
std::unique_ptr<ForAST> Parser::parseStatementLooping() {
    getNextToken();

    if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
    m_loopCount++;
    auto loopBlock = parseBlock();
    m_loopCount--;
    if (!isToken(TokenType::PUNCTUATION, u8";")) return nullptr;

    // TODO: add these
    std::u8string varName = nullptr;
    std::unique_ptr<AST> start = nullptr;
    std::unique_ptr<AST> end = nullptr;
    std::unique_ptr<AST> step = nullptr;

    return std::make_unique<ForAST>(varName, std::move(start), std::move(end), std::move(step), std::move(loopBlock));
}
