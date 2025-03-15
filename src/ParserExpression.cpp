#include "Parser.hpp"
#include "RomanNumber.hpp"
#include "ErrorHandler.hpp"

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
    if (left == nullptr){
        buildString(currentLine, u8"Syntax Error: left side of Expression is missing!");
        return nullptr;
    } 
    
    if (isExpressionEnd()) return left;
    
    if (!isToken(TokenType::OPERATOR) || isToken(TokenType::OPERATOR, operators::ASSIGN)) 
        {
            buildString(currentLine, u8"Syntax Error: missing or wrong operator!");
            return nullptr;
        }

    auto op = *operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);
    getNextToken();
    
    auto right = parseExpressionSingle();
    if (right == nullptr){
        
        buildString(currentLine, u8"Syntax Error: right side of Expression is missing!");
         return nullptr;
        }

    if (isExpressionEnd()) {
        return std::make_unique<BinaryOperatorAST>(std::u8string(op.first), std::move(left), std::move(right));
    }

    if (!isToken(TokenType::OPERATOR) || isToken(TokenType::OPERATOR, operators::ASSIGN)) return nullptr;
    auto nextOp = *operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);

    getNextToken();
    auto nextExpression = parseExpression();
    if (nextExpression == nullptr) {
        buildString(currentLine, u8"Syntax Error: failure to find next Expression!");
        return nullptr;
    };

    if (op.second <= nextOp.second) {
        std::unique_ptr<AST> priorityOp = std::make_unique<BinaryOperatorAST>(std::u8string(op.first), std::move(left), std::move(right));
        return std::make_unique<BinaryOperatorAST>(std::u8string(nextOp.first), std::move(priorityOp), std::move(nextExpression));
    } else {
        std::unique_ptr<AST> priorityOp = std::make_unique<BinaryOperatorAST>(std::u8string(nextOp.first), std::move(right), std::move(nextExpression));
        return std::make_unique<BinaryOperatorAST>(std::u8string(op.first), std::move(left), std::move(priorityOp));
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
        std::u8string sign = m_currentToken->value;

        getNextToken();
        if (isUnaryOperator()) {
            buildString(currentLine, u8"Syntax Error: double unary operator is not allowed!");
            return nullptr;
        }
            

        value = parseExpressionSingle();
        if (value == nullptr) {        
                buildString(currentLine, u8"Syntax Error: failure to parse Expression, invalid Value!");
                return nullptr;
            };

        if (sign == operators::PLUS || sign == operators::MINUS) {
            auto lhs = std::make_unique<NumberAST>(0);
            return std::make_unique<BinaryOperatorAST>(sign, std::move(lhs), std::move(value));
        } else if (sign == operators::NOT){
            // only left is important
            auto rhs = std::make_unique<NumberAST>(0);
            return std::make_unique<BinaryOperatorAST>(sign, std::move(value), std::move(rhs));
        }

        buildString(currentLine, u8"Syntax Error: to parse Expression failure, check your Operators!");
        return nullptr;
    }

    if (isToken(TokenType::NUMBER)) {
        int intValue;
        if (!toArabicConverter(m_currentToken->value, &intValue)) {
            buildString(currentLine, u8"Syntax Error: failure to understand Roman numeral, Optime vale!");
            return nullptr;
        }

        value = std::make_unique<NumberAST>(intValue);
        getNextToken();
    } else if (isToken(TokenType::LITERAL)) {
        char letter = m_currentToken->value[0];

        if(m_currentToken->value == u8"\\0") letter = '\0';
        if(m_currentToken->value == u8"\\n") letter = '\n';
        if(m_currentToken->value == u8"\\t") letter = '\t';
        if(m_currentToken->value == u8"\\r") letter = '\r';
        
        value = std::make_unique<CharAST>(letter);
        getNextToken();

    } else if (isToken(TokenType::BOOL)) {
        bool state = m_currentToken->value == boolean_types::TRUE;
        getNextToken(); // eat bool
        return std::make_unique<BoolAST>(state);

    } else if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken->value;
        getNextToken(); // eat identifier
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
            value = parseExpressionFunctionCall(identifier);
        } else if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
            getNextToken(); // eat [

            std::unique_ptr<AST> index = parseExpression();
            if (!index) {
                // printError("Expected index for indexing array");
                buildString(currentLine, u8"Syntax Error: Expected index to index array with!");
                return nullptr;
            }

            if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
                // printError("Expected ']' after array index accessing");
                buildString(currentLine, u8"Syntax Error: expeted ']' after array index access!");
                return nullptr;
            }
            getNextToken(); // eat ]
            return std::make_unique<AccessArrayElementAST>(identifier, std::move(index));
        } else {
            value = std::make_unique<VariableReferenceAST>(identifier);
        }
    } else if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
        getNextToken();
        value = parseExpression();
        if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
                buildString(currentLine, u8"Syntax Error: didnt close ()!");
                return nullptr;
            }
        getNextToken();
    } else {
        buildString(currentLine, u8"Syntax Error: Token not recognised...!");
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
    while (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
        auto expression = parseExpression();
        if (expression == nullptr) {
            buildString(currentLine, u8"Syntax Error: missing expression!");
            return nullptr;
        }
            
        args.push_back(std::move(expression));

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) return nullptr;
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
            break;
        }

        // printError("Error: Expected ) in function call");
        buildString(currentLine, u8"Syntax Error: closing ) is expected in function call!");

        return nullptr;
    }

    getNextToken();
    return std::make_unique<FuncCallAST>(identifier, std::move(args));
}
