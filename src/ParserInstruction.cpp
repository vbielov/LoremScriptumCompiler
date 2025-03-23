#include "Parser.hpp"
#include "ErrorHandler.hpp"

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
    bool isStructType = m_structHashMap.find(m_currentToken->value) != m_structHashMap.end();

    if (isToken(TokenType::TYPE) || isStructType) {
        std::unique_ptr<AST> declaration = parseInstructionDeclaration();

        if (m_blockCount == 0 && !m_isTest) {
            // is Top level declaration
            
            if (declaration == nullptr) {
                logError(currentLine, u8"Syntax Error: Invalid declaration!");
                return nullptr;
            }
            m_topLevelDeclarations.push_back(std::move(declaration));
            
            auto pseudoEmptyInstr = std::vector<std::unique_ptr<AST>>();
            return std::make_unique<BlockAST>(std::move(pseudoEmptyInstr), currentLine);
        } else {
            return declaration;
        }
    }
    if (isToken(TokenType::IDENTIFIER)) {
        auto identifier = m_currentToken->value;
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
    logError(currentLine, u8"Syntax Error: expected identifier!");
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
    if (expression == nullptr) {
        logError(currentLine, u8"Syntax Error: invalid assignment expression!");
        return nullptr;
    }
    
    std::unique_ptr<AST> reference = std::make_unique<VariableReferenceAST>(identifier, currentLine);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(reference), std::move(expression), currentLine);
}

// the same as above, but for arrays, current token is '['
std::unique_ptr<AST> Parser::parseInstructionArrayAssignment(const std::u8string& identifier) {
    getNextToken(); // eat '['
    
    std::unique_ptr<AST> index = parseExpression();
    if (!index) {
        logError(currentLine, u8"Syntax Error: expected index for indexing array!");
        return nullptr;
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
        logError(currentLine, u8"Syntax Error: expected ']' after array indexing!");
        return nullptr;
    }
    getNextToken(); // eat ']'

    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        logError(currentLine, u8"Syntax Error: assign operator '=' expected!");
        return nullptr;
    }
    getNextToken(); // eat '='
    
    std::unique_ptr<AST> expression = parseExpression();
    if (expression == nullptr) {
        logError(currentLine, u8"Syntax Error: invalid array instruction expression!");
        return nullptr;
    }
    
    std::unique_ptr<AST> arrReference = std::make_unique<AccessArrayElementAST>(identifier, std::move(index), currentLine);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(arrReference), std::move(expression), currentLine);
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
    if (!isToken(operators::PLUS) && !isToken(operators::MINUS) && !isToken(operators::MULTIPLY) && !isToken(operators::DIVIDE) && !isToken(operators::POWER)) {
        logError(currentLine, u8"Syntax Error: invalid shorthand operator - only plus, minus, multiply, divide and power is allowed!");
        return nullptr;
    }

    auto op = operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);

    getNextToken();
    if (!isToken(TokenType::OPERATOR)){
        logError(currentLine, u8"Syntax Error: expected operator!");
        return nullptr;    
    }

    std::unique_ptr<AST> expression;
    if (op->first == m_currentToken->value && (op->first == operators::PLUS || op->first == operators::MINUS)) {
        // var++ or var--
        getNextToken();
        expression = std::make_unique<NumberAST>(1, currentLine);

        if (!isToken(TokenType::EOF_TOKEN) && !isToken(TokenType::NEW_LINE) && !isToken(TokenType::PUNCTUATION)) {
            logError(currentLine, u8"Syntax Error: shorthand operator cannot interact with other operators!");   
            return nullptr;
        }
    } else if (isToken(TokenType::OPERATOR, operators::ASSIGN)) {

        // var -= I
        getNextToken();
        expression = parseExpression();
    } else {
        logError(currentLine, u8"Syntax Error: invalid shorthand operator!");   
        return nullptr;
    } 

    std::unique_ptr<AST> rhs = std::make_unique<BinaryOperatorAST>(std::u8string(op->first), std::make_unique<VariableReferenceAST>(identifier, currentLine), std::move(expression), currentLine);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::make_unique<VariableReferenceAST>(identifier, currentLine), std::move(rhs), currentLine);
}

