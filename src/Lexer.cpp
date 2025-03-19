#include "Lexer.hpp"

Lexer::Lexer(const std::u8string& sourceCode) 
    : m_souceCode(&sourceCode)
    , m_charIterator(0) {}

void Lexer::tokenize(std::vector<Token>& outTokens) {
    Token token;
    while (true) {
        token = getNextToken();
        outTokens.push_back(token);
        if (token.type == TokenType::EOF_TOKEN)
            break;
    }

    #if !defined(NDEBUG)
    std::cout << "----------------------- Tokens: ----------------------- " << std::endl << std::endl;
    for(const auto& token : outTokens) {
        std::cout << TOKEN_TYPE_LABELS[(int)token.type] << ": " << (const char*)(token.value.c_str()) << std::endl;
    }
    std::cout << std::endl;
    #endif
}

Token Lexer::getNextToken() {
    // ignore any whitespace, tab, ect.
    while ( getCharAt(m_charIterator) == u8' ' || 
            getCharAt(m_charIterator) == u8'\r' ||
            getCharAt(m_charIterator) == u8'\t') {
        m_charIterator++;
    }

    // End of the file
    if (getCharAt(m_charIterator) == u8'\0') {
        return {TokenType::EOF_TOKEN, u8""};
    }

    // New line
    if (getCharAt(m_charIterator) == u8'\n') {
        m_charIterator++;
        return {TokenType::NEW_LINE, u8""};
    }

    // is punctuation
    for (size_t i = 0; i < punctuation::VALUES_SIZE; i++) {
        if (getCharAt(m_charIterator) == punctuation::VALUES[i][0]) {
            std::u8string puctuationSymbol = u8"";
            puctuationSymbol += getCharAt(m_charIterator);
            m_charIterator++;
            return {TokenType::PUNCTUATION, std::move(puctuationSymbol)};
        }
    }

    // is boolean
    int boolIndex = startWithWord(boolean_types::VALUES, boolean_types::VALUES_SIZE, false);
    if (boolIndex != -1) {
        m_charIterator += boolean_types::VALUES[boolIndex].length();
        return {TokenType::BOOL, std::u8string(boolean_types::VALUES[boolIndex])};
    }

    // is keyword
    int keywordIndex = startWithWord(keywords::VALUES, keywords::VALUES_SIZE, false);
    if (keywordIndex != -1) {
        m_charIterator += keywords::VALUES[keywordIndex].length();
        return {TokenType::KEYWORD, std::u8string(keywords::VALUES[keywordIndex])};
    }

    // is type
    int typeIndex = startWithWord(types::VALUES, types::VALUES_SIZE, false);
    if (typeIndex != -1) {
        m_charIterator += types::VALUES[typeIndex].length();
        return {TokenType::TYPE, std::u8string(types::VALUES[typeIndex])};
    }

    // if comment
    if (getCharAt(m_charIterator) == u8'/' && getCharAt(m_charIterator+1) == '/') {
        while (getCharAt(m_charIterator) != u8'\n' && getCharAt(m_charIterator) != '\0') {
            m_charIterator++;
        }
        return getNextToken();
    }

    // is operator
    int operatorIndex = startWithWord(operators::VALUES, operators::VALUES_SIZE, true);
    if (operatorIndex != -1) {
        m_charIterator += operators::VALUES[operatorIndex].length();
        return {TokenType::OPERATOR, std::u8string(operators::VALUES[operatorIndex])};
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
            getCharAt(m_charIterator) != u8'\r' && 
            getCharAt(m_charIterator) != u8'\0' && 
            startWithWord(operators::VALUES, operators::VALUES_SIZE, true) == -1 &&
            startWithWord(punctuation::VALUES, punctuation::VALUES_SIZE, true) == -1) {
        identifierStr += getCharAt(m_charIterator);
        m_charIterator++;
    }
    return {TokenType::IDENTIFIER, std::move(identifierStr)};
}


int Lexer::startWithWord(const std::u8string_view* words, size_t wordsSize, bool allowIdentifierAfter) const {
    for (size_t i = 0; i < wordsSize; i++) {
        const std::u8string_view& word = words[i];
        std::u8string_view subString(m_souceCode->c_str() + m_charIterator, word.length());
        if (subString.starts_with(word)) {
            if (!allowIdentifierAfter) {
                char8_t character = getCharAt(m_charIterator + word.length());
                if (character >= u8'a' && character <= u8'z') {
                    // there is some identifier after. So, the whole thing is identifier
                    return -1;
                }
            }
            return i;
        }
    }
    return -1;
}

char8_t Lexer::getCharAt(int index) const
{
    return (*m_souceCode)[index];
}
