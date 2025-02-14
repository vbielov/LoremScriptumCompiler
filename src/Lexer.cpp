#include "Lexer.hpp"

static const char* PUNCTUATION[] = {
    "(", ")", ":", ";"
};

static const char* KEYWORDS[] = {
    "\u03BB", "retro", "finio", "\u2211(\u221E)", "si", "ni"
};

static const char* TYPES[] = {
    "numbers", "literra", "nihil"
};

static const char* OPERATORS[] = {
    "," ":=", "=", "\u2260" , ">", "<", ">=", "<=", "+", "-", "*", "/", "^", "\u00AC", "!"
};

void tokenize(const std::vector<std::string>& sourceCodeLines, std::vector<Token>* outTokens) {

}
