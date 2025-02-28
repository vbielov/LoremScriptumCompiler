#include <iostream>
#include <sstream>
#include <fstream>
#include <codecvt>
#include <locale>
#include "Lexer.hpp"
#include "Parser.hpp"
#include "IRGenerator.hpp"
#include "Assembler.hpp"
#include <cstdlib>

std::u8string readFileToU8String(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);  // Open file in binary mode
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return std::u8string(content.begin(), content.end()); // Convert to u8string
}

int main(int argc, const char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: lsc <input_file.lorem>" << std::endl;
        return 2;
    }

    // Read File
    const char* inputFilePath = argv[1];
    std::u8string sourceCode = readFileToU8String(inputFilePath);

    std::cout << "----------------------- Source Code: ----------------------- " << std::endl << std::endl;
    std::cout << (const char*)(sourceCode.c_str()) << std::endl << std::endl;

    // Tokenize
    std::cout << "----------------------- Tokens: ----------------------- " << std::endl << std::endl;
    Lexer lexer = Lexer(sourceCode);
    Token token;
    while ((token = lexer.getNextToken()).type != TokenType::EOF_TOKEN) {
        std::cout << TOKEN_TYPE_LABELS[(int)token.type] << ": " << (const char*)(token.value.c_str()) << std::endl;
    }
    std::cout << std::endl;
    lexer = Lexer(sourceCode); // reset Lexer

    // Parser
    std::cout << "----------------------- Abstract Syntax Tree: ----------------------- " << std::endl << std::endl;
    Parser parser = Parser(lexer);
    std::unique_ptr<AST> tree = parser.parseProgram();
    if (tree) {
        tree->printTree("", false);
    } else {
        std::cerr << "Error: Nothing parsed :(" << std::endl;
    }
    std::cout << std::endl;

    if (!tree) {
        return 1;
    }

    // Generate IR
    std::cout << "----------------------- LLVM IR Code: ----------------------- " << std::endl << std::endl;
    IRGenerator codeGenerator = IRGenerator(tree);
    codeGenerator.generateIRCode();
    std::cout << codeGenerator.getIRCodeString()<< std::endl;


    std::cout << "----------------------- Assembly: ----------------------- " << std::endl << std::endl;
    
    std::string inputFilePathStr = inputFilePath;
    size_t lastindex = inputFilePathStr.find_last_of("."); 
    std::string rawname = inputFilePathStr.substr(0, lastindex);
    std::string objFileName = rawname + ".o";
    std::string asmFileName = rawname + ".asm";
    std::string exeFileName = rawname + ".exe"; // TODO: don't add extension, but add a flag on linux
    
    Assembler assembler;
    assembler.compileToObjectFile(objFileName.c_str(), codeGenerator.getModule(), CodeGenFileType::ObjectFile); 
    assembler.compileToObjectFile(asmFileName.c_str(), codeGenerator.getModule(), CodeGenFileType::AssemblyFile);

    std::u8string assembly = readFileToU8String(asmFileName);
    std::cout << (const char*)(assembly.c_str()) << std::endl;

    assembler.compileToExecutable(objFileName.c_str(), exeFileName.c_str(), codeGenerator.getModule());

    lld::exitLld(0); // NOTE: it should not be used like this...
    return 0;
}