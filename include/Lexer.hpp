#pragma once
#include "Token.hpp"
#include <vector>

inline static const char* PUNCTUATION[] = {
    "(", ")", ":", ";"
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
    "numbers", "literra", "nihil"
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

class Lexer {
private:
    const char* m_souceCode;
    int m_charIterator; // for going through the sourceCode chars

public:
    Lexer(const char* sourceCode);
    Token getNextToken();
};