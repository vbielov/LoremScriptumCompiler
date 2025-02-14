#pragma once
#include <string>

enum class TokenType {
    EOF_TOKEN,      // End of the file
    IDENTIFIER,     // Name of the variable, function, ect.
    NUMBER,         // Integer, ∅
    LITERAL,        // anything but ", surrounded by "'s
    TYPE,           // numerus, literra, nihil, ect.
    COMMENT,        // //
    OPERATOR,       // , :=, =, ≠ , >, <, >=, <=, +, -, *, /, ^, ¬, !
    PUNCTUATION,    // (, ), :, ;
    KEYWORD,        // λ, retro, finio, ∑(∞), si, ni
};

struct Token {
    TokenType type;
    std::string value;
};