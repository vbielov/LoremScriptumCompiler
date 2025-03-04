#include "Parser.hpp"

/**
 * An Instruction can be a declaration, assignment or function call
 * 
 * currentToken is at first token of instruction
 * 
 * Examples:
 *      - numerus var = I
 *      - var = I
 *      - func()
 *      - var++
 *      - var--
 */
std::unique_ptr<AST> Parser::parseInstruction() { 
    if (isToken(TokenType::TYPE)) {
        return parseInstructionDeclaration();
    }
    if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken.value;
        getNextToken();
        if (isToken(TokenType::PUNCTUATION, u8"(")) return parseExpressionFunctionCall(identifier);
        if (isToken(TokenType::OPERATOR, u8"=")) return parseInstructionAssignment(identifier);
        if (isToken(TokenType::OPERATOR, u8"+") || isToken(TokenType::OPERATOR, u8"-")) return parseInstructionIncrement(identifier);
    }
    return nullptr;
}


/**
 * An Assignment sets an identifer to the value of an expression
 * 
 * currentToken is at second token of assignment. It is always '='
 * 
 * Examples:
 *      - var = I
 *      - var = I + I
 *      - var = (I + I)
 *      - var = func()
 *            ^ we are always here
 */
std::unique_ptr<AST> Parser::parseInstructionAssignment(const std::u8string& identifier) {
    getNextToken();
    auto expression = parseExpression();
    if (expression == nullptr) return nullptr;
    
    std::unique_ptr<AST> reference = std::make_unique<VariableReferenceAST>(identifier);
    return std::make_unique<BinaryOperatorAST>(u8"=", std::move(reference), std::move(expression));
}


/**
 * An Increment instruction is a shorthand version of var = var -/+ 1
 * 
 * currentToken is at second token of assignment. It is always '+' OR '-'
 * 
 * Examples:
 *      - var++
 *      - var--
 *           ^ we are always here
 */
std::unique_ptr<AST> Parser::parseInstructionIncrement(const std::u8string& identifier) {
    auto op = *BINARY_OPERATION_PRIORITY.find(m_currentToken.value);
    auto assign = *BINARY_OPERATION_PRIORITY.find(u8"=");

    getNextToken();
    if (!isToken(TokenType::OPERATOR) || op.first != m_currentToken.value) return nullptr;

    getNextToken();

    std::unique_ptr<AST> pseudoIncrement = std::make_unique<BinaryOperatorAST>(op.first, std::make_unique<VariableReferenceAST>(identifier), std::make_unique<NumberAST>(1));
    return std::make_unique<BinaryOperatorAST>(assign.first, std::make_unique<VariableReferenceAST>(identifier), std::move(pseudoIncrement));
}

/**
 * A Declaration can be just init or init and assign. It can also be a function declaration
 *
 * currentToken is at first token of declaration. It is always TYPE
 *
 * Examples:
 *    - numerus var
 *    - numerus var = I
 *    - numerus var = λ(...): [Block] ;
 *    - nihil   var = λ(...): [Block] ;
 */
std::unique_ptr<AST> Parser::parseInstructionDeclaration() {
    auto type = m_currentToken.value;

    getNextToken();
    if (!isToken(TokenType::IDENTIFIER)) return nullptr;

    auto identifier = m_currentToken.value;

    getNextToken();
    if (isToken(TokenType::NEW_LINE)) {
        return std::make_unique<VariableDeclarationAST>(type, identifier);
    }

    if (!isToken(TokenType::OPERATOR, u8"=")) return nullptr;

    getNextToken();
    if (isToken(TokenType::KEYWORD, u8"λ")) {
        return parseInstructionFunction(type, identifier);
    } else if (isToken(TokenType::KEYWORD, u8"apere")) {
        getNextToken(); // eat apere
        return parseInstructionPrototype(type, identifier);
    }
    
    auto declaration = std::make_unique<VariableDeclarationAST>(type, identifier);
    auto expression = parseExpression();
    if (expression == nullptr) return nullptr;

    return std::make_unique<BinaryOperatorAST>(u8"=", std::move(declaration), std::move(expression));
}

/**
 * A function prototype is a function declaration, 
 * this function can be defined in our program or somewhere else in linked library
 *
 * currentToken is at (.
 *
 * Examples:
 *      - numerus add  = λ(numerus a, numerus b): [Block] ;
 *                        ^ we are always here
 */
std::unique_ptr<FunctionPrototypeAST> Parser::parseInstructionPrototype(const std::u8string& type, const std::u8string& identifier) {
    getNextToken(); // eat '('
    std::vector<std::unique_ptr<VariableDeclarationAST>> args;
    while (!isToken(TokenType::PUNCTUATION, u8")")) {
        if (!isToken(TokenType::TYPE)) return nullptr;
        auto type = m_currentToken.value;

        getNextToken();
        if (!isToken(TokenType::IDENTIFIER)) return nullptr;

        auto arg = std::make_unique<VariableDeclarationAST>(type, m_currentToken.value);
        args.push_back(std::move(arg));

        getNextToken();
        if (isToken(TokenType::PUNCTUATION, u8",")) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, u8")")) return nullptr;
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, u8")")) {
            break;
        }

        printError("Error: Expected ) in function declaration");
        return nullptr;
    }

    getNextToken();

    return std::make_unique<FunctionPrototypeAST>(type, identifier, std::move(args));
}

/**
 * A function can have many args. args are variable declarations. type is return type and identifier is name of the function.
 *
 * currentToken is at λ.
 *
 * Examples:
 *      - numerus func = λ(): [Block] ;
 *      - nihil func   = λ(numerus a): [Block] ;
 *      - numerus add  = λ(numerus a, numerus b): [Block] ;
 *                       ^ we are always here
 */
std::unique_ptr<FunctionAST> Parser::parseInstructionFunction(const std::u8string& type, const std::u8string& identifier) {
    if (m_blockCount != 0) {
        printError("Function Declaration is only allowed at top-level");
        return nullptr;
    }

    getNextToken(); // eat λ
    if (!isToken(TokenType::PUNCTUATION, u8"(")) return nullptr;

    auto prototype = parseInstructionPrototype(type, identifier);
    if (prototype == nullptr) return nullptr;

    // for the case if : is on the next line
    while(isToken(TokenType::NEW_LINE)) {
        getNextToken();
    }

    if (!isToken(TokenType::PUNCTUATION, u8":")) {
        printError("Expected ':' when parsin Block");
        return nullptr;
    }
    auto funcBlock = parseBlock();
    if (funcBlock == nullptr) return nullptr;

    return std::make_unique<FunctionAST>(std::move(prototype), std::move(funcBlock));
}
