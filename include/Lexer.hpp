#pragma once
#include "Token.hpp"
#include <vector>

void tokenize(const std::vector<std::string>& sourceCodeLines, std::vector<Token>* outTokens);