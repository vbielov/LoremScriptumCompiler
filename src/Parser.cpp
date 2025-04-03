#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) 
    : m_tokens(tokens)
    , m_currentToken(nullptr)
    , m_ostr(std::cout)
    , m_loopCount(0)
    , m_blockCount(-1)
    , m_isValid(true)
    , m_isTest(false)
    , m_structHashMap() {}

Parser::Parser(const std::vector<Token>& tokens, bool isTest, std::ostream &ostr) 
    : m_tokens(tokens)
    , m_currentToken(nullptr)
    , m_ostr(ostr)
    , m_loopCount(0)
    , m_blockCount(-1)
    , m_isValid(true)
    , m_isTest(isTest)
    , m_structHashMap() {}

std::unique_ptr<BlockAST> Parser::parse() {
    if (m_isTest) {
        // Do not make a main function wrapper for parse tests -> Easier to debug
        return parseBlock();
    }

    auto block = parseBlock();

    // Create main wrapper function
    std::unique_ptr<AST> pseudoReturnValue = std::make_unique<NumberAST>(0, currentLine);
    auto pseudoReturn = std::make_unique<ReturnAST>(std::move(pseudoReturnValue), currentLine);

    auto pseudoBlockInstr = std::vector<std::unique_ptr<AST>>();
    pseudoBlockInstr.push_back(std::move(block));
    pseudoBlockInstr.push_back(std::move(pseudoReturn));
    auto pseudoBlock = std::make_unique<BlockAST>(std::move(pseudoBlockInstr), currentLine);

    std::unique_ptr<IDataType> mainReturnType = std::make_unique<PrimitiveDataType>(PrimitiveType::INT);
    auto pseudoFunctionPrototype = std::make_unique<FunctionPrototypeAST>(
        u8"main", 
        std::move(mainReturnType),
        std::vector<std::unique_ptr<TypeIdentifierPair>>(),
        true,
        currentLine
    );
    auto pseudoFunction = std::make_unique<FunctionAST>(std::move(pseudoFunctionPrototype), std::move(pseudoBlock), currentLine);

    m_topLevelDeclarations.push_back(std::move(pseudoFunction));

    auto treeRoot = std::make_unique<BlockAST>(std::move(m_topLevelDeclarations), currentLine);

    #if !defined(NDEBUG)
    m_ostr << "----------------------- Abstract Syntax Tree: ----------------------- " << std::endl << std::endl;
    if (treeRoot) {
        treeRoot->printTree(m_ostr, "", false);
    }
    m_ostr << std::endl;
    #endif

    return treeRoot;
}


bool Parser::isValid() {
    return m_isValid;
}

bool Parser::isInsideLoop() {
    return m_loopCount > 0;
}

bool Parser::isExpressionEnd() {
    return isToken(TokenType::NEW_LINE) || isToken(TokenType::EOF_TOKEN) || isToken(TokenType::PUNCTUATION);
}

bool Parser::isToken(TokenType type) {
    return m_currentToken->type == type;
}

bool Parser::isToken(const std::u8string_view& value) {
    return m_currentToken->value == value;
}

bool Parser::isToken(TokenType type, const std::u8string_view& value) {
    return m_currentToken->type == type && m_currentToken->value == value;
}

bool Parser::isUnaryOperator() {
    return isToken(TokenType::OPERATOR, operators::PLUS) || isToken(TokenType::OPERATOR, operators::MINUS) || isToken(TokenType::OPERATOR, operators::NOT);
}

/**
* Examples:
*    - numerus
*    - numerus[I]
*    - numerus[]
*    - nihil
*    - struct[I]
*    - struct
*/
std::unique_ptr<IDataType> Parser::parseType() {
    std::unique_ptr<IDataType> basicType; // first part of the type without array part
    std::unordered_map<std::u8string, StructDataType*>::iterator iter;
    if ((iter = m_structHashMap.find(m_currentToken->value)) != m_structHashMap.end()) {
        basicType = std::make_unique<StructDataType>(iter->first);
    } else {
        auto typeIter = STR_TO_PRIMITIVE_MAP.find(m_currentToken->value);
        if (typeIter == STR_TO_PRIMITIVE_MAP.end()) {
            logError(currentLine, u8"Unknown or invalid type!");
            return nullptr;
        }
        basicType = std::make_unique<PrimitiveDataType>(typeIter->second);
    }
    getNextToken(); // eat basic type

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_OPEN)) {
        return basicType;
    }

    // That's array!
    // TODO(Vlad): 2D Arrays?
    PrimitiveDataType* primitiveType = dynamic_cast<PrimitiveDataType*>(basicType.get());
    if (primitiveType && primitiveType->type == PrimitiveType::VOID) {
        logError(currentLine, u8"Void type cannot be an array!");
        return nullptr;
    }

    // it is array declaration
    getNextToken(); // eat '['

    // TODO(Vlad): Test arrays with no given size
    int arrSize = 0;
    if (isToken(TokenType::NUMBER)) {
        bool success = toArabicConverter(m_currentToken->value, &arrSize);
        if (!success) {
            logError(currentLine, u8"Syntax Error: invalid roman number!");
            return nullptr;
        }
        getNextToken(); // eat number
    }

    if (!isToken(TokenType::PUNCTUATION, punctuation::SQR_BRACKET_CLOSE)) {
        logError(currentLine, u8"Syntax Error: closing array bracket ']' expected!");
        return nullptr;
    }
    getNextToken(); // eat ']'
    return std::make_unique<ArrayDataType>(std::move(basicType), arrSize);
}

const Token& Parser::getNextToken() {
    // Get first token
    if (m_currentToken.base() == nullptr) {
        m_currentToken = m_tokens.begin();
        return *m_currentToken;
    }
    m_currentToken++;
    assert(m_currentToken != m_tokens.end());
    return *m_currentToken; 
}
