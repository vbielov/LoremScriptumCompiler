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
    if (left == nullptr)
        return nullptr;
    
    if (isExpressionEnd()) return left;
    
    if (!isToken(TokenType::OPERATOR)) {
        logError(currentLine, u8"Syntax Error: operator expected!");
        return nullptr;
    }

    if (isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        logError(currentLine, u8"Syntax Error: assign operator is not allowed here!");
        return nullptr;
    }

    auto op = *operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);
    getNextToken();
    
    auto right = parseExpressionSingle();
    if (right == nullptr){
        logError(currentLine, u8"Syntax Error: right side of binary operator is missing or invalid!");
        return nullptr;
    }

    if (isExpressionEnd()) {
        return std::make_unique<BinaryOperatorAST>(std::u8string(op.first), std::move(left), std::move(right), currentLine);
    }

    if (!isToken(TokenType::OPERATOR)) {
        logError(currentLine, u8"Syntax Error: expected operator!");   
        return nullptr;
    }
    if (isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        logError(currentLine, u8"Syntax Error: assign operator is not allowed here!");   
        return nullptr;
    }
    auto nextOp = *operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);

    getNextToken();
    auto nextExpression = parseExpression();
    if (nextExpression == nullptr) 
        return nullptr;

    if (op.second <= nextOp.second) {
        std::unique_ptr<AST> priorityOp = std::make_unique<BinaryOperatorAST>(std::u8string(op.first), std::move(left), std::move(right), currentLine);
        return std::make_unique<BinaryOperatorAST>(std::u8string(nextOp.first), std::move(priorityOp), std::move(nextExpression), currentLine);
    } else {
        std::unique_ptr<AST> priorityOp = std::make_unique<BinaryOperatorAST>(std::u8string(nextOp.first), std::move(right), std::move(nextExpression), currentLine);
        return std::make_unique<BinaryOperatorAST>(std::u8string(op.first), std::move(left), std::move(priorityOp), currentLine);
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
 *    - [I, II, ...]
 *
 * Trick of sign number -I -> 0 - I
 *
 */
std::unique_ptr<AST> Parser::parseExpressionSingle() {
    if (isUnaryOperator()) {
        std::unique_ptr<AST> value;
        std::u8string sign = m_currentToken->value;

        getNextToken();
        if (isUnaryOperator()) {
            logError(currentLine, u8"Syntax Error: double unary operator is not allowed!");
            return nullptr;
        }
            

        value = parseExpressionSingle();
        if (value == nullptr)
            return nullptr;

        if (sign == operators::PLUS || sign == operators::MINUS) {
            auto lhs = std::make_unique<NumberAST>(0, currentLine);
            return std::make_unique<BinaryOperatorAST>(sign, std::move(lhs), std::move(value), currentLine);
        } else if (sign == operators::NOT){
            // only left is important
            auto rhs = std::make_unique<NumberAST>(0, currentLine);
            return std::make_unique<BinaryOperatorAST>(sign, std::move(value), std::move(rhs), currentLine);
        }

        logError(currentLine, u8"Syntax Error: invalid unary operator!");
        return nullptr;
    }

    if (isToken(TokenType::NUMBER)) {
        int intValue;
        if (!toArabicConverter(m_currentToken->value, &intValue)) {
            logError(currentLine, u8"Syntax Error: failure to understand Roman numeral, Optime vale!");
            return nullptr;
        }

        auto value = std::make_unique<NumberAST>(intValue, currentLine);
        getNextToken();
        return value;
    }

    if (isToken(TokenType::LITERAL)) {
        if (m_currentToken->value.length() > 1) {
            if (m_currentToken->value[0] != '\\' || m_currentToken->value.length() != 2) {
                logWarning(currentLine, u8"Syntax Error: Char can't be longer then a single character!");
            }
        }

        char8_t letter = m_currentToken->value[0];

        if(m_currentToken->value == u8"\\0") letter = '\0';
        if(m_currentToken->value == u8"\\n") letter = '\n';
        if(m_currentToken->value == u8"\\t") letter = '\t';
        if(m_currentToken->value == u8"\\r") letter = '\r';
        
        auto value = std::make_unique<CharAST>(letter, currentLine);
        getNextToken();
        return value;
    }

    if (isToken(TokenType::STRING)) {
        std::vector<std::unique_ptr<AST>> letters;
        for (size_t i = 0; i < m_currentToken->value.length(); i++) {
            letters.push_back(std::make_unique<CharAST>(m_currentToken->value[i], currentLine));
        }
        letters.push_back(std::make_unique<CharAST>(u8'\0', currentLine)); // null terminator
        auto strArr = std::make_unique<ArrayAST>(std::move(letters), currentLine);
        getNextToken(); // eat string
        return strArr;
    }

    if (isToken(TokenType::BOOL)) {
        bool state = m_currentToken->value == boolean_types::TRUE;
        getNextToken(); // eat bool
        return std::make_unique<BoolAST>(state, currentLine);
    } 
    if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken->value;
        getNextToken(); // eat identifier
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
            return parseExpressionFunctionCall(identifier);
        } else if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
            getNextToken(); // eat [

            std::unique_ptr<AST> index = parseExpression();
            if (index == nullptr) {
                logError(currentLine, u8"Syntax Error: Expected index to index array!");
                return nullptr;
            }

            if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
                logError(currentLine, u8"Syntax Error: closing array bracket ']' expected!");
                return nullptr;
            }
            getNextToken(); // eat ]
            return std::make_unique<AccessArrayElementAST>(identifier, std::move(index), currentLine);
        } else {
            return std::make_unique<VariableReferenceAST>(identifier, currentLine);
        }
    }
    if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
        getNextToken();
        auto value = parseExpression();
        if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
                logError(currentLine, u8"Syntax Error: closing array bracket ')' expected!");
                return nullptr;
            }
        getNextToken();
        return value;
    } 
    if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
        return parseArray();
    }

    logError(currentLine, u8"Syntax Error: Token not recognised...!");
    return nullptr;
}

/**
 * Array Initialization
 * 
 * Examples:
 *      - [I]
 *      - [I, II]
 *        ^ we are always here
 */
std::unique_ptr<ArrayAST> Parser::parseArray() {
    getNextToken(); // eat '['
    std::vector<std::unique_ptr<AST>> elements;
    // get expression from each index
    while (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) { 
        if (isToken(TokenType::EOF_TOKEN)) {
            logError(currentLine, u8"Syntax Error: closing bracket for array initialization ']' expected!");
            return nullptr;
        }

        std::unique_ptr<AST> element = parseExpression();
        if (element == nullptr) {
            logError(currentLine, u8"Syntax Error: invalid expression during array initialization!");
            return nullptr;
        }

        elements.push_back(std::move(element));

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken(); // eat ','
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
            break;
        }
        logError(currentLine, u8"Syntax Error: expected ',' or ']' during array initialization!");
        return nullptr;
    }
    getNextToken(); // eat ']'
    return std::make_unique<ArrayAST>(std::move(elements), currentLine);
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
            logError(currentLine, u8"Syntax Error: invalid expression inside function call!");
            return nullptr;
        }
            
        args.push_back(std::move(expression));

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
                logError(currentLine, u8"Syntax Error: closing function call bracket ')' expected!");
                return nullptr;
            } 
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
            break;
        }

        logError(currentLine, u8"Syntax Error: closing function call bracket ')' expected!");
        return nullptr;
    }

    getNextToken();
    return std::make_unique<FuncCallAST>(identifier, std::move(args), currentLine);
}
