#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <sstream>

#include <filesystem>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "IRGenerator.hpp"
#include "Assembler.hpp"
#include <cstdlib>

std::u8string readFileToU8String(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);  // Open file in binary mode
    if (!file) {
        return u8"";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::u8string(content.begin(), content.end());  // Convert to u8string
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: lsc <input_file.lorem>" << std::endl;
        return 1;
    }

    // Read File
    const char* inputFilePath = argv[1];
    std::string inputFilePathStr = inputFilePath;
    size_t lastindex = inputFilePathStr.find_last_of("."); 
    std::string rawFilePath = inputFilePathStr.substr(0, lastindex);
    std::string objFileName = rawFilePath + ".o";
    std::string asmFileName = rawFilePath + ".asm";
    std::string exeFileName = rawFilePath;
    std::string fileName = std::filesystem::path(inputFilePathStr).stem().string();
    #if defined(_WIN32)
        exeFileName += ".exe";
    #endif

    std::u8string sourceCode = readFileToU8String(inputFilePath);
    if (sourceCode.empty()) {
        std::cerr << "Error: Couldn't read the file " << inputFilePath << std::endl;
        return 1;
    }

    std::cout << "----------------------- Source Code: ----------------------- " << std::endl
              << std::endl;
    std::cout << (const char*)(sourceCode.c_str()) << std::endl
              << std::endl;

    // Tokenize
    std::cout << "----------------------- Tokens: ----------------------- " << std::endl
              << std::endl;
    Lexer lexer = Lexer(sourceCode);
    Token token;
    while ((token = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        std::cout << TOKEN_TYPE_LABELS[(int)token.type] << ": " << (const char*)(token.value.c_str()) << std::endl;
    }
    std::cout << std::endl;
    lexer = Lexer(sourceCode);  // reset Lexer

    // Parser
    std::cout << "----------------------- Abstract Syntax Tree: ----------------------- " << std::endl
              << std::endl;
    Parser parser = Parser(lexer);
    auto tree = parser.parseBlock();
    if (tree) {
        tree->printTree(std::cout, "", false);
    } else {
        std::cerr << "Error: Nothing is parsed" << std::endl;
    }
    std::cout << std::endl;

    if (!tree) {
        return 1;
    }

    // Generate IR
    std::cout << "----------------------- LLVM IR Code: ----------------------- " << std::endl << std::endl;
    IRGenerator codeGenerator = IRGenerator(fileName.c_str(), tree);
    codeGenerator.generateIRCode();
    std::cout << codeGenerator.getIRCodeString()<< std::endl;

    // Assembler
    std::cout << "----------------------- Assembly: ----------------------- " << std::endl << std::endl;
    
    Assembler assembler;
    assembler.compileToObjectFile(asmFileName.c_str(), codeGenerator.getModule(), CodeGenFileType::AssemblyFile);
    assembler.compileToObjectFile(objFileName.c_str(), codeGenerator.getModule(), CodeGenFileType::ObjectFile); 

    std::u8string assembly = readFileToU8String(asmFileName);
    std::cout << (const char*)(assembly.c_str()) << std::endl;

    assembler.compileToExecutable(objFileName.c_str(), exeFileName.c_str(), codeGenerator.getModule());

    lld::exitLld(0); // NOTE: it should not be used like this...
    return 0;
}