#include "Lexer.hpp"

Lexer::Lexer(const std::u8string& sourceCode) 
    : m_souceCode(&sourceCode), m_charIterator(0) {}

Token Lexer::getNextToken() {
    // ignore any whitespace, new line, tab, ect.
    while ( getCharAt(m_charIterator) == u8' ' || 
            getCharAt(m_charIterator) == u8'\n' || 
            getCharAt(m_charIterator) == u8'\r' ||
            getCharAt(m_charIterator) == u8'\t') {
        m_charIterator++;
    }

    // End of the file
    if (getCharAt(m_charIterator) == u8'\0') {
        return {TokenType::EOF_TOKEN, u8""};
    }

    // is punctuation
    for (int i = 0; i < (int)PUNCTUATION.size(); i++) {
        if (getCharAt(m_charIterator) == PUNCTUATION[i][0]) {
            std::u8string puctuationSymbol = u8"";
            puctuationSymbol += getCharAt(m_charIterator);
            m_charIterator++;
            return {TokenType::PUNCTUATION, std::move(puctuationSymbol)};
        }
    }

    // is keyword
    int keywordIndex = startWithWord(KEYWORDS, false);
    if (keywordIndex != -1) {
        m_charIterator += KEYWORDS[keywordIndex].length();
        return {TokenType::KEYWORD, KEYWORDS[keywordIndex]};
    }

    // is type
    int typeIndex = startWithWord(VARIABLE_TYPE_LABELS, false);
    if (typeIndex != -1) {
        m_charIterator += VARIABLE_TYPE_LABELS[typeIndex].length();
        return {TokenType::TYPE, VARIABLE_TYPE_LABELS[typeIndex]};
    }

    // if comment
    if (getCharAt(m_charIterator) == u8'/' && getCharAt(m_charIterator+1) == '/') {
        std::u8string commentStr = u8"";
        while (getCharAt(m_charIterator) != u8'\n' && getCharAt(m_charIterator) != '\0') {
            commentStr += getCharAt(m_charIterator);
            m_charIterator++;
        }
        return {TokenType::COMMENT, std::move(commentStr)};
    }

    // is operator
    int operatorIndex = startWithWord(OPERATORS, true);
    if (operatorIndex != -1) {
        m_charIterator += OPERATORS[operatorIndex].length();
        return {TokenType::OPERATOR, OPERATORS[operatorIndex]};
    }

    // if literal
    if (getCharAt(m_charIterator) == u8'\'') {
        std::u8string literraSymbol = u8"";
        m_charIterator++; // skip '
        // NOTE(Vlad):  it's allowed here to input multiple symbols to literal, 
        //              parser should not let literal with multiple symbols inside
        while (getCharAt(m_charIterator) != u8'\'') {
            literraSymbol += getCharAt(m_charIterator);
            m_charIterator++;
        }
        m_charIterator++; // skip '
        return {TokenType::LITERAL, std::move(literraSymbol)};
    }

    // if number
    if (getCharAt(m_charIterator) >= u8'A' && getCharAt(m_charIterator) <= 'Z') {
        std::u8string numberStr = u8"";
        while (getCharAt(m_charIterator) >= u8'A' && getCharAt(m_charIterator) <= u8'Z') {
            numberStr += getCharAt(m_charIterator);
            m_charIterator++;
        }
        return {TokenType::NUMBER, std::move(numberStr)};
    }

    // if identifier
    std::u8string identifierStr = u8"";
    while ( getCharAt(m_charIterator) != u8' ' && 
            getCharAt(m_charIterator) != u8'\n' && 
            getCharAt(m_charIterator) != u8'\0' && 
            startWithWord(OPERATORS, true) == -1 &&
            startWithWord(PUNCTUATION, true) == -1) {
        identifierStr += getCharAt(m_charIterator);
        m_charIterator++;
    }
    return {TokenType::IDENTIFIER, std::move(identifierStr)};
}

int Lexer::startWithWord(const std::vector<std::u8string>& strList, bool isOperator) {
    for (int i = 0; i < (int)strList.size(); i++) {
        bool isEqual = true;
        int j = 0;
        for (; j < (int)strList[i].length(); j++) {
            if (getCharAt(m_charIterator+j) != strList[i][j]) {
                isEqual = false;
                break;
            }    
        }
        if (isEqual) {
            if ((!isOperator) && ( // operator can be before identifier, for example "¬cadabra"
                (getCharAt(m_charIterator+j) >= u8'a' && getCharAt(m_charIterator+j) <= u8'z') ||
                (getCharAt(m_charIterator+j) >= u8'A' && getCharAt(m_charIterator+j) <= u8'Z') ||
                (getCharAt(m_charIterator+j) >= u8'0' && getCharAt(m_charIterator+j) <= u8'9'))) {
                // it's some identifier 
                break;
            }
            return i;
        }
    }
    return -1;
}

char8_t Lexer::getCharAt(int index) const {
    return (*m_souceCode)[index];
}
