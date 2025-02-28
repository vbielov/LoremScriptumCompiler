#include "Parser.hpp"

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
std::unique_ptr<AST> Parser::parseDeclaration() {
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
    if (isToken(TokenType::KEYWORD, u8"λ")) return parseDeclarationFunction(type, identifier);

    auto declaration = std::make_unique<VariableDeclarationAST>(type, identifier);
    auto expression = parseExpression();
    if (expression == nullptr) return nullptr;

    return std::make_unique<BinaryOperatorAST>(u8"=", std::move(declaration), std::move(expression));
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
std::unique_ptr<FunctionAST> Parser::parseDeclarationFunction(std::u8string& type, std::u8string& identifier) {
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
