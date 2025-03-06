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
 */
std::unique_ptr<AST> Parser::parseInstruction() { 
    if (isToken(TokenType::TYPE)) {
        return parseInstructionDeclaration();
    }
    if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken.value;
        getNextToken();

        if (isToken(TokenType::PUNCTUATION, u8"[")) return parseInstructionArrayAssignment(identifier);
        if (isToken(TokenType::PUNCTUATION, u8"(")) return parseExpressionFunctionCall(identifier);
        if (isToken(TokenType::OPERATOR, u8"=")) return parseInstructionAssignment(identifier);
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

// the same as above, but for arrays, current token is '['
std::unique_ptr<AST> Parser::parseInstructionArrayAssignment(const std::u8string& identifier)
{
    getNextToken(); // eat '['
    
    std::unique_ptr<AST> index = parseExpression();
    if (!index) {
        printError("Expected index for indexing array");
        return nullptr;
    }

    if (!isToken(TokenType::PUNCTUATION, u8"]")) {
        printError("Expected ']' after array indexing");
        return nullptr;
    }
    getNextToken(); // eat ']'

    if (!isToken(TokenType::OPERATOR, u8"=")) {
        printError("Why would you just access an array?, assign something to it!");
        return nullptr;
    }
    getNextToken(); // eat '='
    
    std::unique_ptr<AST> expression = parseExpression();
    if (expression == nullptr) return nullptr;
    
    std::unique_ptr<AST> arrReference = std::make_unique<AccessArrayElementAST>(identifier, std::move(index));
    return std::make_unique<BinaryOperatorAST>(u8"=", std::move(arrReference), std::move(expression));
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
    std::u8string type = m_currentToken.value;

    getNextToken();

    bool isArray = false;
    int arrSize = 0;
    if (isToken(TokenType::PUNCTUATION, u8"[")) {
        // it's array declaration
        isArray = true;
        // TODO(Vlad): maybe thats can be a more then a number?
        getNextToken(); // eat '['
        if (!isToken(TokenType::NUMBER)) {
            printError("Expected number after [ for array declaration");
            return nullptr; 
        }
        toArabicConverter(m_currentToken.value, &arrSize);
        getNextToken(); // eat number
        if (!isToken(TokenType::PUNCTUATION, u8"]")) {
            printError("Expected ']' in array declaration");
            return nullptr;
        }
        getNextToken(); // eat ]
    }

    if (!isToken(TokenType::IDENTIFIER)) return nullptr;
    auto identifier = m_currentToken.value;
    
    getNextToken();
    if (isToken(TokenType::NEW_LINE)) {
        if (isArray) {
            return std::make_unique<ArrayAST>(type, identifier, arrSize);
        }
        return std::make_unique<VariableDeclarationAST>(type, identifier);
    }
    
    if (!isToken(TokenType::OPERATOR, u8"=")) return nullptr;
    
    getNextToken();
    if (isToken(TokenType::KEYWORD, u8"λ")) {
        return parseInstructionFunction(type, identifier, isArray, arrSize);
    } else if (isToken(TokenType::KEYWORD, u8"apere")) {
        getNextToken(); // eat apere
        return parseInstructionPrototype(type, identifier, isArray, arrSize);
    } else if (isToken(TokenType::PUNCTUATION, u8"[")) {
        getNextToken(); // eat '['
        std::vector<std::unique_ptr<AST>> elements;
        while (!isToken(TokenType::PUNCTUATION, u8"]") && !isToken(TokenType::EOF_TOKEN)) {
            std::unique_ptr<AST> element = parseExpression();
            elements.push_back(std::move(element));
            if (!elements.back() || isToken(TokenType::EOF_TOKEN)) {
                printError("Error when parsing elements of array initialization");
                return nullptr;
            }
            if (isToken(TokenType::PUNCTUATION, u8",")) {
                getNextToken(); // eat ','
            }
        }
        if (!isToken(TokenType::PUNCTUATION, u8"]")) {
            printError("Expected ] in array initalization");
            return nullptr;
        }
        getNextToken(); // eat ']'

        return std::make_unique<ArrayAST>(type, identifier, arrSize, std::move(elements));
    }
    
    std::unique_ptr<VariableDeclarationAST> declaration;
    if (isArray) {
        declaration = std::make_unique<ArrayAST>(type, identifier, arrSize);
    } else {
        declaration = std::make_unique<VariableDeclarationAST>(type, identifier);
    }
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
std::unique_ptr<FunctionPrototypeAST> Parser::parseInstructionPrototype(const std::u8string& type, const std::u8string& identifier, bool returnsArray, int arrSize) {
    getNextToken(); // eat '('
    std::vector<std::unique_ptr<VariableDeclarationAST>> args;
    while (!isToken(TokenType::PUNCTUATION, u8")") && !isToken(TokenType::EOF_TOKEN)) {
        if (!isToken(TokenType::TYPE)) 
            return nullptr;
        std::u8string argType = m_currentToken.value;
        getNextToken(); //  eat type
        
        int arrSize = -1;
        if (isToken(TokenType::PUNCTUATION, u8"[")) {
            getNextToken(); // eat '['
            if (!isToken(TokenType::NUMBER) || !toArabicConverter(m_currentToken.value, &arrSize)) {
                printError("Expected number after '[', when dealing with array func-argument");
                return nullptr;
            }
            getNextToken(); // eat number
            if (!isToken(TokenType::PUNCTUATION, u8"]")) {
                printError("Expected ']', when dealing with array func-argument");
                return nullptr;
            }
            getNextToken(); // eat ']'
        }

        if (!isToken(TokenType::IDENTIFIER)) 
            return nullptr;

        std::u8string identifier = m_currentToken.value;
        getNextToken(); // eat identifier

        std::unique_ptr<VariableDeclarationAST> arg;
        if (arrSize != -1) {
            arg = std::make_unique<ArrayAST>(argType, identifier, arrSize);
        } else {
            arg = std::make_unique<VariableDeclarationAST>(argType, identifier);
        }
        args.push_back(std::move(arg));

        if (isToken(TokenType::PUNCTUATION, u8",")) {
            getNextToken();
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, u8")")) {
            break;
        }

        printError("Error: Expected ) in function declaration");
        return nullptr;
    }

    getNextToken();

    return std::make_unique<FunctionPrototypeAST>(type, identifier, std::move(args), returnsArray, arrSize);
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
std::unique_ptr<FunctionAST> Parser::parseInstructionFunction(const std::u8string& type, const std::u8string& identifier, bool returnsArray, int arrSize) {
    if (m_blockCount != 0) {
        printError("Function Declaration is only allowed at top-level");
        return nullptr;
    }

    getNextToken(); // eat λ
    if (!isToken(TokenType::PUNCTUATION, u8"(")) return nullptr;

    auto prototype = parseInstructionPrototype(type, identifier, returnsArray, arrSize);
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
