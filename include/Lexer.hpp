#pragma once
#include <assert.h>
#include <codecvt>
#include <cstring>
#include <iostream>
#include <vector>
#include "Token.hpp"
#include "Syntax.hpp"

class Lexer {
   private:
    const std::u8string* m_souceCode;
    int m_charIterator;

   public:
    Lexer(const std::u8string& sourceCode);
    void tokenize(std::vector<Token>& outTokens, std::ostream &ostr);
    
private:
    /// @return next token from code given in constructor
    Token getNextToken();

    /// @brief checks if text starts with some string from vector
    /// @param allowIdentifierAfter if true, will allow identifiers to start directly after operators without space in-between.
    ///                     Example: operators can be before identifier "¬cadabra"
    /// @return index to the string, or -1 if current code is not starting with any element from the vector
    int startWithWord(const std::u8string_view* words, size_t wordsSize, bool allowIdentifierAfter) const;

    char8_t getCharAt(int index) const;
};