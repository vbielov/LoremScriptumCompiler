#include "Parser.hpp"
#include "RomanNumber.hpp"

/**
 * An Expression always calculates a value of some type. Expression end on new line, eof or punctuation.
 * Operator '=' is not allowed - it is handled by parseInstructionAssignment()
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
    
    if (isExpressionEnd()) return left;
    
    if (!isToken(TokenType::OPERATOR) || isToken(TokenType::OPERATOR, u8"=")) return nullptr;
    auto op = *BINARY_OPERATION_PRIORITY.find(m_currentToken.value);
    getNextToken();
    
    auto right = parseExpressionSingle();
    if (right == nullptr) return nullptr;

    if (isExpressionEnd()) {
        return std::make_unique<BinaryOperatorAST>(op.first, std::move(left), std::move(right));
    }

    if (!isToken(TokenType::OPERATOR) || isToken(TokenType::OPERATOR, u8"=")) return nullptr;
    auto nextOp = *BINARY_OPERATION_PRIORITY.find(m_currentToken.value);

    getNextToken();
    auto nextExpression = parseExpression();
    if (nextExpression == nullptr) return nullptr;

    if (op.second <= nextOp.second) {
        std::unique_ptr<AST> priorityOp = std::make_unique<BinaryOperatorAST>(op.first, std::move(left), std::move(right));
        return std::make_unique<BinaryOperatorAST>(nextOp.first, std::move(priorityOp), std::move(nextExpression));
    } else {
        std::unique_ptr<AST> priorityOp = std::make_unique<BinaryOperatorAST>(nextOp.first, std::move(right), std::move(nextExpression));
        return std::make_unique<BinaryOperatorAST>(op.first, std::move(left), std::move(priorityOp));
    }
}

/**
 * A single Expression is a value without using operators that need two values (e.g. addition)
 * Can also be with unary operators, but only single. (vaid: -var   invalid: --var)
 *
 * currentToken is at first token of single expression.
 *
 * Examples:
 *    - I
 *    - func()
 *    - -I
 *    - +var
 *    - -var
 *    - ¬bool
 *    - (I + II *IV)
 *
 * Trick of sign number -I -> 0 - I
 *
 */
std::unique_ptr<AST> Parser::parseExpressionSingle() {
    std::unique_ptr<AST> value;

    if (isUnaryOperator()) {
        auto sign = m_currentToken.value;

        getNextToken();
        if (isUnaryOperator()) return nullptr;
        value = parseExpressionSingle();
        if (value == nullptr) return nullptr;

        if (sign == u8"+" || sign == u8"-") {
            auto lhs = std::make_unique<NumberAST>(0);
            return std::make_unique<BinaryOperatorAST>(sign, std::move(lhs), std::move(value));
        } else if (sign == u8"¬"){
            // TODO: Add BoolAST
            return nullptr;
        }
        return nullptr;
    }

    if (isToken(TokenType::NUMBER)) {
        int intValue;
        if (!toArabicConverter(m_currentToken.value, &intValue)) return nullptr;
        value = std::make_unique<NumberAST>(intValue);
        getNextToken();
    } else if (isToken(TokenType::LITERAL)) {
        value = std::make_unique<CharAST>(m_currentToken.value[0]);  // TODO: is this correct?
        getNextToken();
    } else if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken.value;
        getNextToken();
        if (isToken(TokenType::PUNCTUATION, u8"(")) {
            value = parseExpressionFunctionCall(identifier);
        } else {
            value = std::make_unique<VariableReferenceAST>(std::move(identifier));
        }
    } else if (isToken(TokenType::PUNCTUATION, u8"(")) {
        getNextToken();
        value = parseExpression();
        if (!isToken(TokenType::PUNCTUATION, u8")")) return nullptr;
        getNextToken();
    } else {
        return nullptr;
    }

    return value;
}

/**
 * A Function call can have many args and is surrounded by ( )
 *
 * currentToken is at SECOND token of function call. It is always '('. An arg is an expression
 *
 * Examples:
 *      - func()
 *      - func(I)
 *      - func(I, I)
 *      - func(var)
 *      - func(func())
 *      - func(var + (I - II))
 *            ^ we are always here
 */
std::unique_ptr<FuncCallAST> Parser::parseExpressionFunctionCall(const std::u8string& identifier) {
    getNextToken();

    std::vector<std::unique_ptr<AST>> args;
    while (!isToken(TokenType::PUNCTUATION, u8")")) {
        auto expression = parseExpression();
        if (expression == nullptr) return nullptr;

        args.push_back(std::move(expression));

        if (isToken(TokenType::PUNCTUATION, u8",")) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, u8")")) return nullptr;
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, u8")")) {
            break;
        }

        printError("Error: Expected ) in function call");
        return nullptr;
    }

    getNextToken();
    return std::make_unique<FuncCallAST>(identifier, std::move(args));
}
