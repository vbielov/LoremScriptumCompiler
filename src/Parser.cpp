#include "Parser.hpp"

Parser::Parser(Lexer& lexer) 
    : m_lexer(&lexer) {}

std::unique_ptr<BlockAST> Parser::parseProgram() {
    return parseBlock();
}

// this function eat ':' that opens block
std::unique_ptr<BlockAST> Parser::parseBlock() {
    getNextToken();
    std::vector<std::unique_ptr<AST>> instructions;

    // while it's not the end of current scope and not eof, search for next instr
    while(m_currentToken.value != u8";" && m_currentToken.type != TokenType::EOF_TOKEN) {
        // Empty line
        if (m_currentToken.type == TokenType::NEW_LINE || m_currentToken.type == TokenType::COMMENT) {
            getNextToken(); // eat comment or empty line
            continue;
        }
        switch (m_currentToken.type) {
            case TokenType::TYPE: {
                auto instr = parseType();
                if (instr) instructions.push_back(std::move(instr));
                break;
            }
            case TokenType::IDENTIFIER: {
                auto instr = parseExpression(); // because parseIdentifier() doesn't capture binary operation that can follow after identifier
                if (instr) instructions.push_back(std::move(instr));
                break;
            }
            case TokenType::KEYWORD: {
                auto instr = parseKeyword();
                if (instr) instructions.push_back(std::move(instr));
                break;
            }
            default:
                std::cerr   << RED << "Error: Unknown token: " << TOKEN_TYPE_LABELS[(int)(m_currentToken.type)] 
                            << " " << (const char*)(m_currentToken.value.c_str()) << RESET << std::endl;
                getNextToken();
                break;
        }
    }
    getNextToken(); // eat last token that is ';' or EOF

    return std::make_unique<BlockAST>(std::move(instructions));
}

int Parser::getTokenPrecedence() const {    
    int precendence = -1;
    auto precedenceIt = BINARY_OPERATION_PRECEDENCE.find(m_currentToken.value);
    if (precedenceIt != BINARY_OPERATION_PRECEDENCE.end()) {
        precendence = precedenceIt->second;
    }
    return precendence;
}

Token& Parser::getNextToken() {
    return m_currentToken = m_lexer->getNextToken(); 
}

std::unique_ptr<AST> Parser::parseType() {
    std::u8string type = m_currentToken.value;
    getNextToken(); // eat type

    if (m_currentToken.type != TokenType::IDENTIFIER) {
        std::cerr << RED << "Error: Expecting identifier after type" << RESET << std::endl;
        return nullptr;
    }
    std::u8string identifier = m_currentToken.value;
    getNextToken(); // eat identifier

    if (m_currentToken.type != TokenType::OPERATOR || m_currentToken.value != u8"=") {
        // it's just a variable declaration
        return std::make_unique<VariableDeclarationAST>(type, identifier);
    }
    getNextToken(); // eat =

    if (m_currentToken.type == TokenType::KEYWORD) {
        if (m_currentToken.value == u8"λ") {
            return parseFunction(type, identifier);
        }
        if (m_currentToken.value == u8"apere") {
            getNextToken(); // eat apere
            return parsePrototype(type, identifier);
        }
    }

    auto expression = parseExpression();
    if (expression) {
        std::u8string op = u8"=";
        auto left = std::make_unique<VariableDeclarationAST>(type, identifier);
        return std::make_unique<BinaryOperatorAST>(op, std::move(left), std::move(expression));
    }

    std::cerr << RED << "Error: Expected expression or function after assign" << RESET << std::endl;
    return nullptr;
}

std::unique_ptr<AST> Parser::parseIdentifier() {
    std::u8string identifier = m_currentToken.value;
    getNextToken(); // eat identifier

    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8"(") {
        // just a variable reference
        return std::make_unique<VariableReferenceAST>(identifier);   
    }

    // that's a function call
    getNextToken(); // eat (

    std::vector<std::unique_ptr<AST>> arguments;
    while (true) {
        auto expr = parseExpression();
        if (expr) {
            arguments.push_back(std::move(expr));
        }
        if (m_currentToken.type == TokenType::PUNCTUATION) {
            if (m_currentToken.value == u8",") {
                getNextToken(); // eat ,
                continue;
            }
            if (m_currentToken.value == u8")") {
                getNextToken(); // eat )
                break;
            }
        }
        std::cerr << RED << "Error: Expected ) in function call" << RESET << std::endl;
        return nullptr;
    }

    return std::make_unique<FuncCallAST>(identifier, std::move(arguments));
}

