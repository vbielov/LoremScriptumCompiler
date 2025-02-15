#pragma once
#include "Token.hpp"
#include <vector>
#include <cstring>
#include <assert.h>
#include <iostream>

inline static const char PUNCTUATION[] = {
    '(', ')', ':', ';'
};

inline static const char* KEYWORDS[] = {
    "\u03BB", // lambda
    "retro", 
    "finio", 
    "\u2211(\u221E)", // sum, infinity
    "si", 
    "ni"
};

inline static const char* TYPES[] = {
    "numerus", "literra", "nihil"
};

inline static const char* OPERATORS[] = {
    ",",
    ":=", 
    "=", 
    "\u2260", // not equal 
    ">", 
    "<", 
    ">=", 
    "<=", 
    "+", 
    "-", 
    "*", 
    "/", 
    "^", 
    "\u00AC", // negation
    "!"
};

// TODO(Vlad): Enable unicode support and use wchar_t instead of char to enable greek letters

class Lexer {
private:
    const char* m_souceCode;
    int m_charIterator; // for going through the sourceCode chars

public:
    Lexer(const char* sourceCode);
    Token getNextToken();

private:
    // checks if text starts with some string from array
    int startWithWord(const std::string& text, const char** strArr, int arrSize, bool isOperator);
};