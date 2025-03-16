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
		// NOTE: For some reason "x86_64-w64-mingw32" triple produces ELF objects, but on windows we expect PE
		targetTriple = "x86_64-pc-windows-gnu";
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

struct IncludedBinaryFile {
	const char* name;
	const unsigned char* compressedData;
	size_t compressedSize;
	size_t originalSize;
};

void Assembler::compileToExecutable(const char* objectFilePath, const char* executableFilePath, std::vector<std::filesystem::path>& linkLibraries) {
	lld::Result result;
	std::vector<std::string> args;
	lld::DriverDef drivers[1] = {};

	#if defined(_WIN32)
		IncludedBinaryFile includedFiles[] = {
			IncludedBinaryFile{ .name = "crt2.o", .compressedData = CRT2, .compressedSize = sizeof(CRT2), .originalSize = CRT2_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libgcc.a", .compressedData = LIBGCC, .compressedSize = sizeof(LIBGCC), .originalSize = LIBGCC_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libmingw32.a", .compressedData = LIBMINGW32, .compressedSize = sizeof(LIBMINGW32), .originalSize = LIBMINGW32_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libmingwex.a", .compressedData = LIBMINGWEX, .compressedSize = sizeof(LIBMINGWEX), .originalSize = LIBMINGWEX_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libmsvcrt.a", .compressedData = LIBMSVCRT, .compressedSize = sizeof(LIBMSVCRT), .originalSize = LIBMSVCRT_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libkernel32.a", .compressedData = LIBKERNEL32, .compressedSize = sizeof(LIBKERNEL32), .originalSize = LIBKERNEL32_ORIGINAL_SIZE },
		};

		args.push_back("ld");
		args.push_back(objectFilePath);
		args.push_back("-o");
		args.push_back(executableFilePath);

		for (const auto& file : includedFiles) {
			auto path = storeFileTmp(file.name, file.compressedData, file.compressedSize, file.originalSize);
			args.push_back(path.string());
		}

		drivers[0] = {lld::MinGW, &lld::mingw::link};
	#elif defined(__linux__)
		IncludedBinaryFile includedFiles[] = {
			IncludedBinaryFile{ .name = "crt1.o", .compressedData = CRT1, .compressedSize = sizeof(CRT1), .originalSize = CRT1_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "crti.o", .compressedData = CRTI, .compressedSize = sizeof(CRTI), .originalSize = CRTI_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "crtn.o", .compressedData = CRTN, .compressedSize = sizeof(CRTN), .originalSize = CRTN_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libc.a", .compressedData = LIBC, .compressedSize = sizeof(LIBC), .originalSize = LIBC_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libgcc.a", .compressedData = LIBGCC, .compressedSize = sizeof(LIBGCC), .originalSize = LIBGCC_ORIGINAL_SIZE },
			IncludedBinaryFile{ .name = "libgcc_eh.a", .compressedData = LIBGCC_EH, .compressedSize = sizeof(LIBGCC_EH), .originalSize = LIBGCC_EH_ORIGINAL_SIZE },
		};

		args.push_back("ld.lld");
		args.push_back(objectFilePath);
		args.push_back("-o");
		args.push_back(executableFilePath);

		for (const auto& file : includedFiles) {
			auto path = storeFileTmp(file.name, file.compressedData, file.compressedSize, file.originalSize);
			args.push_back(path.string());
		}

		drivers[0] = {lld::Gnu, &lld::elf::link};
	#else
		assert(false && "Unsupported OS");
	#endif
		
	for(const auto& lib : linkLibraries) {
		args.push_back(lib.string().c_str());
	}

	std::vector<const char*> argsCstr(args.size());
	for (size_t i = 0; i < args.size(); i++) {
		argsCstr[i] = args[i].c_str();
	}

	result = lld::lldMain(argsCstr, llvm::outs(), llvm::errs(), drivers);


	if (result.retCode != 0) {
		llvm::errs() << "Error: Linking failed with return code " << result.retCode << "\n";
		return;
	}

	if (!result.canRunAgain) {
		llvm::errs() << "Error: Linker cannot run again, exiting...\n"; 
		llvm::errs().flush();
		lld::exitLld(result.retCode);
		return;
	}

	llvm::outs().flush();
	llvm::errs().flush();
	std::cout << "Linked successfuly" << std::endl;
}

std::filesystem::path Assembler::storeFileTmp(const char* name, const unsigned char* compressedData, size_t compressedSize, size_t originalSize) {
    const auto pathForFile = std::filesystem::temp_directory_path() / "lsc" / name;
    if (std::filesystem::exists(pathForFile)) {
        // NOTE: It might be a better idea to calculate checksum of this file and use it, if it's OK
        //       But for now I link small libraries and don't care about performance as much.
        std::filesystem::remove(pathForFile);
    }

    std::filesystem::create_directories(pathForFile.parent_path());

    std::ofstream tmpFile;
    tmpFile.open(pathForFile, std::ios::binary);

	std::vector<char> data = std::vector<char>(compressedData, compressedData+ compressedSize);
	std::vector<char> decompressedData(originalSize);
    int decompressedSize = fastlz_decompress(data.data(), data.size(), decompressedData.data(), originalSize);
    assert(decompressedSize > 0 && "Decompression failed");

    tmpFile.write(reinterpret_cast<const char*>(decompressedData.data()), originalSize);
    tmpFile.flush();
    tmpFile.close();
    return pathForFile;
}
