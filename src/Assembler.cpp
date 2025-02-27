#include "Assembler.hpp"

void Assembler::compileToObjectFile(const char* objectFileName, Module* module) {
    // Initialize the target registry etc.
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();

    // TODO: Detect platform that user is using and compile for it
    // auto TargetTriple = sys::getDefaultTargetTriple(); // default is linux for some reason
    auto TargetTriple = "x86_64-pc-windows-msvc"; 
    module->setTargetTriple(TargetTriple);

    std::string Error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target) {
      errs() << Error;
      return;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto TheTargetMachine = Target->createTargetMachine(
        TargetTriple, CPU, Features, opt, Reloc::PIC_);

    module->setDataLayout(TheTargetMachine->createDataLayout());

    std::error_code EC;
    raw_fd_ostream dest(objectFileName, EC, sys::fs::OF_None);

    if (EC) {
      errs() << "Could not open file: " << EC.message();
      return;
    }

    legacy::PassManager pass;
    auto FileType = CodeGenFileType::ObjectFile;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
      errs() << "TheTargetMachine can't emit a file of this type";
      return;
    }

    pass.run(*module);
    dest.flush();
}