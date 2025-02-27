#include "parser/Parser.hpp"

/**
 * An Expression always calculates a value of some type. Expression end on new line or unopend ')'
 *
 * currentToken is at first token of expression.
 *
 * Examples:
 *    - I
 *    - I + I
 *    - I - (I + I)
 *    - var + I
 *    - I + func()
 *    - I + II * IV
 *      ^ we are always here
 *
 *  Trick of priority ranking:
 *               I + II * III
 *  priority:      a    b
 *
 *  a <= b: (I +  II) * III
 *  a >  b:  I + (II  * III)
 */
std::unique_ptr<AST> Parser::parseExpression() {
    auto left = parseExpressionSingle();
    if (left == nullptr) return nullptr;

    getNextToken();
    if (isExpressionEnd()) return left;

    if (!isToken(TokenType::OPERATOR)) return nullptr;
    auto op = *BINARY_OPERATION_PRIORITY.find(m_currentToken.value);

    getNextToken();
    auto right = parseExpressionSingle();
    if (right) return nullptr;

    getNextToken();
    if (isExpressionEnd()) {
        return std::make_unique<BinaryOperatorAST>(op, left, right);
    }

    if (!isToken(TokenType::OPERATOR)) return nullptr;
    auto nextOp = *BINARY_OPERATION_PRIORITY.find(m_currentToken.value);

    getNextToken();
    auto nextExpression = parseExpression();
    if (nextExpression == nullptr) return nullptr;

    if (op.second <= nextOp.second) {
        auto priorityOp = std::make_unique<BinaryOperatorAST>(op, left, right);
        return std::make_unique<BinaryOperatorAST>(nextOp, priorityOp, nextExpression);
    } else {
        auto priorityOp = std::make_unique<BinaryOperatorAST>(op, right, nextExpression);
        return std::make_unique<BinaryOperatorAST>(nextOp, left, priorityOp);
    }
}

/**
 * A single Expression is a value without using operators that need two values (e.g. addition)
 *
 * currentToken is at first token of single expression.
 *
 * Examples:
 *    - I
 *    - func()
 *    - -I
 *    - +var
 *    - ---var
 *    - (I + II *IV)
 *
 * Trick of sign number -I -> 0 - I
 *
 */
std::unique_ptr<AST> Parser::parseExpressionSingle() {
    std::unique_ptr<AST> value;

    if (isToken(TokenType::OPERATOR, u8"+") || isToken(TokenType::OPERATOR, u8"-")) {
        auto sign = m_currentToken.value;

        getNextToken();
        value = parseExpressionSingle();

        // TODO: Create a UnaryOperatorAST
        auto lhs = std::make_unique<NumberAST>(0);
        return std::make_unique<BinaryOperatorAST>(sign, lhs, value);
    }

    if (isToken(TokenType::NUMBER)) {
        value = std::make_unique<NumberAST>(m_currentToken.value);
        getNextToken();
    } else if (isToken(TokenType::LITERAL)) {
        value = std::make_unique<CharAST>(m_currentToken.value);  // TODO: is this correct?
        getNextToken();
    } else if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken.value;

        getNextToken();
        if (isToken(TokenType::PUNCTUATION, u8"(")) {
            value = parseInstructionFunctionCall(identifier);
        } else {
            value = std::make_unique<VariableReferenceAST>(identifier);
        }
    } else if (isToken(TokenType::OPERATOR, u8"(")) {
        getNextToken();
        value = parseExpression();
        if (!isToken(TokenType::OPERATOR, u8")")) return nullptr;
        getNextToken();
    } else {
        return nullptr;
    }

    return value;
}