/**
 * A Declaration of a struct
 *
 * currentToken is a type "rerum"
 *
 * Examples:
 *    - rerum name (type attribute, ...)
 *      ^ we are always here
 */
std::unique_ptr<AST> Parser::parseInstructionDeclarationStruct() {
    getNextToken(); // eat rerum

    if (!isToken(TokenType::IDENTIFIER)) {
        logError(currentLine, u8"Syntax Error: identifier expected!");
        return nullptr;
    }
    std::u8string identifier = m_currentToken->value;
    getNextToken(); // eat identifier

    if (!isToken(TokenType::OPERATOR, u8"=")) {
        logError(currentLine, u8"Syntax Error: assign operator '=' expected!");
        return nullptr;
    }
    getNextToken(); // eat '=' 

    auto hackyPrototype = parseInstructionPrototype(u8"", nullptr);
    if (hackyPrototype == nullptr) {
        logError(currentLine, u8"Syntax Error: invalid struct declaration! Try: rerum vector = (numerus x, numerus y)");
        return nullptr;
    }

    std::vector<TypeIdentifierPair> attributes;
    attributes.reserve(hackyPrototype->getArgs().size());
    for (const auto& arg : hackyPrototype->getArgs()) {
        attributes.emplace_back(std::move(arg->type), arg->identifier);
    }
    std::unique_ptr<StructDataType> type = std::make_unique<StructDataType>(identifier, std::move(attributes));
    m_structHashMap[identifier] = type.get();

    return std::make_unique<StructAST>(std::move(type), currentLine);
}

/**
 * A Declaration can be just init or init and assign. It can also be a function declaration
 *
 * currentToken is at first token of declaration. It is always TYPE
 *
 * Examples:
 *    - numerus var
 *    - numerus var = I
 *    - numerus[I] var = [I]
 *    - numerus var = λ(...): [Block] ;
 *    - nihil   var = λ(...): [Block] ;
 *      ^ we are always here
 */
std::unique_ptr<AST> Parser::parseInstructionDeclaration() {
    std::unique_ptr<IDataType> dataType;
    std::u8string typeStr = m_currentToken->value;
    if (typeStr == types::STRUCT) {
        return parseInstructionDeclarationStruct();
    }

    if (m_structHashMap.find(m_currentToken->value) != m_structHashMap.end()) {
        return parseInstructionDeclarationStructType();
    }

    auto typeIter = STR_TO_PRIMITIVE_MAP.find(typeStr);
    if (typeIter == STR_TO_PRIMITIVE_MAP.end()) {
        logError(currentLine, u8"Unknown or invalid type!");
        return nullptr;
    }
    PrimitiveType primitiveType = typeIter->second;
    getNextToken(); // eat type

    if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
        // it is array declaration
        if (primitiveType == PrimitiveType::VOID) {
            logError(currentLine, u8"Syntax Error: array declaration cannot be of type nihil!");
            return nullptr;
        }

        auto array = parseInstructionDeclarationArray(primitiveType);
        if (array == nullptr) {
            logError(currentLine, u8"Syntax Error: invalid array declaration!");
            return nullptr;
        }
        return array;
    } else {
        dataType = std::make_unique<PrimitiveDataType>(primitiveType);
    }

    if (!isToken(TokenType::IDENTIFIER)) {
        logError(currentLine, u8"Syntax Error: identifier expected!");
        return nullptr;
    }
    std::u8string identifier = m_currentToken->value;
    getNextToken(); // eat identifier

    if (isToken(TokenType::NEW_LINE)) {
        return std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    }
    
    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        logError(currentLine, u8"Syntax Error: initialization missing! Use assign operator '=' to assign a value!");
        return nullptr;
    }
    getNextToken(); // eat '='

    std::unique_ptr<AST> expression;

    // λ
    if (isToken(TokenType::KEYWORD, keywords::FUNCTION)) { 
        return parseInstructionFunction(identifier, std::move(dataType));
    } 

    expression = parseExpression();
    if (expression == nullptr)  {
        logError(currentLine, u8"Syntax Error: invalid declaration expression!");
        return nullptr;
    }
    
    std::unique_ptr<AST> declaration = std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(declaration), std::move(expression), currentLine);
}

