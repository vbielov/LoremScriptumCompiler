#include "Parser.hpp"
#include "ErrorHandler.hpp"

/**
 * A block contains statements
 *
 * ↓ we are here
 * :
 *      numerus i = I;              <-- Instruction
 *      si numerus == I:            <-- Flow
 *          numerus = numerus + I   <-- Instruction in new Block
 *      ;
 * ;
 */
std::unique_ptr<BlockAST> Parser::parseBlock() {
    std::vector<std::unique_ptr<AST>> statements;

    m_blockCount++;
    lastOpenBlock.push_back(currentLine);

    getNextToken(); // eat ':', or if it's a first block in the tree, read first token
    while (!isFinishedBlock()) {
        if (isToken(TokenType::NEW_LINE)) {
            currentLine++;
            getNextToken();
            continue;
        }

        auto statement = parseStatement();
        if (statement != nullptr) {
            statements.emplace_back(std::move(statement));
        } else {
            m_isValid = false;
            logError(currentLine, u8"Syntax Error: invalid block statement!");
            // Error on this line detected. Go to next line and try to do business as usual
            while (!isFinishedBlock() && !isToken(TokenType::NEW_LINE)) {
                getNextToken();
            }
        }
    }


    // Catch close/open more blocks than possible
    if (isToken(TokenType::PUNCTUATION, punctuation::BLOCK_CLOSE)) {
        m_blockCount--;
        
        lastOpenBlock.erase(lastOpenBlock.end());
        if (m_blockCount < 0) {
            // Closing block that was never opened
            m_isValid = false;
            logError(currentLine, u8"Syntax Error: invalid closing block that was never opened!");
        }
    }

    //ErrorHandler
    if (m_blockCount < 0 || (isToken(TokenType::EOF_TOKEN) && m_blockCount != 0)) {
        if(m_blockCount != 0 && lastOpenBlock.size() > 1){

            logError(lastOpenBlock.back(), u8"ControlFlow Error: opened bracket must be closed!");
            lastOpenBlock.erase(lastOpenBlock.end());
        }
        m_isValid = false;
    }

    if (isToken(TokenType::PUNCTUATION, punctuation::BLOCK_CLOSE)) 
        getNextToken();

    return std::make_unique<BlockAST>(std::move(statements), currentLine);
}

bool Parser::isFinishedBlock() {
    return isToken(TokenType::EOF_TOKEN) || isToken(TokenType::PUNCTUATION, punctuation::BLOCK_CLOSE);
}
