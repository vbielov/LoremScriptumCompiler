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
    std::string sourceCode;
    while (std::getline(inputFile, lineBuffer)) {
        sourceCode += lineBuffer + "\n"; // das is sehr langsam, aber egal.
    }

    // Tokenize
    Lexer lexer = Lexer(sourceCode.c_str());

    // Parser
    

    // Generate IR

    // End?

    return 0;
}