/**
 * create instance of self defined struct type
 * 
 * Examples:
 *      - vector point  = [V, VI]
 *      - test_struct t = ['a', 'b', III, veri]
 *        ^ we are always here
 * 
 */
std::unique_ptr<AST> Parser::parseInstructionDeclarationStructType() {
    auto structIter = m_structHashMap.find(m_currentToken->value);
    std::unique_ptr<IDataType> dataType = std::make_unique<StructDataType>(structIter->first);

    getNextToken();
    if (!isToken(TokenType::IDENTIFIER)) {
        logError(currentLine, u8"Syntax Error: identifier expected!");
        return nullptr;
    }

    std::u8string identifier = m_currentToken->value;

    getNextToken();
    if (isToken(TokenType::NEW_LINE) || isToken(TokenType::EOF_TOKEN)) {
        // declaration without init
        return std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    }

    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        logError(currentLine, u8"Syntax Error: rerum initialization missing! Use assign operator '=' to initialize rerum!");
        return nullptr;
    }

    getNextToken();

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
        logError(currentLine, u8"Syntax Error: opening rerum bracket for initialization '[' expected!");
        return nullptr;
    }

    getNextToken();
    std::vector<std::unique_ptr<AST>> elements;

    // get expression from each index
    while (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) { 
        if (isToken(TokenType::EOF_TOKEN)) {
            logError(currentLine, u8"Syntax Error: closing bracket for rerum initialization ']' expected!");
            return nullptr;
        }

        std::unique_ptr<AST> element = parseExpression();
        if (element == nullptr) {
            logError(currentLine, u8"Syntax Error: invalid expression during rerum initialization!");
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
        logError(currentLine, u8"Syntax Error: expected ',' or ']' during rerum initialization!");
        return nullptr;
    }

    // currentToken is always ]
    getNextToken(); // eat ']'

    std::unique_ptr<AST> expression = std::make_unique<ArrayInitializationAST>(identifier, std::move(elements), currentLine);
    std::unique_ptr<AST> declaration = std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(declaration), std::move(expression), currentLine);
}

/**
 * An array consists of multiple values from type primitive inside bracket
 * 
 * Examples:
 *      - numerus[I]  arr = [I]
 *      - numerus[II] arr = [I, II]
 *      - numerus[II] arr = [I, II, III]
 *      - numerus[I] arr = [foo() + IV]
 *      - litera[II] arr = ['a', 'b']
 *      - numerus[] arr = [foo() + IV]
 *               ^ we are always here
 * 
 * note: numerus[] will automatically get the size of the array by counting the commas in initialization
 */
