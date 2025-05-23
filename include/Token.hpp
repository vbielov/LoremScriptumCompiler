#pragma once
#include <string>

enum class TokenType {
    EOF_TOKEN,      // End of the file
    IDENTIFIER,     // Name of the variable, function, ect. [a-z][a-zA-Z0-9]
    NUMBER,         // Integer, ∅
    LITERAL,        // anything but ", surrounded by "'s
    STRING,
    BOOL,
    TYPE,           // numerus, literra, nihil, ect.
    OPERATOR,       // , :=, =, ≠ , >, <, >=, <=, +, -, *, /, ^, ¬, !
    PUNCTUATION,    // (, ), :, ;
    KEYWORD,        // λ, retro, finio, ∑(∞), si, ni
    NEW_LINE,       // \n
};

inline const char* TOKEN_TYPE_LABELS[] = {
    "EOF_TOKEN",
    "IDENTIFIER",
    "NUMBER",
    "LITERAL",
    "STRING",
    "BOOL",
    "TYPE",
    "OPERATOR",
    "PUNCTUATION",
    "KEYWORD",
    "NEW_LINE",
};

struct Token {
    TokenType type;
    std::u8string value; 
};
