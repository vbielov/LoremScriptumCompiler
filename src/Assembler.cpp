#include "Assembler.hpp"

void Assembler::compileToObjectFile(const char* objectFilePath, Module* module, CodeGenFileType fileType) {
	InitializeAllTargetInfos();
	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmParsers();
	InitializeAllAsmPrinters();

	std::string targetTriple;

	// Detect platform that user is using and compile for it
	#if defined(_WIN32)
		targetTriple = "x86_64-pc-windows-msvc"; 
	#elif defined(__linux__)
		targetTriple = sys::getDefaultTargetTriple();
	#endif
	
	module->setTargetTriple(targetTriple);

	std::string Error;
	auto target = TargetRegistry::lookupTarget(targetTriple, Error);

	if (!target) {
		llvm::errs() << Error;
		return;
	}

	auto cpu = "generic";
	auto features = "";

	TargetOptions opt;
	auto TheTargetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, Reloc::PIC_);

	module->setDataLayout(TheTargetMachine->createDataLayout());

	std::error_code errorCode;
	raw_fd_ostream dest(objectFilePath, errorCode, sys::fs::OF_None);

	if (errorCode) {
		llvm::errs() << "Could not open file: " << errorCode.message();
		return;
	}

	legacy::PassManager pass;

	if (TheTargetMachine->addPassesToEmitFile(pass, dest, nullptr, fileType)) {
		llvm::errs() << "TheTargetMachine can't emit a file of this type";
		return;
	}

	pass.run(*module);
	dest.flush();
}

void Assembler::compileToExecutable(const char* objectFilePath, const char* executableFilePath, Module* srcModule) {
	lld::Result result;
	#if defined(_WIN32)
		const char* WINDOWS_ARGS[] = {
			"ld", 
			"../lib/windows/crt2.o", 
			objectFilePath, "-o", executableFilePath, 
			"../lib/windows/libgcc.a", "../lib/windows/libmingw32.a", "../lib/windows/libmingwex.a", 
			"../lib/windows/libmsvcrt.a", "../lib/windows/libkernel32.a",
			nullptr
		};
		const lld::DriverDef WINDOWS_DRIVERS[] = {
			{lld::MinGW, &lld::mingw::link}
		};
		result = lld::lldMain(WINDOWS_ARGS, llvm::outs(), llvm::errs(), WINDOWS_DRIVERS);
	#elif defined(__linux__)
		const char* LINUX_ARGS[] = {
			"ld.lld", 
			"../lib/linux/crt1.o", "../lib/linux/crti.o", "../lib/linux/crtn.o",
			objectFilePath, "-o", executableFilePath,
			"../lib/linux/libc.a", "../lib/linux/libgcc_eh.a", "../lib/linux/libgcc.a",
			nullptr
		};
		const lld::DriverDef LINUX_DRIVER[] = {
			{lld::Gnu, &lld::elf::link}
		};
		result = lld::lldMain(LINUX_ARGS, llvm::outs(), llvm::errs(), LINUX_DRIVER);
	#endif

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
