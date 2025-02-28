#include "Assembler.hpp"

void Assembler::compileToObjectFile(const char* objectFilePath, Module* module, CodeGenFileType fileType) {
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
    raw_fd_ostream dest(objectFilePath, EC, sys::fs::OF_None);

    if (EC) {
      errs() << "Could not open file: " << EC.message();
      return;
    }

    legacy::PassManager pass;

    if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
      errs() << "TheTargetMachine can't emit a file of this type";
      return;
    }

    pass.run(*module);
    dest.flush();
}

void Assembler::compileToExecutable(const char* objectFilePath, const char* executableFilePath, Module* srcModule) {
    // LLD expects arguments just like command-line linking
    const char* args[] = {
        "ld", "../lib/crt2.o", objectFilePath, "-o", executableFilePath, 
        "../lib/libgcc.a", 
        "../lib/libmingw32.a", 
        "../lib/libmingwex.a", 
        "../lib/libmsvcrt.a", 
        "../lib/libkernel32.a",
        nullptr
    };

    // Define the drivers, including the GNU linker driver
    const lld::DriverDef drivers[] = {
        {lld::MinGW, &lld::mingw::link}
    };

    lld::Result result = lld::lldMain(args, llvm::outs(), llvm::errs(), drivers);

    if (result.retCode != 0) {
        llvm::errs() << "Error: Linking failed with return code " << result.retCode << "\n";
        return;
    }

    if (!result.canRunAgain) {
        llvm::errs() << "Error: Linker cannot run again, exiting...\n"; 
        lld::exitLld(result.retCode);
        return;
    }

    llvm::outs().flush();
    llvm::errs().flush();
    std::cout << "Linked successfuly" << std::endl;
}
