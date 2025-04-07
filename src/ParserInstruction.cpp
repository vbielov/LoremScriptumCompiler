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
                ErrorHandler::logError(currentLine, u8"Syntax Error: Invalid declaration!");
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
    ErrorHandler::logError(currentLine, u8"Syntax Error: expected identifier!");
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
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid assignment expression!");
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
        ErrorHandler::logError(currentLine, u8"Syntax Error: expected index for indexing array!");
        return nullptr;
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: expected ']' after array indexing!");
        return nullptr;
    }
    getNextToken(); // eat ']'

    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: assign operator '=' expected!");
        return nullptr;
    }
    getNextToken(); // eat '='
    
    std::unique_ptr<AST> expression = parseExpression();
    if (expression == nullptr) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid array instruction expression!");
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
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid shorthand operator - only plus, minus, multiply, divide and power is allowed!");
        return nullptr;
    }

    auto op = operators::BINARY_OPERATION_PRIORITY_MAP.find(m_currentToken->value);

    getNextToken();
    if (!isToken(TokenType::OPERATOR)){
        ErrorHandler::logError(currentLine, u8"Syntax Error: expected operator!");
        return nullptr;    
    }

    std::unique_ptr<AST> expression;
    if (op->first == m_currentToken->value && (op->first == operators::PLUS || op->first == operators::MINUS)) {
        // var++ or var--
        getNextToken();
        expression = std::make_unique<NumberAST>(1, currentLine);

        if (!isToken(TokenType::EOF_TOKEN) && !isToken(TokenType::NEW_LINE) && !isToken(TokenType::PUNCTUATION)) {
            ErrorHandler::logError(currentLine, u8"Syntax Error: shorthand operator cannot interact with other operators!");   
            return nullptr;
        }
    } else if (isToken(TokenType::OPERATOR, operators::ASSIGN)) {

        // var -= I
        getNextToken();
        expression = parseExpression();
    } else {
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid shorthand operator!");   
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
        ErrorHandler::logError(currentLine, u8"Syntax Error: identifier expected!");
        return nullptr;
    }
    std::u8string identifier = m_currentToken->value;
    getNextToken(); // eat identifier

    if (!isToken(TokenType::OPERATOR, u8"=")) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: assign operator '=' expected!");
        return nullptr;
    }
    getNextToken(); // eat '=' 

    auto hackyPrototype = parseInstructionPrototype(u8"", nullptr);
    if (hackyPrototype == nullptr) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid struct declaration! Try: rerum vector = (numerus x, numerus y)");
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

    // NOTE(Vlad): Automatically generating constructor for struct is impossible.
    //             Because there can be only one owner for memory of attributes.
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
    if (m_currentToken->value == types::STRUCT) {
        return parseInstructionDeclarationStruct();
    }
    
    std::unique_ptr<IDataType> dataType = parseType();
    if(!dataType)
        return nullptr;

    if (!isToken(TokenType::IDENTIFIER)) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: identifier expected!");
        return nullptr;
    }
    std::u8string identifier = m_currentToken->value;
    getNextToken(); // eat identifier

    if (isToken(TokenType::NEW_LINE) || isToken(TokenType::EOF_TOKEN)) {
        return std::make_unique<VariableDeclarationAST>(identifier, std::move(dataType), currentLine);
    }
    
    if (!isToken(TokenType::OPERATOR, operators::ASSIGN)) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: initialization missing! Use assign operator '=' to assign a value!");
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
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid declaration expression!");
        return nullptr;
    }
    
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
            ErrorHandler::logError(currentLine, u8"Syntax Error: type expected!");
            return nullptr;
        }

        std::unique_ptr<IDataType> dataType = parseType();

        if (!isToken(TokenType::IDENTIFIER)) {
            ErrorHandler::logError(currentLine, u8"Syntax Error: identifier expected!");
            return nullptr;
        }

        std::u8string identifier = m_currentToken->value;
        getNextToken(); // eat identifier
        
        args.emplace_back(std::make_unique<TypeIdentifierPair>(std::move(dataType), identifier));

        if (isToken(TokenType::PUNCTUATION, punctuation::COMMA)) {
            getNextToken();
            if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE) || isToken(TokenType::EOF_TOKEN)) {
                ErrorHandler::logError(currentLine, u8"Syntax Error: missing expression after comma!");   
                return nullptr;
            }

            continue;
        }
        if (isToken(TokenType::PUNCTUATION, punctuation::PAREN_CLOSE)) {
            break;
        }

        ErrorHandler::logError(currentLine, u8"Syntax Error: expected ')' in function declaration!");
        return nullptr;
    }

    if(!isToken(TokenType::PUNCTUATION, u8")")) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: expected ')' in function declaration!");
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
        ErrorHandler::logError(currentLine, u8"Syntax Error: Function Declaration is only allowed at top-level!");
        return nullptr;
    }

    getNextToken(); // eat λ
    if (!isToken(TokenType::PUNCTUATION, punctuation::PAREN_OPEN)) {
        ErrorHandler::logError(currentLine, u8"Syntax Error: opening bracket '(' expected!");
        return nullptr;
    }

    auto prototype = parseInstructionPrototype(identifier, std::move(type));
    if (prototype == nullptr) {
            ErrorHandler::logError(currentLine, u8"Syntax Error: invalid function header!");
            return nullptr;
        }

    if (!prototype->isDefined())
        return prototype; // it's a extern defined function

    auto funcBlock = parseBlock();
    if (funcBlock == nullptr){
        ErrorHandler::logError(currentLine, u8"Syntax Error: invalid function block!");
        return nullptr;
    }

    return std::make_unique<FunctionAST>(std::move(prototype), std::move(funcBlock), currentLine);
}
