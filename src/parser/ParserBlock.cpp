#include "parser/Parser.hpp"

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

    while (!isFinishedBlock()) {
        getNextToken();

        if (isToken(TokenType::NEW_LINE)) continue;

        auto statement = parseStatement();
        if (statement != nullptr) {
            statements.emplace_back(std::move(statement));
        } else {
            printUnknownTokenError();
        }
    }

    return std::make_unique<BlockAST>(std::move(statements));
}

bool Parser::isFinishedBlock() {
    return m_currentToken.type == TokenType::EOF_TOKEN || (m_currentToken.type == TokenType::PUNCTUATION && m_currentToken.value == u8";");
}
