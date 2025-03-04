#include "Parser.hpp"

/**
 * A statement can be an instuction or a flow statement like for, if, break, return
 *
 * currentToken is at first token of statement
 *
 * Instructions - always start with IDENTIFIER or TYPE (is checked inside parseInstruction())
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
    if (isToken(TokenType::KEYWORD)) return parseStatementFlow();
    return parseInstruction();
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

        getNextToken();
        
        return std::make_unique<BreakAST>();
    }

    if (isToken(u8"retro")) {
        getNextToken();
        return std::make_unique<ReturnAST>(parseExpression());
    }
    if (isToken(u8"si")) return parseStatementBranching();
    if (isToken(u8"∑")) return parseStatementLooping();
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

    while (isToken(TokenType::NEW_LINE)) getNextToken();
    if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
    auto ifBlock = parseBlock();
    if (ifBlock == nullptr) return nullptr;

    std::unique_ptr<BlockAST> elseBlock;

    while (isToken(TokenType::NEW_LINE)) getNextToken();

    if (isToken(TokenType::KEYWORD, u8"nisi")) {
        auto pseudoIf = std::vector<std::unique_ptr<AST>>();
        auto elifBranch = parseStatementBranching();
        if (elifBranch == nullptr) return nullptr;
        pseudoIf.emplace_back(std::move(elifBranch));
        elseBlock = std::make_unique<BlockAST>(std::move(pseudoIf));
    } else if (isToken(TokenType::KEYWORD, u8"ni")) {
        getNextToken();

        while (isToken(TokenType::NEW_LINE)) getNextToken();
        if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
        elseBlock = parseBlock();
        if (elseBlock == nullptr) return nullptr;
    } else {
        auto emptyInstructions = std::vector<std::unique_ptr<AST>>();
        elseBlock = std::make_unique<BlockAST>(std::move(emptyInstructions));
    }

    return std::make_unique<IfAST>(std::move(condition), std::move(ifBlock), std::move(elseBlock));
}

/**
 *  There are four possible Looping structures: ∑(declaration, expression, assignment): ... ;   ∑(bool expression, expression): ... ;   ∑(bool expression): ... ;   ∑(): ... ;
 *
 *  currentToken is always at '∑' token
 *
 *  All variants:
 *  1. ∑(numerus i = I, i > X, i = i + I): [Block] ;
 *  2. ∑(numerus i = I, i > X)           : [Block] ;
 *  3. ∑(numerus i = I)                  : [Block] ;
 *  4. ∑(i > X, i = i + I)               : [Block] ;
 *  5. ∑(i > X)                          : [Block] ;
 *  6. ∑()                               : [Block] ;
 *
 *  Side notes:
 *      - If we enter loop block we increase m_loopCount. If the exit loop block we decrease. Necessary, because 'finio' can only be called inside loop
 *      - Header will generate pseudo wrapper that is executed before block
 */
std::unique_ptr<AST> Parser::parseStatementLooping() {
    getNextToken();
    if (!isToken(TokenType::PUNCTUATION, u8"(")) return nullptr;

    std::unique_ptr<AST> declaration, endExpression, stepExpression;

    getNextToken();
    if (isToken(TokenType::TYPE)) {
        declaration = parseInstructionDeclaration();
        if (declaration == nullptr) return nullptr;
        if (isToken(TokenType::PUNCTUATION, u8",")) getNextToken();
    }

    if (!isToken(TokenType::PUNCTUATION, u8")")) {
        endExpression = parseExpression();
        if (endExpression == nullptr) return nullptr;

        if (isToken(TokenType::PUNCTUATION, u8",")) {
            getNextToken();
            if (isToken(TokenType::TYPE)) return nullptr;
            stepExpression = parseInstruction();
            if (stepExpression == nullptr) return nullptr;
        }

        if (!isToken(TokenType::PUNCTUATION, u8")")) return nullptr;
    }

    getNextToken();
    while (isToken(TokenType::NEW_LINE)) getNextToken();
    if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
    m_loopCount++;
    auto loopBlock = parseBlock();
    m_loopCount--;
    if (loopBlock == nullptr) return nullptr;

    // build pseudo wrapper block

    auto loopWrapper = std::vector<std::unique_ptr<AST>>();
    if (endExpression != nullptr) {
        auto elseBlockInstr = std::vector<std::unique_ptr<AST>>();
        std::unique_ptr<AST> breakAst = std::make_unique<BreakAST>();
        elseBlockInstr.emplace_back(std::move(breakAst));
        auto elseBlock = std::make_unique<BlockAST>(std::move(elseBlockInstr));
        auto endAST = std::make_unique<IfAST>(std::move(endExpression), std::move(loopBlock), std::move(elseBlock));
        loopWrapper.emplace_back(std::move(endAST));
    }

    if (stepExpression != nullptr) {
        loopWrapper.emplace_back(std::move(stepExpression));
    }

    if (!loopWrapper.empty()) {
        loopBlock = std::make_unique<BlockAST>(std::move(loopWrapper));
    }
    auto loopAST = std::make_unique<LoopAST>(std::move(loopBlock));

    if (declaration != nullptr) {
        auto outerLoopWrapper = std::vector<std::unique_ptr<AST>>();
        outerLoopWrapper.emplace_back(std::move(declaration));
        outerLoopWrapper.emplace_back(std::move(loopAST));
        return std::make_unique<BlockAST>(std::move(outerLoopWrapper));
    } else {
        return loopAST;
    }
}
