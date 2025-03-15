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
#include "ErrorHandler.hpp"
#include <cstdlib>
#include "Preprocessor.hpp"

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

    std::filesystem::path mainFilePath = std::filesystem::canonical(inputFilePathStr);
    std::u8string sourceCode = u8"";
    std::vector<std::filesystem::path> includeStack;
    std::vector<std::filesystem::path> linkLibraries;
    processPreprocessors(mainFilePath, sourceCode, includeStack, linkLibraries);
    
    if (sourceCode.empty()) {
        std::cerr << "Error: Couldn't read the file " << inputFilePath << std::endl;
        return 1;
    }

    //ErrorHandler grabbing source code pointer
    buildRanges(sourceCode);
    grabSource(sourceCode, inputFilePathStr); //TODO: start thread maybe and lock ErrorHandler in meantime
    //
    

    std::cout << "----------------------- Source Code: ----------------------- " << std::endl
              << std::endl;
    std::cout << (const char*)(sourceCode.c_str()) << std::endl
              << std::endl;

    // Tokenize
    std::cout << "----------------------- Tokens: ----------------------- " << std::endl
              << std::endl;
    Lexer lexer = Lexer(sourceCode);
    std::vector<Token> tokens;
    lexer.tokenize(tokens);
    for(const auto& token : tokens) {
        std::cout << TOKEN_TYPE_LABELS[(int)token.type] << ": " << (const char*)(token.value.c_str()) << std::endl;
    }
    std::cout << std::endl;

    // Parser
    std::cout << "----------------------- Abstract Syntax Tree: ----------------------- " << std::endl
              << std::endl;
    Parser parser = Parser(tokens);
    std::unique_ptr<AST> tree = parser.parse();

    // ErrorHandler
    if(error()){ // check if any errors occured
        dumpErrorLog();
        return 1;
    }
    //

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
    std::string fileName = std::filesystem::path(inputFilePathStr).stem().string();

    IRGenerator codeGenerator = IRGenerator(fileName.c_str(), tree);
    codeGenerator.generateIRCode();

    // ErrorHandler
    if(error()){ // check if any errors occured
        dumpErrorLog();
        return 1;
    }
    //

    std::cout << codeGenerator.getIRCodeString()<< std::endl;

    // Assembler
    std::cout << "----------------------- Assembly: ----------------------- " << std::endl << std::endl;
    std::string objFileName = rawFilePath + ".o";
    std::string asmFileName = rawFilePath + ".asm";
    std::string exeFileName = rawFilePath;
    #if defined(_WIN32)
        exeFileName += ".exe";
    #endif

    Assembler assembler;
    assembler.compileToObjectFile(asmFileName.c_str(), codeGenerator.getModule(), CodeGenFileType::AssemblyFile);
    assembler.compileToObjectFile(objFileName.c_str(), codeGenerator.getModule(), CodeGenFileType::ObjectFile); 
    assembler.compileToExecutable(objFileName.c_str(), exeFileName.c_str(), linkLibraries);

    lld::exitLld(0); // NOTE: it should not be used like this...
    return 0;
}