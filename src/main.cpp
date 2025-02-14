#include <iostream>
#include <fstream>
#include <sstream>
#include <Lexer.hpp>

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: lsc <input_file.ls>" << std::endl;
        return 2;
    }

    // Read File
    const char* inputFilePath = argv[1];
    std::ifstream inputFile(inputFilePath);
    std::string lineBuffer;
    std::vector<std::string> sourceCodeLines;
    while (std::getline(inputFile, lineBuffer)) {
        sourceCodeLines.emplace_back(std::move(lineBuffer));
    }

    // Tokenize
    std::vector<Token> tokens;
    tokenize(sourceCodeLines, &tokens);

    // Build AST

    // Generate IR

    // End?
    
    return 0;
}