std::unique_ptr<AST> Parser::parseKeyword() {
    if (m_currentToken.value == u8"∑") {
        getNextToken(); // eat ∑
        return parseFor();
    } else if (m_currentToken.value == u8"si") {
        getNextToken(); // eat "si"
        return parseIf();
    } else if (m_currentToken.value == u8"retro") {
        getNextToken(); // eat "retro"
        return std::make_unique<ReturnAST>(std::move(parseExpression()));
    }

    // u8"retro", 
    // u8"finio", 

    std::cerr << RED << "Error: Wrong keyword" << RESET << std::endl;
    return nullptr;
}

std::unique_ptr<AST> Parser::parseExpression() {
    std::unique_ptr<AST> lhs;
    if (m_currentToken.type == TokenType::NUMBER) {
        int number = 0;
        
        if(!toArabicConverter(m_currentToken.value, &number)){
            std::cout << "Illegal roman numeral!" <<std::endl; 
            getNextToken();
            return nullptr;
        }
        getNextToken(); // eat number
        lhs = std::make_unique<NumberAST>(number);
    }

    if (m_currentToken.type == TokenType::LITERAL) {
        char8_t character = m_currentToken.value[0];
        getNextToken(); // eat char
        lhs = std::make_unique<CharAST>(character);
    }

    if (m_currentToken.type == TokenType::IDENTIFIER) {
        lhs = parseIdentifier();
    }

    // that's a paren expression
    if (m_currentToken.type == TokenType::PUNCTUATION && m_currentToken.value == u8"(") {
        getNextToken(); // eat (
        auto innerExpr = parseExpression();
        if (!innerExpr) {
            return nullptr;
        }
        if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8")") {
            std::cerr << RED << "Error: expected ')' after paren expression" << RESET << std::endl;
            return nullptr;
        }
        getNextToken(); // eat )
        return innerExpr;
    }

    if (!lhs) {
        return nullptr;
    }
    return parseBinOpRHS(0, std::move(lhs));
}

std::unique_ptr<AST> Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<AST> lhs) {
    while (true) {
        int tokPrec = getTokenPrecedence();
        if (tokPrec < exprPrec) {
            // this is not a binop or we are done
            return lhs;
        }

        // This is 100% binop
        std::u8string binOp = m_currentToken.value;
        getNextToken(); // eat binop

        auto rhs = parseExpression();
        if (!rhs) {
            return nullptr;
        }

        int nextPrec = getTokenPrecedence();
        if (tokPrec < nextPrec) {
            // take rhs as its lhs
            rhs = parseBinOpRHS(tokPrec + 1, std::move(rhs));
            if (!rhs) {
                return nullptr;
            }
        }

        // merge lhs/rhs
        lhs = std::make_unique<BinaryOperatorAST>(binOp, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<FunctionPrototypeAST> Parser::parsePrototype(const std::u8string& returnType, const std::u8string& funcName)
{
    getNextToken(); // eat (

    std::vector<std::unique_ptr<VariableDeclarationAST>> arguments;
    while (true) {
        if (m_currentToken.type == TokenType::TYPE) {
            std::u8string type = m_currentToken.value;
            getNextToken(); // eat type

            if (m_currentToken.type != TokenType::IDENTIFIER) {
                std::cerr << RED << "Error: Expecting identifier after type" << RESET << std::endl;
                return nullptr;
            }
            std::u8string identifier = m_currentToken.value;
            getNextToken(); // eat identifier

            arguments.emplace_back(std::make_unique<VariableDeclarationAST>(type, identifier));
        }

        if (m_currentToken.type == TokenType::PUNCTUATION) {
            if (m_currentToken.value == u8",") {
                getNextToken(); // eat ,
                continue;
            }
            if (m_currentToken.value == u8")") {
                getNextToken(); // eat )
                break;
            }
        }

        std::cerr << RED << "Error: Expected ')' in function declaration" << RESET << std::endl;
        return nullptr;
    }
    return std::make_unique<FunctionPrototypeAST>(returnType, funcName, std::move(arguments));
}

std::unique_ptr<FunctionAST> Parser::parseFunction(const std::u8string& returnType, const std::u8string& funcName) {
    getNextToken(); // eat λ
    
    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8"(") {
        std::cerr << RED << "Error: Expected '(' in function declaration" << RESET << std::endl;
        return nullptr;
    }
    
    auto prototype = parsePrototype(returnType, funcName);

    if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8":") {
        std::cerr << RED << "Error: Expected ':' after function declaration" << RESET << std::endl;
        return nullptr;
    }
    // Don't eat ':' before parsing Block
    auto block = parseBlock();
    if (block) {
        return std::make_unique<FunctionAST>(std::move(prototype), std::move(block));
    }
    return nullptr;
}

std::unique_ptr<IfAST> Parser::parseIf() {
    return nullptr;
}

std::unique_ptr<ForAST> Parser::parseFor() {
    return nullptr;
}