std::unique_ptr<AST> Parser::parseInstructionDeclarationArray(PrimitiveType type) {
    getNextToken();

    if (!isToken(TokenType::NUMBER) && !isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
        logError(currentLine, u8"Syntax Error: expected static number or closing bracket ']' after '[' in array declaration! Example: numerus[III] = [I, II, III]");
        return nullptr;
    }

    int arrSize = -1;
    if (isToken(TokenType::NUMBER)) {
        bool success = toArabicConverter(m_currentToken->value, &arrSize);
        if (!success) {
            logError(currentLine, u8"Syntax Error: invalid roman number!");
            return nullptr;
        }
        getNextToken();
    }
    
    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
        logError(currentLine, u8"Syntax Error: closing array bracket ']' expected!");
        return nullptr;
    }

    getNextToken();
    if (!isToken(TokenType::IDENTIFIER)) {
        logError(currentLine, u8"Syntax Error: identifier expected!");
        return nullptr;
    }

    std::u8string identifier = m_currentToken->value;

    getNextToken();

    if (isToken(TokenType::NEW_LINE) || isToken(TokenType::EOF_TOKEN)) {
        // only declaration - no init
        if (arrSize < 0) {
            logError(currentLine, u8"Syntax Error: array declaration without initialization must have size!");
            return nullptr;
        }

        auto dataType = std::make_unique<ArrayDataType>(type, arrSize);
        return std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    }


    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        logError(currentLine, u8"Syntax Error: array initialization missing! Use assign operator '=' to initialize array!");
        return nullptr;
    }

    getNextToken(); // eat '='

    // λ
    if (isToken(TokenType::KEYWORD, keywords::FUNCTION)) { 
        if (arrSize < 0) {
            logError(currentLine, u8"Syntax Error: Array size must be defined as positive number!");
            return nullptr;
        }
        auto dataType = std::make_unique<ArrayDataType>(type, arrSize);
        return parseInstructionFunction(identifier, std::move(dataType));
    }

    // Function call
    if(isToken(TokenType::IDENTIFIER)) {
        std::u8string funcCallIdentifier = m_currentToken->value;
        getNextToken(); // eat funcCall identifier

        if(!isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
            logError(currentLine, u8"Syntax Error: Expected '(' after identifier for function call!");
            return nullptr;
        }
        
        std::unique_ptr<AST> expression = parseExpressionFunctionCall(funcCallIdentifier);
        if (expression == nullptr)  {
            logError(currentLine, u8"Syntax Error: invalid declaration expression!");
            return nullptr;
        }

        auto dataType = std::make_unique<ArrayDataType>(type, arrSize);
        std::unique_ptr<AST> declaration = std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
        return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(declaration), std::move(expression), currentLine);
    }

    std::vector<std::unique_ptr<AST>> elements;

    // Literal wrapper
    if (isToken(TokenType::LITERAL) && type == PrimitiveType::CHAR) {
        // 'a' -> ['a', '\0']
        // litera[] = 'abc'
        int actualSize = 0;
        for (char8_t ch : m_currentToken->value) {
            std::unique_ptr<AST> element = std::make_unique<CharAST>(ch, currentLine);
            elements.push_back(std::move(element));
            actualSize++;
        }

        std::unique_ptr<AST> element = std::make_unique<CharAST>(u8'\0', currentLine);
        elements.push_back(std::move(element));

        getNextToken();

        if (arrSize != actualSize && arrSize != -1) {
            std::string arrSizeStr = std::to_string(arrSize);
            std::string actualSizeStr = std::to_string(actualSize);
            std::u8string arrSizeStrU8(arrSizeStr.begin(), arrSizeStr.end());
            std::u8string actualSizeStrU8(actualSizeStr.begin(), actualSizeStr.end());
    
            logError(currentLine, u8"Syntax Error: array size mismatch! Array is declared with size " + arrSizeStrU8 + u8" but initialized with size " + actualSizeStrU8 + u8"!");
            return nullptr;
        }

        auto dataType = std::make_unique<ArrayDataType>(type, elements.size());
        std::unique_ptr<AST> expression = std::make_unique<ArrayInitializationAST>(identifier, std::move(elements), currentLine);
        std::unique_ptr<AST> declaration = std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
        return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(declaration), std::move(expression), currentLine);
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
        logError(currentLine, u8"Syntax Error: opening array bracket for initialization '[' expected!");
        return nullptr;
    }

    getNextToken();
    
    int actualSize = 0;

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

        actualSize++;
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
    
    // currentToken is always ]
    getNextToken(); // eat ']'

    if (arrSize != actualSize && arrSize != -1) {
        std::string arrSizeStr = std::to_string(arrSize);
        std::string actualSizeStr = std::to_string(actualSize);
        std::u8string arrSizeStrU8(arrSizeStr.begin(), arrSizeStr.end());
        std::u8string actualSizeStrU8(actualSizeStr.begin(), actualSizeStr.end());

        logError(currentLine, u8"Syntax Error: array size mismatch! Array is declared with size " + arrSizeStrU8 + u8" but initialized with size " + actualSizeStrU8 + u8"!");
        return nullptr;
    }

    arrSize = elements.size();
    if (arrSize < 0) {
        logError(currentLine, u8"Syntax Error: array size cannot be negative!");
        return nullptr;
    }

    auto dataType = std::make_unique<ArrayDataType>(type, arrSize);
    std::unique_ptr<AST> expression = std::make_unique<ArrayInitializationAST>(identifier, std::move(elements), currentLine);
    std::unique_ptr<AST> declaration = std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    return std::make_unique<BinaryOperatorAST>(std::u8string(operators::ASSIGN), std::move(declaration), std::move(expression), currentLine);
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
std::unique_ptr<FunctionPrototypeAST> Parser::parseInstructionPrototype(const std::u8string& identifier, std::unique_ptr<IDataType> type) {
    getNextToken(); // eat '('
    std::vector<std::unique_ptr<TypeIdentifierPair>> args;
    while (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE) && !isToken(TokenType::EOF_TOKEN)) {
        if (!isToken(TokenType::TYPE)) {
            logError(currentLine, u8"Syntax Error: type expected!");
            return nullptr;
        }

        std::u8string argType = m_currentToken->value;
        auto typeIter = STR_TO_PRIMITIVE_MAP.find(argType);
        assert(typeIter != STR_TO_PRIMITIVE_MAP.end() && "Unkown type");
        PrimitiveType primitiveType = typeIter->second;
        getNextToken(); //  eat type
        
        bool isArray = false;
        int arrSize = -1;
        if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
            getNextToken(); // eat '['
            if (!isToken(TokenType::NUMBER) || !toArabicConverter(m_currentToken->value, &arrSize)) {
                logError(currentLine, u8"Syntax Error: Expected number after '[', when dealing with array func-argument!");
                return nullptr;
            }
            getNextToken(); // eat number
            if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
                logError(currentLine, u8"Expected ']', when dealing with array func-argument!");
                return nullptr;
            }
            getNextToken(); // eat ']'
            isArray = true;
        }

        if (!isToken(TokenType::IDENTIFIER)) {
            logError(currentLine, u8"Syntax Error: identifier expected!");
            return nullptr;
        }

        std::u8string identifier = m_currentToken->value;
        getNextToken(); // eat identifier
        
        std::unique_ptr<IDataType> dataType;
        if (isArray)
            dataType = std::make_unique<ArrayDataType>(primitiveType, arrSize);
        else
            dataType = std::make_unique<PrimitiveDataType>(primitiveType);
        
        args.emplace_back(std::make_unique<TypeIdentifierPair>(std::move(dataType), identifier));

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE) || isToken(TokenType::EOF_TOKEN)) {
                logError(currentLine, u8"Syntax Error: missing expression after comma!");   
                return nullptr;
            }

            continue;
        }
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
            break;
        }


        logError(currentLine, u8"Syntax Error: expected ')' in function declaration!");
        return nullptr;
    }

    if(!isToken(TokenType::PUNCTUATION, u8")")) {
        logError(currentLine, u8"Syntax Error: expected ')' in function declaration!");
        return nullptr;
    }
    getNextToken(); // eat ')'


    while(isToken(TokenType::NEW_LINE)) {
        currentLine++;
        getNextToken();
    }

    bool isDefined = isToken(TokenType::PUNCTUATION, punctuation::BLOCK_OPEN);
    return std::make_unique<FunctionPrototypeAST>(identifier, std::move(type), std::move(args), isDefined, currentLine);
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
std::unique_ptr<AST> Parser::parseInstructionFunction(const std::u8string& identifier, std::unique_ptr<IDataType> type) {
    if (m_blockCount != 0) {
        logError(currentLine, u8"Syntax Error: Function Declaration is only allowed at top-level!");
        return nullptr;
    }

    getNextToken(); // eat λ
    if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
        logError(currentLine, u8"Syntax Error: opening bracket '(' expected!");
        return nullptr;
    }

    auto prototype = parseInstructionPrototype(identifier, std::move(type));
    if (prototype == nullptr) {
            logError(currentLine, u8"Syntax Error: invalid function header!");
            return nullptr;
        }

    if (!prototype->isDefined())
        return prototype; // it's a extern defined function

    auto funcBlock = parseBlock();
    if (funcBlock == nullptr){
        logError(currentLine, u8"Syntax Error: invalid function block!");
        return nullptr;
    }

    return std::make_unique<FunctionAST>(std::move(prototype), std::move(funcBlock), currentLine);
}
