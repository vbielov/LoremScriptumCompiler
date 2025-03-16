#include "Parser.hpp"

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

    getNextToken(); // eat ':', or if it's a first block in the tree, read first token
    while (!isFinishedBlock()) {
        if (isToken(TokenType::NEW_LINE)) {
            getNextToken();
            continue;
        }

        auto statement = parseStatement();
        if (statement != nullptr) {
            statements.emplace_back(std::move(statement));
        } else {
            m_isValid = false;
            printUnknownTokenError();
            // Error on this line detected. Go to next line and try to do business as usual
            while (!isFinishedBlock() && !isToken(TokenType::NEW_LINE)) {
                getNextToken();
            }
        }
    }

    // Catch close/open more blocks than possible
    if (isToken(TokenType::PUNCTUATION, punctuation::BLOCK_CLOSE)) 
        m_blockCount--;

    if (isToken(TokenType::PUNCTUATION, punctuation::BLOCK_CLOSE)) 
        getNextToken();

    return std::make_unique<BlockAST>(std::move(statements));
}

bool Parser::isFinishedBlock() {
    return isToken(TokenType::EOF_TOKEN) || isToken(TokenType::PUNCTUATION, punctuation::BLOCK_CLOSE);
}
