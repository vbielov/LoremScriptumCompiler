#include "Parser.hpp"

static std::unordered_map<std::u8string, StructDataType*> s_structHashMap;

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

    auto op = operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);

    getNextToken();
    if (!isToken(TokenType::OPERATOR)) return nullptr;

    std::unique_ptr<AST> expression;
    if (op->first == m_currentToken->value && (op->first == operators::PLUS || op->first == operators::MINUS)) {
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
 * A Declaration of a struct
 *
 * currentToken is a type "rerum"
 *
 * Examples:
 *    - rerum name (type attribute, ...)
 *      ^ we are always here
 */
std::unique_ptr<AST> Parser::parseStructDeclaration() {
    getNextToken(); // eat rerum

    if (!isToken(TokenType::IDENTIFIER)) 
        return nullptr;
    std::u8string identifier = m_currentToken->value;
    getNextToken(); // eat identifier

    if (!isToken(TokenType::OPERATOR, u8"=")) 
        return nullptr;
    getNextToken(); // eat '=' 

    auto hackyPrototype = parseInstructionPrototype(u8"", nullptr);
    std::vector<TypeIdentifierPair> attributes;
    attributes.reserve(hackyPrototype->getArgs().size());
    for (const auto& arg : hackyPrototype->getArgs()) {
        attributes.emplace_back(std::move(arg->type), arg->identifier);
    }
    std::unique_ptr<StructDataType> type = std::make_unique<StructDataType>(identifier, std::move(attributes));
    s_structHashMap[identifier] = type.get();

    return std::make_unique<StructAST>(std::move(type));
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
 *      ^ we are always here
 */
std::unique_ptr<AST> Parser::parseInstructionDeclaration() {
    std::unique_ptr<IDataType> dataType;
    std::u8string typeStr = m_currentToken->value;
    if (typeStr == types::STRUCT) {
        return parseStructDeclaration();
    } else {
        auto structIter = s_structHashMap.find(typeStr);
        if (structIter != s_structHashMap.end()) {
            dataType = std::make_unique<StructDataType>(structIter->first);
        } else {
            auto typeIter = STR_TO_PRIMITIVE_MAP.find(typeStr);
            assert(typeIter != STR_TO_PRIMITIVE_MAP.end() && "Unknown type");
            PrimitiveType primitiveType = typeIter->second;
            getNextToken(); // eat type
        
            if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
                // it's array declaration
                // TODO(Vlad): maybe thats can be a more then a number?
                getNextToken(); // eat '['
                if (!isToken(TokenType::NUMBER)) {
                    printError("Expected number after [ for array declaration");
                    return nullptr; 
                }
                int arrSize = 0;
                toArabicConverter(m_currentToken->value, &arrSize);
                getNextToken(); // eat number
                if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
                    printError("Expected ']' in array declaration");
                    return nullptr;
                }
                getNextToken(); // eat ']'
                dataType = std::make_unique<ArrayDataType>(primitiveType, arrSize);
            } else {
                dataType = std::make_unique<PrimitiveDataType>(primitiveType);
            }
        }
    }

    if (!isToken(TokenType::IDENTIFIER)) 
        return nullptr;
    std::u8string identifier = m_currentToken->value;
    getNextToken(); // eat identifier

    if (isToken(TokenType::NEW_LINE)) {
        return std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType));
    }
    
    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) 
        return nullptr;
    getNextToken(); // eat '='

    std::unique_ptr<AST> expression;

    // λ
    if (isToken(TokenType::KEYWORD, keywords::FUNCTION)) { 
        return parseInstructionFunction(identifier, std::move(dataType));
    } 
    // Array initialization
    else if (isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) { 
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
        expression = std::make_unique<ArrayInitializationAST>(identifier, std::move(elements));
    }
    // Expression
    else {
        expression = parseExpression();
    }
    
    if (expression == nullptr) 
        return nullptr;
    
    std::unique_ptr<AST> declaration = std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType));
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
std::unique_ptr<FunctionPrototypeAST> Parser::parseInstructionPrototype(const std::u8string& identifier, std::unique_ptr<IDataType> type) {
    getNextToken(); // eat '('
    std::vector<std::unique_ptr<TypeIdentifierPair>> args;
    while (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE) && !isToken(TokenType::EOF_TOKEN)) {
        if (!isToken(TokenType::TYPE)) 
            return nullptr;

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
                printError("Expected number after '[', when dealing with array func-argument");
                return nullptr;
            }
            getNextToken(); // eat number
            if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
                printError("Expected ']', when dealing with array func-argument");
                return nullptr;
            }
            getNextToken(); // eat ']'
            isArray = true;
        }

        if (!isToken(TokenType::IDENTIFIER)) 
            return nullptr;

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
    getNextToken(); // eat ')'

    while(isToken(TokenType::NEW_LINE)) {
        getNextToken();
    }

    bool isDefined = isToken(TokenType::PUNCTUATION, punctuation::BLOCK_OPEN);
    return std::make_unique<FunctionPrototypeAST>(identifier, std::move(type), std::move(args), isDefined);
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
        printError("Function Declaration is only allowed at top-level");
        return nullptr;
    }

    getNextToken(); // eat λ
    if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) return nullptr;

    auto prototype = parseInstructionPrototype(identifier, std::move(type));
    if (prototype == nullptr) 
        return nullptr;

    if (!prototype->isDefined())
        return prototype; // it's a extern defined function

    auto funcBlock = parseBlock();
    if (funcBlock == nullptr) return nullptr;

    return std::make_unique<FunctionAST>(std::move(prototype), std::move(funcBlock));
}
