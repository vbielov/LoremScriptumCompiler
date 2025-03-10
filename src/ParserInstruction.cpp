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
        if (m_blockCount == 0 && !m_isTest) {
            // is Top level declaration
            auto declaration = parseInstructionDeclaration();
            if (declaration == nullptr) return nullptr;

            m_topLevelDeclarations.push_back(std::move(declaration));
            
            auto pseudoEmptyInstr = std::vector<std::unique_ptr<AST>>();
            return std::make_unique<BlockAST>(std::move(pseudoEmptyInstr));
        } else {
            return parseInstructionDeclaration();
        }
    }
    if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken.value;
        getNextToken();

        if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) 
            return parseInstructionArrayAssignment(identifier);
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) 
            return parseExpressionFunctionCall(identifier);
        if (isToken(TokenType::OPERATOR)) {
            if (isToken(operators::ASSIGN)) 
                return parseInstructionAssignment(identifier);
            else 
                return parseInstructionShorthand(identifier);
        }
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
    if (expression == nullptr) 
        return nullptr;
    
    std::unique_ptr<AST> reference = std::make_unique<VariableReferenceAST>(identifier);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(reference), std::move(expression));
}

// the same as above, but for arrays, current token is '['
std::unique_ptr<AST> Parser::parseInstructionArrayAssignment(const std::u8string& identifier) {
    getNextToken(); // eat '['
    
    std::unique_ptr<AST> index = parseExpression();
    if (!index) {
        printError("Expected index for indexing array");
        return nullptr;
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
        printError("Expected ']' after array indexing");
        return nullptr;
    }
    getNextToken(); // eat ']'

    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        printError("Why would you just access an array?, assign something to it!");
        return nullptr;
    }
    getNextToken(); // eat '='
    
    std::unique_ptr<AST> expression = parseExpression();
    if (expression == nullptr) 
        return nullptr;
    
    std::unique_ptr<AST> arrReference = std::make_unique<AccessArrayElementAST>(identifier, std::move(index));
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(arrReference), std::move(expression));
}

/**
 * An Increment instruction is a shorthand version of var = var -/+ 1
 * 
 * currentToken is at second token of assignment. It is always Operator but not '='
 * 
 * Examples:
 *      - var++
 *      - var--
 *      - var-= I
 *      - var*= I
 *           ^ we are always here
 */
std::unique_ptr<AST> Parser::parseInstructionShorthand(const std::u8string& identifier) {
    if (!isToken(operators::PLUS) && !isToken(operators::MINUS) && !isToken(operators::MULTIPLY) && !isToken(operators::DIVIDE) && !isToken(operators::POWER)) 
        return nullptr;

    auto op = operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken.value);

    getNextToken();
    if (!isToken(TokenType::OPERATOR)) return nullptr;

    std::unique_ptr<AST> expression;
    if (op->first == m_currentToken.value && (op->first == operators::PLUS || op->first == operators::MINUS)) {
        // var++ or var--
        getNextToken();
        expression = std::make_unique<NumberAST>(1);

        if (!isToken(TokenType::EOF_TOKEN) && !isToken(TokenType::NEW_LINE) && !isToken(TokenType::PUNCTUATION)) return nullptr;
    } else if (isToken(TokenType::OPERATOR, operators::ASSIGN)) {

        // var -= I
        getNextToken();
        expression = parseExpression();
    } else return nullptr;

    std::unique_ptr<AST> rhs = std::make_unique<BinaryOperatorAST>(std::u8string(op->first), std::make_unique<VariableReferenceAST>(identifier), std::move(expression));
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::make_unique<VariableReferenceAST>(identifier), std::move(rhs));
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
    if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
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
        if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
            printError("Expected ']' in array declaration");
            return nullptr;
        }
        getNextToken(); // eat ']'
    }

    if (!isToken(TokenType::IDENTIFIER)) 
        return nullptr;
    std::u8string identifier = m_currentToken.value;
    getNextToken(); // eat identifier

    if (isToken(TokenType::NEW_LINE)) {
        if (isArray) {
            return std::make_unique<ArrayAST>(type, identifier, arrSize);
        }
        return std::make_unique<VariableDeclarationAST>(type, identifier);
    }
    
    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) 
        return nullptr;
    getNextToken(); // eat '='

    // λ
    if (isToken(TokenType::KEYWORD, keywords::FUNCTION)) { 
        return parseInstructionFunction(type, identifier, isArray, arrSize);
    // apere
    } else if (isToken(TokenType::KEYWORD, keywords::INCLUDE)) { 
        getNextToken(); // eat apere
        return parseInstructionPrototype(type, identifier, isArray, arrSize);
    // '['
    } else if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) { 
        getNextToken(); // eat '['
        std::vector<std::unique_ptr<AST>> elements;
        // while != ']'
        while (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE) && !isToken(TokenType::EOF_TOKEN)) { 
            std::unique_ptr<AST> element = parseExpression();
            elements.push_back(std::move(element));
            if (!elements.back() || isToken(TokenType::EOF_TOKEN)) {
                printError("Error when parsing elements of array initialization");
                return nullptr;
            }
            if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
                getNextToken(); // eat ','
            }
        }
        
        if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) { // ']'
            printError("Expected ] in array initalization");
            return nullptr;
        }
        getNextToken(); // eat ']'

        return std::make_unique<ArrayAST>(type, identifier, arrSize, std::move(elements));
    }
    
    std::unique_ptr<VariableDeclarationAST> declaration;
    if (isArray)
        declaration = std::make_unique<ArrayAST>(type, identifier, arrSize);
    else
        declaration = std::make_unique<VariableDeclarationAST>(type, identifier);

    auto expression = parseExpression();
    if (expression == nullptr) 
        return nullptr;

    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(declaration), std::move(expression));
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
    while (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE) && !isToken(TokenType::EOF_TOKEN)) {
        if (!isToken(TokenType::TYPE)) 
            return nullptr;
        std::u8string argType = m_currentToken.value;
        getNextToken(); //  eat type
        
        int arrSize = -1;
        if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
            getNextToken(); // eat '['
            if (!isToken(TokenType::NUMBER) || !toArabicConverter(m_currentToken.value, &arrSize)) {
                printError("Expected number after '[', when dealing with array func-argument");
                return nullptr;
            }
            getNextToken(); // eat number
            if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
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
        if (arrSize != -1)
            arg = std::make_unique<ArrayAST>(argType, identifier, arrSize);
        else
            arg = std::make_unique<VariableDeclarationAST>(argType, identifier);
            
        args.push_back(std::move(arg));

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE) || isToken(TokenType::EOF_TOKEN)) 
                return nullptr;

            continue;
        }
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
            break;
        }

        printError("Error: Expected ')' in function declaration");
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
    if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) return nullptr;

    auto prototype = parseInstructionPrototype(type, identifier, returnsArray, arrSize);
    if (prototype == nullptr) return nullptr;

    // for the case if : is on the next line
    while(isToken(TokenType::NEW_LINE)) {
        getNextToken();
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::BLOCK_OPEN)) {
        printError("Expected ':' when parsin Block");
        return nullptr;
    }
    auto funcBlock = parseBlock();
    if (funcBlock == nullptr) return nullptr;

    return std::make_unique<FunctionAST>(std::move(prototype), std::move(funcBlock));
}
