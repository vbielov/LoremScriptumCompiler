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

    getNextToken();
    while (!isFinishedBlock()) {
        if (!isToken(TokenType::NEW_LINE)) {
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
                continue;
            }
        }

        getNextToken();
    }

    return std::make_unique<BlockAST>(std::move(statements));
}

bool Parser::isFinishedBlock() {
    return isToken(TokenType::EOF_TOKEN) || isToken(TokenType::PUNCTUATION, u8";");
}
