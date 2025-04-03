#include "Preprocessor.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "IRGenerator.hpp"
#include "Assembler.hpp"
#include "ErrorHandler.hpp"

int main(int argc, const char** argv) {
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        std::cout << "Usage: \n"
            <<"\t"<<"lsc <input_file.lorem>"<<"           "<<"compiles file to executable\n"
            <<"\t"<<"lsc <input_file.lorem> -l"<<"        "<<"dumps logs upon detection of error or warning" 
            << std::endl;
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0) {
        std::cout << "LSC 1.0.0 (built by Backbenchers)";
        return 0;
    }

    if (argc == 3 && strcmp(argv[2], "-l") == 0) {
        std::cout << "logs dumped on creation\n";
        setInstantDump();
    }

    // Read File
    const char* inputFilePath = argv[1];
    std::filesystem::path mainFilePath;
    try {
        mainFilePath = std::filesystem::canonical(inputFilePath);
    } catch([[maybe_unused]] std::filesystem::filesystem_error& e) {
        std::cerr << "Error: Couldn't read the file " << inputFilePath << std::endl;
        return 1;
    }

    // Preprocess
    Preprocessor preprocessor = Preprocessor();
    std::u8string sourceCode = preprocessor.process(mainFilePath); 
    
    buildRanges(sourceCode);
    grabSource(sourceCode, inputFilePath); //TODO: start thread maybe and lock ErrorHandler in meantime
    
    // Tokenize
    Lexer lexer = Lexer(sourceCode);
    std::vector<Token> tokens;
    lexer.tokenize(tokens, std::cout);

    // Parse
    Parser parser = Parser(tokens);
    std::unique_ptr<AST> tree = parser.parse();
    
    if (error()) { // check if any errors occured
        dumpErrorLog();
        return 1;
    }

    // Generate IR
    IRGenerator codeGenerator = IRGenerator(mainFilePath.stem().string().c_str(), tree);
    codeGenerator.generateIRCode();

    if (error()) { // check if any errors occured
        dumpErrorLog();
        return 1;
    }

    // Assemble
    std::filesystem::path outputDir = mainFilePath.parent_path();
    std::filesystem::path objFilePath = outputDir / mainFilePath.stem();
    objFilePath += ".o";
    std::filesystem::path exeFilePath = outputDir / mainFilePath.stem();
    #ifdef _WIN32
        exeFilePath += ".exe";
    #endif
    
    Assembler assembler;
    assembler.compileToObjectFile(objFilePath, codeGenerator.getModule(), CodeGenFileType::ObjectFile); 
    auto libs = preprocessor.getLinkLibs();
    assembler.compileToExecutable(objFilePath, exeFilePath, libs);

    if (warn()) { // check if any warnings occured
        dumpErrorLog();
    }

    // Note: There is a bug, when lld is linked dynamicly, that it can't stop program after end of main()
    //       Mingw doesn't support staticlly linking LLVM/LLD, for some reason => it will not allow exiting program without lld::exitLld(0), 
    //       Maybe it's somehow related to this bug: https://reviews.llvm.org/D102684
    #ifdef _WIN32
        lld::exitLld(0);
    #endif

    return 0;
}