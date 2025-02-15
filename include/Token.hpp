#pragma once
#include <string>

enum class TokenType {
    EOF_TOKEN,      // End of the file
    IDENTIFIER,     // Name of the variable, function, ect. [a-z][a-zA-Z0-9]
    NUMBER,         // Integer, ∅
    LITERAL,        // anything but ", surrounded by "'s
    TYPE,           // numerus, literra, nihil, ect.
    COMMENT,        // //
    OPERATOR,       // , :=, =, ≠ , >, <, >=, <=, +, -, *, /, ^, ¬, !
    PUNCTUATION,    // (, ), :, ;
    KEYWORD,        // λ, retro, finio, ∑(∞), si, ni
};

inline static const char* TOKEN_TYPE_LABELS[] = {
    "EOF_TOKEN",
    "IDENTIFIER",
    "NUMBER",
    "LITERAL",
    "TYPE",
    "COMMENT",
    "OPERATOR",
    "PUNCTUATION",
    "KEYWORD",
};

struct Token {
    TokenType type;
    std::string value;
};