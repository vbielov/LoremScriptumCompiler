#include "Parser.hpp"
#include "ErrorHandler.hpp"

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
    if (isToken(TokenType::KEYWORD)) 
        return parseStatementFlow();
    else
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
    if (isToken(keywords::BREAK)) {
        if (m_loopCount == 0) {
            logError(currentLine, u8"Syntax Error: finio can only be called inside of a loop");
            return nullptr;
        }
        getNextToken(); // eat finio
        
        return std::make_unique<BreakAST>(currentLine);
    }

    if (isToken(keywords::RETURN)) {
        getNextToken();
        return std::make_unique<ReturnAST>(parseExpression(), currentLine);
    }
    if (isToken(keywords::IF)) 
        return parseStatementBranching();
    if (isToken(keywords::FOR_LOOP)) 
        return parseStatementLooping();
    
    logError(currentLine, u8"Syntax Error: Invalid flow keyword!");
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
    if (condition == nullptr){
        logError(currentLine, u8"Syntax Error: condition is missing!");
        return nullptr;
    }

    while (isToken(TokenType::NEW_LINE)){ 
        currentLine++;
        getNextToken();
    }
    if (!isToken(TokenType::PUNCTUATION, punctuation::BLOCK_OPEN)) {
        logError(currentLine, u8"Syntax Error: opening bracket ':' expected!");
        return nullptr;
    }
    auto ifBlock = parseBlock();
    if (ifBlock == nullptr) {
        logError(currentLine, u8"Syntax Error: failed to read branching statement!");
        return nullptr;
    }

    std::unique_ptr<BlockAST> elseBlock;

    while (isToken(TokenType::NEW_LINE)){
        currentLine++;
        getNextToken();
    }

    if (isToken(TokenType::KEYWORD, keywords::ELIF)) {
        auto pseudoIf = std::vector<std::unique_ptr<AST>>();
        auto elifBranch = parseStatementBranching();
        if (elifBranch == nullptr) {
            logError(currentLine, u8"Syntax Error: failed to read branching statement!");
            return nullptr;
        }
        pseudoIf.emplace_back(std::move(elifBranch));
        elseBlock = std::make_unique<BlockAST>(std::move(pseudoIf), currentLine);
    } else if (isToken(TokenType::KEYWORD, keywords::ELSE)) {
        getNextToken();

        while (isToken(TokenType::NEW_LINE)){
            currentLine++;
            getNextToken();
        }
        if (!isToken(TokenType::PUNCTUATION, punctuation::BLOCK_OPEN)) {
            logError(currentLine, u8"Syntax Error: closing bracket ';' expected!");
            return nullptr;
        } 
        elseBlock = parseBlock();
        if (elseBlock == nullptr) {
            logError(currentLine, u8"Syntax Error: failed to read branching statement!");
            return nullptr;
        }
    } else {
        auto emptyInstructions = std::vector<std::unique_ptr<AST>>();
        elseBlock = std::make_unique<BlockAST>(std::move(emptyInstructions), currentLine);
    }

    return std::make_unique<IfAST>(std::move(condition), std::move(ifBlock), std::move(elseBlock), currentLine);
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
    if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
        logError(currentLine, u8"Syntax Error: opening bracket '(' expected!");
        return nullptr;
    }

    std::unique_ptr<AST> declaration, endExpression, stepExpression;

    getNextToken();
    if (isToken(TokenType::TYPE)) {
        declaration = parseInstructionDeclaration();
        if (declaration == nullptr) {
                logError(currentLine, u8"Syntax Error: Invalid loop declaration!");
                return nullptr;
            }
        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) getNextToken();
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
        endExpression = parseExpression();
        if (endExpression == nullptr){ 
            logError(currentLine, u8"Syntax Error: closing bracket ')' is expected!");
            return nullptr;
        }

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken();
            if (isToken(TokenType::TYPE)) {
                logError(currentLine, u8"Syntax Error: type declaration must be at first position of loop!");
                return nullptr;
            }
            stepExpression = parseInstruction();
            if (stepExpression == nullptr) {
                logError(currentLine, u8"Syntax Error: invalid step expression!");
                return nullptr;
            }
        }

        if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
            logError(currentLine, u8"Syntax Error: closing bracket not expected!");
            return nullptr;
        }
    }

    getNextToken();
    while (isToken(TokenType::NEW_LINE)) {
        currentLine++;
        getNextToken();
    }
    if (!isToken(TokenType::PUNCTUATION, punctuation::BLOCK_OPEN)){ 
        logError(currentLine, u8"Syntax Error: opening bracket ':' expected!");
        return nullptr;
    }

    m_loopCount++;
    auto loopBlock = parseBlock();
    m_loopCount--;
    if (loopBlock == nullptr) {
        logError(currentLine, u8"Syntax Error: invalid loop block!");
        return nullptr;
    }

    // Building pseudo wrapper block
    auto loopWrapper = std::vector<std::unique_ptr<AST>>();
    if (endExpression != nullptr) {
        auto elseBlockInstr = std::vector<std::unique_ptr<AST>>();
        std::unique_ptr<AST> breakAst = std::make_unique<BreakAST>(currentLine);
        elseBlockInstr.emplace_back(std::move(breakAst));
        auto elseBlock = std::make_unique<BlockAST>(std::move(elseBlockInstr), currentLine);
        auto endAST = std::make_unique<IfAST>(std::move(endExpression), std::move(loopBlock), std::move(elseBlock), currentLine);
        loopWrapper.emplace_back(std::move(endAST));
    }

    if (stepExpression != nullptr) {
        loopWrapper.emplace_back(std::move(stepExpression));
    }

    if (!loopWrapper.empty()) {
        loopBlock = std::make_unique<BlockAST>(std::move(loopWrapper), currentLine);
    }
    auto loopAST = std::make_unique<LoopAST>(std::move(loopBlock), currentLine);

    if (declaration != nullptr) {
        auto outerLoopWrapper = std::vector<std::unique_ptr<AST>>();
        outerLoopWrapper.emplace_back(std::move(declaration));
        outerLoopWrapper.emplace_back(std::move(loopAST));
        return std::make_unique<BlockAST>(std::move(outerLoopWrapper), currentLine);
    } else {
        return loopAST;
    }
}
