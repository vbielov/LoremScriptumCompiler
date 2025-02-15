#include "Lexer.hpp"

Lexer::Lexer(const char* sourceCode) 
    : m_souceCode(sourceCode), m_charIterator(0) {}

Token Lexer::getNextToken() {
    // ignore any whitespace, new line, tab, ect.
    while ( m_souceCode[m_charIterator] == ' ' || 
            m_souceCode[m_charIterator] == '\n' || 
            m_souceCode[m_charIterator] == '\r' ||
            m_souceCode[m_charIterator] == '\t') {
        m_charIterator++;
    }

    // End of the file
    if (m_souceCode[m_charIterator] == '\0') {
        return Token{TokenType::EOF_TOKEN, ""};
    }

    // is punctuation
    for (int i = 0; i < sizeof(PUNCTUATION); i++) {
        if (m_souceCode[m_charIterator] == PUNCTUATION[i]) {
            std::string puctuationSymbol = "";
            puctuationSymbol += m_souceCode[m_charIterator];
            m_charIterator++;
            return Token{TokenType::PUNCTUATION, std::move(puctuationSymbol)};
        }
    }

    // is keyword
    int keywordIndex = startWithWord(m_souceCode, KEYWORDS, sizeof(KEYWORDS) / sizeof(const char*), false);
    if (keywordIndex != -1) {
        m_charIterator += strlen(KEYWORDS[keywordIndex]);
        return Token{TokenType::KEYWORD, KEYWORDS[keywordIndex]};
    }

    // is type
    int typeIndex = startWithWord(m_souceCode, TYPES, sizeof(TYPES) / sizeof(const char*), false);
    if (typeIndex != -1) {
        m_charIterator += strlen(TYPES[typeIndex]);
        return Token{TokenType::TYPE, TYPES[typeIndex]};
    }

    // if comment
    if (m_souceCode[m_charIterator] == '/' && m_souceCode[m_charIterator+1] == '/') {
        std::string commentStr = "";
        while (m_souceCode[m_charIterator] != '\n' && m_souceCode[m_charIterator] != '\0') {
            commentStr += m_souceCode[m_charIterator];
            m_charIterator++;
        }
        return Token{TokenType::COMMENT, std::move(commentStr)};
    }

    // is operator
    int operatorIndex = startWithWord(m_souceCode, OPERATORS, sizeof(OPERATORS) / sizeof(const char*), true);
    if (operatorIndex != -1) {
        m_charIterator += strlen(OPERATORS[operatorIndex]);
        return Token{TokenType::OPERATOR, OPERATORS[operatorIndex]};
    }

    // if literal
    // TODO(Vlad): think how you would parse \n
    if (m_souceCode[m_charIterator] == '\'' && m_souceCode[m_charIterator+1] != '\0' && m_souceCode[m_charIterator+2] == '\'') {
        if ((m_souceCode[m_charIterator+1] >= 'a' && m_souceCode[m_charIterator+1] <= 'z') ||
            (m_souceCode[m_charIterator+1] >= 'A' && m_souceCode[m_charIterator+1] <= 'Z') ||
            (m_souceCode[m_charIterator+1] >= '0' && m_souceCode[m_charIterator+1] <= '9')) {
                std::string literraSymbol = "";
                literraSymbol += m_souceCode[m_charIterator+1];
                m_charIterator += 3; // skip ', literra and '
                return Token{TokenType::LITERAL, std::move(literraSymbol)};
        }
    }

    // if number
    if (m_souceCode[m_charIterator] >= 'A' && m_souceCode[m_charIterator] <= 'Z') {
        std::string numberStr = "";
        while (m_souceCode[m_charIterator] >= 'A' && m_souceCode[m_charIterator] <= 'Z') {
            numberStr += m_souceCode[m_charIterator];
            m_charIterator++;
        }
        return Token{TokenType::NUMBER, std::move(numberStr)};
    }

    // if identifier
    if (m_souceCode[m_charIterator] >= 'a' && m_souceCode[m_charIterator] <= 'z') {
        std::string identifierStr = "";
        while ( (m_souceCode[m_charIterator] >= 'a' && m_souceCode[m_charIterator] <= 'z') ||
                (m_souceCode[m_charIterator] >= 'A' && m_souceCode[m_charIterator] <= 'Z') ||
                (m_souceCode[m_charIterator] >= '0' && m_souceCode[m_charIterator] <= '9')) {
            identifierStr += m_souceCode[m_charIterator];
            m_charIterator++;
        }
        return Token{TokenType::IDENTIFIER, std::move(identifierStr)};
    }

    std::cout << m_souceCode[m_charIterator] << std::endl;
    assert(false && "Unknown token");
}

int Lexer::startWithWord(const std::string& text, const char** strArr, int arrSize, bool isOperator) {
    for (int i = 0; i < arrSize; i++) {
        bool isEqual = true;
        int j = 0;
        for (; j < strlen(strArr[i]); j++) {
            if (m_souceCode[m_charIterator+j] != strArr[i][j]) {
                isEqual = false;
                break;
            }    
        }
        if (isEqual) {
            if (!isOperator && // operator can be directory before identifier, for example "¬cadabra"
                (m_souceCode[m_charIterator+j] >= 'a' && m_souceCode[m_charIterator+j] <= 'z') ||
                (m_souceCode[m_charIterator+j] >= 'A' && m_souceCode[m_charIterator+j] <= 'Z') ||
                (m_souceCode[m_charIterator+j] >= '0' && m_souceCode[m_charIterator+j] <= '9')) {
                // it's some identifier 
                break;
            }
            return i;
        }
    }
    return -1;
};
