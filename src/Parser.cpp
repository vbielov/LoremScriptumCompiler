#include "Parser.hpp"

Parser::Parser(Lexer& lexer) : m_lexer(&lexer), m_currentToken(lexer.getNextToken()) {}

void Parser::parse() {
    while (true) {
        switch (m_currentToken.type) {
            case TokenType::EOF_TOKEN: 
                return;
            case TokenType::KEYWORD: {
                if(m_currentToken.value == u8"λ") {
                    if (!parseDefinition()) {
                        m_currentToken = m_lexer->getNextToken();
                    }
                    break;
                }
            }
            default: {
                if (!parseTopLevelExpr()) {
                    m_currentToken = m_lexer->getNextToken(); // skip token for error recovery
                }
                break;
            }
        }
    }
}

int Parser::getTokenPrecedence() const {    
    int precendence = -1;
    auto precedenceIt = BINARY_OPERATION_PRECEDENCE.find(m_currentToken.value);
    if (precedenceIt != BINARY_OPERATION_PRECEDENCE.end()) {
        precendence = precedenceIt->second;
    }
    return precendence;
}

std::unique_ptr<AST> Parser::sieveToken(const Token& token) const {
    return nullptr;
}

std::unique_ptr<AST> Parser::parseExpression() {
    auto LHS = parsePrimary();
    if (!LHS) {
        return nullptr;
    }
    return parseBinOpRHS(0, std::move(LHS));
}

std::unique_ptr<AST> Parser::parseNumberExpr() {
    int arabicNumber = toArabicConverter(m_currentToken.value);
    m_currentToken = m_lexer->getNextToken(); // eat number
    return std::make_unique<NumberAST>(arabicNumber);
}

std::unique_ptr<AST> Parser::parseParenExpr() {
    m_currentToken = m_lexer->getNextToken(); // eat (
    std::unique_ptr<AST> body = parseExpression();
    if (!body) {
        return nullptr;
    }
    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8")") {
        std::cerr << "Error: expected ')'\n";
        return nullptr;
    }
    m_currentToken = m_lexer->getNextToken(); // eat )
    return body;
}

std::unique_ptr<AST> Parser::parseIdentifierExpr() {
    std::u8string identifier = m_currentToken.value;
    m_currentToken = m_lexer->getNextToken(); // eat identifier
    
    // variable reference
    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8"(") {
        return std::make_unique<VariableAST>(identifier);
    }

    m_currentToken = m_lexer->getNextToken(); // eat ( 
    std::vector<std::unique_ptr<AST>> args;
    if (m_currentToken.type != TokenType::PUNCTUATION && m_currentToken.value != u8")") {
        while (true) {
            if (auto arg = parseExpression()) {
                args.push_back(std::move(arg));
            }
            if (m_currentToken.type == TokenType::PUNCTUATION && m_currentToken.value == u8")") {
                break;
            }
            if (m_currentToken.type != TokenType::PUNCTUATION && m_currentToken.value != u8",") {
                std::cerr << "Error: Expected ')' or ',' in argument list\n";
                return nullptr;
            }
            m_currentToken = m_lexer->getNextToken(); // eat ,
        }
    }
    m_currentToken = m_lexer->getNextToken(); // eat )
    return std::make_unique<FuncCallAST>(identifier, std::move(args));
}

std::unique_ptr<AST> Parser::parsePrimary() {
    switch (m_currentToken.type) {
        case TokenType::IDENTIFIER:
            return parseIdentifierExpr();
        case TokenType::NUMBER:
            return parseNumberExpr();
        case TokenType::PUNCTUATION: {
            if (m_currentToken.value == u8"(") {
                return parseParenExpr();
            }
            std::cerr << "Error: Idk bro\n";
            return nullptr;
        }
        default: {
            std::cerr << "Error: Unkown token when expecting an expression\n";
            return nullptr;
        }
    }
    return nullptr;
}

std::unique_ptr<AST> Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<AST> LHS) {
    while (true) {
        // if this is a binary operation, find it's precedence
        int prec = getTokenPrecedence();
        
        // if new token has weaker precendence => we are done.
        if (prec < exprPrec) {
            return LHS;
        }

        // now it's 100% binary operation, and not a single value
        std::u8string op = m_currentToken.value;
        m_currentToken = m_lexer->getNextToken(); // eat binary operation

        // parse right side after operator
        std::unique_ptr<AST> RHS = parsePrimary();
        if (!RHS) {
            return nullptr;
        }

        // if next operator has higher precendence, let's change order (recursively?)
        int nextPrec = getTokenPrecedence();
        if (prec < nextPrec) {
            RHS = parseBinOpRHS(prec + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }

        // Merge LHS/RHS.
        LHS = std::make_unique<BinaryOperatorAST>(op, std::move(LHS), std::move(RHS));
    }

}

std::unique_ptr<FuncDeclarationAST> Parser::parseDeclaration() {

    if (m_currentToken.type != TokenType::IDENTIFIER) {
        std::cerr << "Error: Expected function name in function declaration\n";
        return nullptr;
    }

    std::u8string funcName = m_currentToken.value;
    m_currentToken = m_lexer->getNextToken(); // eat identifier

    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8"(") {
        std::cerr << "Error: Expected '(' in function declaration\n";
        return nullptr;
    }

    std::vector<std::u8string> argNames;
    while ((m_currentToken = m_lexer->getNextToken()).type == TokenType::IDENTIFIER) {
        argNames.push_back(m_currentToken.value);
    }

    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8")") {
        std::cerr << "Error: Expected ')' in function declaration\n";
        return nullptr;
    }

    m_currentToken = m_lexer->getNextToken(); // eat )
    return std::make_unique<FuncDeclarationAST>(funcName, std::move(argNames));
}

std::unique_ptr<FuncDefinitionAST> Parser::parseDefinition() {
    m_currentToken = m_lexer->getNextToken(); // eat λ

    auto declaration = parseDeclaration();
    if (!declaration) {
        return nullptr;
    }

    auto body = parseExpression();
    if (body) {
        return std::make_unique<FuncDefinitionAST>(std::move(declaration), std::move(body));
    }
    return nullptr;
}

std::unique_ptr<FuncDefinitionAST> Parser::parseTopLevelExpr() {
    if (auto expr = parseExpression()) {
        auto declaration = std::make_unique<FuncDeclarationAST>(u8"", std::vector<std::u8string>());
        return std::make_unique<FuncDefinitionAST>(std::move(declaration), std::move(expr));
    }
    return nullptr;
}
