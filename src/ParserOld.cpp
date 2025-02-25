// #include "Parser.hpp"

// Parser::Parser(Lexer& lexer) : m_lexer(&lexer), m_currentToken(lexer.getNextToken()) {}

// void Parser::parse() {
//     std::vector<std::unique_ptr<AST>> ASTTrees;
//     while (true) {
//         switch (m_currentToken.type) {
//             case TokenType::EOF_TOKEN: 
//                 return;
//             default:
//                 getNextToken();
//                 break;
//         }
//     }
//     for (const auto& rootNode : ASTTrees) {
//         rootNode->printTree();
//     }
// }

// int Parser::getTokenPrecedence() const {    
//     int precendence = -1;
//     auto precedenceIt = BINARY_OPERATION_PRECEDENCE.find(m_currentToken.value);
//     if (precedenceIt != BINARY_OPERATION_PRECEDENCE.end()) {
//         precendence = precedenceIt->second;
//     }
//     return precendence;
// }

// Token& Parser::getNextToken() {
//     return m_currentToken = m_lexer->getNextToken(); 
// }

// std::unique_ptr<AST> Parser::parseExpression() {
//     auto LHS = parsePrimary();
//     if (!LHS) {
//         return nullptr;
//     }
//     return parseBinOpRHS(0, std::move(LHS));
// }

// std::unique_ptr<AST> Parser::parseNumberExpr() {
//     int arabicNumber = toArabicConverter(m_currentToken.value);
//     getNextToken(); // eat number
//     return std::make_unique<NumberAST>(arabicNumber);
// }

// std::unique_ptr<AST> Parser::parseParenExpr() {
//     getNextToken(); // eat (
//     std::unique_ptr<AST> body = parseExpression();
//     if (!body) {
//         return nullptr;
//     }
//     if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8")") {
//         std::cerr << "Error: expected ')'\n";
//         return nullptr;
//     }
//     getNextToken(); // eat )
//     return body;
// }

// std::unique_ptr<AST> Parser::parseIdentifierExpr() {
//     std::u8string identifier = m_currentToken.value;
//     getNextToken(); // eat identifier
    
//     // variable reference
//     if (m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8"(") {
//         return std::make_unique<VariableReferenceAST>(identifier);
//     }

//     getNextToken(); // eat ( 
//     std::vector<std::unique_ptr<AST>> args;
//     if (m_currentToken.type != TokenType::PUNCTUATION && m_currentToken.value != u8")") {
//         while (true) {
//             if (auto arg = parseExpression()) {
//                 args.push_back(std::move(arg));
//             }
//             if (m_currentToken.type == TokenType::PUNCTUATION && m_currentToken.value == u8")") {
//                 break;
//             }
//             if (m_currentToken.type != TokenType::PUNCTUATION && m_currentToken.value != u8",") {
//                 std::cerr << "Error: Expected ')' or ',' in argument list\n";
//                 return nullptr;
//             }
//             getNextToken(); // eat ,
//         }
//     }
//     getNextToken(); // eat )
//     return std::make_unique<FuncCallAST>(identifier, std::move(args));
// }

// std::unique_ptr<AST> Parser::parsePrimary() {
//     switch (m_currentToken.type) {
//         case TokenType::IDENTIFIER:
//             return parseIdentifierExpr();
//         case TokenType::NUMBER:
//             return parseNumberExpr();
//         case TokenType::PUNCTUATION: {
//             if (m_currentToken.value == u8"(") {
//                 return parseParenExpr();
//             }
//             std::cerr << "Error: Idk bro\n";
//             return nullptr;
//         }
//         default: {
//             std::cerr << "Error: Unkown token when expecting an expression\n";
//             return nullptr;
//         }
//     }
//     return nullptr;
// }

// std::unique_ptr<AST> Parser::parseBinOpRHS(int exprPrec, std::unique_ptr<AST> LHS) {
//     while (true) {
//         // if this is a binary operation, find it's precedence
//         int prec = getTokenPrecedence();
        
//         // if new token has weaker precendence => we are done.
//         if (prec < exprPrec) {
//             return LHS;
//         }

//         // now it's 100% binary operation, and not a single value
//         std::u8string op = m_currentToken.value;
//         getNextToken(); // eat binary operation

//         // parse right side after operator
//         std::unique_ptr<AST> RHS = parsePrimary();
//         if (!RHS) {
//             return nullptr;
//         }

//         // if next operator has higher precendence, let's change order (recursively?)
//         int nextPrec = getTokenPrecedence();
//         if (prec < nextPrec) {
//             RHS = parseBinOpRHS(prec + 1, std::move(RHS));
//             if (!RHS) {
//                 return nullptr;
//             }
//         }

//         // Merge LHS/RHS.
//         LHS = std::make_unique<BinaryOperatorAST>(op, std::move(LHS), std::move(RHS));
//     }
// }

// std::unique_ptr<AST> Parser::parseAssigment() {
//     std::u8string type = m_currentToken.value;
//     getNextToken(); // eat type

//     if(m_currentToken.type != TokenType::IDENTIFIER) {
//         std::cerr << "Error: Expected identifier after type\n";
//         return nullptr;
//     }
//     std::u8string identifier = m_currentToken.value;
//     getNextToken(); // eat identifier

//     if(m_currentToken.type != TokenType::OPERATOR || m_currentToken.value != u8"=") {
//         // it's just a variable declaration
//         return std::make_unique<VariableDeclarationAST>(type, identifier);
//     }
//     getNextToken(); // eat '='

//     if(m_currentToken.type != TokenType::KEYWORD || m_currentToken.value != u8"λ") {
//         // Just a assigment
//         return std::make_unique<BinaryOperatorAST>(u8"=", std::make_unique<VariableDeclarationAST>(type, identifier), parseExpression());
//     }

//     // That's a function
//     getNextToken(); // eat 'λ'

//     if(m_currentToken.type != TokenType::PUNCTUATION || m_currentToken.value != u8"(") {
//         std::cerr << "Error: Expected '(' in function definition\n";
//         return nullptr;
//     }
//     getNextToken(); // eat '('

//     std::vector<VariableDeclarationAST> args;

//     return nullptr;
// }

// std::unique_ptr<FunctionAST> Parser::parseTopLevelExpr() {
//     if (auto expr = parseExpression()) {
//         return std::make_unique<FunctionAST>(u8"nihil", u8"anon", std::vector<VariableDeclarationAST>(), std::move(expr));
//     }
//     return nullptr;
// }
