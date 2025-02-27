#include "Parser.hpp"

/**
 * An Instruction changes data. It can be a declaration or expression
 *
 * currentToken is at first token of instruction. It is always TYPE or IDENTIFIER
 *
 * Examples:
 *    - numerus var = I
 *    - var = var + I
 *    - func(...)
 */
std::unique_ptr<AST> Parser::parseInstruction() {
    if (isToken(TokenType::TYPE)) return parseInstructionDeclaration();
    return parseExpression();
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
    if (isToken(TokenType::KEYWORD, u8"λ")) return parseInstructionDeclarationFunction(type, identifier);

    auto declaration = std::make_unique<VariableDeclarationAST>(type, identifier);
    auto expression = parseExpression();
    if (expression == nullptr) return nullptr;

    return std::make_unique<BinaryOperatorAST>(u8"=", std::move(declaration), std::move(expression));
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
std::unique_ptr<FuncCallAST> Parser::parseInstructionFunctionCall(std::u8string identifier) {
    getNextToken();

    std::vector<std::unique_ptr<AST>> args;
    while (!isToken(TokenType::PUNCTUATION, u8")")) {
        auto expression = parseExpression();
        if (expression == nullptr) return nullptr;

        args.push_back(std::move(expression));

        if (isToken(TokenType::PUNCTUATION, u8",")) {
            getNextToken();
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, u8")")) {
            getNextToken();
            break;
        }

        printError("Error: Expected ) in function call");
        return nullptr;
    }

    return std::make_unique<FuncCallAST>(identifier, std::move(args));
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
 *
 *
 *  std::u8string m_returnType;
    std::u8string m_name;
    std::vector<std::unique_ptr<VariableDeclarationAST>> m_args;
    std::unique_ptr<BlockAST> m_body;
 */
std::unique_ptr<FunctionAST> Parser::parseInstructionDeclarationFunction(std::u8string type, std::u8string identifier) {
    getNextToken();
    if (!isToken(TokenType::PUNCTUATION, u8"(")) return nullptr;

    getNextToken();
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
            continue;
        }
        if (isToken(TokenType::PUNCTUATION, u8")")) {
            break;
        }

        printError("Error: Expected ) in function declaration");
        return nullptr;
    }

    getNextToken();
    if (!isToken(TokenType::PUNCTUATION, u8":")) return nullptr;
    auto funcBlock = parseBlock();
    if (!isToken(TokenType::PUNCTUATION, u8";")) return nullptr;

    return std::make_unique<FunctionAST>(type, identifier, std::move(args), std::move(funcBlock));
}
