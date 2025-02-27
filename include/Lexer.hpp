#pragma once
#include <assert.h>

#include <codecvt>
#include <cstring>
#include <iostream>
#include <vector>

#include "Token.hpp"

inline static const std::vector<std::u8string> PUNCTUATION = {
    u8"(",
    u8")",
    u8":",
    u8";",
    u8",",
};

inline static const std::vector<std::u8string> KEYWORDS = {
    u8"λ",
    u8"retro",
    u8"finio",
    u8"∑(∞)",
    u8"si",
    u8"nisi",
    u8"ni",
};

inline static const std::vector<std::u8string> VARIABLE_TYPE_LABELS = {
    u8"numerus",
    u8"literra",
    u8"nihil",
    // u8"asertio",
    // u8"rerum",
};

inline static const std::vector<std::u8string> OPERATORS = {
    u8"==",
    u8"=",
    u8"≠",
    u8">",
    u8"<",
    u8">=",
    u8"<=",
    u8"+",
    u8"-",
    u8"*",
    u8"/",
    u8"%",
    u8"^",
    u8"¬",
    u8"!",
};

class Lexer {
   private:
    // NOTE(Vlad): std::u8string may not be supported by some systems...
    const std::u8string* m_souceCode;
    int m_charIterator;

   public:
    Lexer(const std::u8string& sourceCode);

    /// @return next token from code given in constructor
    Token getNextToken();

   private:
    /// @brief checks if text starts with some string from vector
    /// @param isOperator if true, will allow identifiers to start directly after operators without space in-between
    /// @return index to the string, or -1 if current code is not starting with any element from the vector
    int startWithWord(const std::vector<std::u8string>& strList, bool isOperator);

    char8_t getCharAt(int index) const;
};