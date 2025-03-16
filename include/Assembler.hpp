#pragma once
#include <string>
#include "llvm/IR/Module.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Linker/Linker.h"
#include "llvm/Object/ObjectFile.h"
#include "lld/Common/Driver.h"
#include "lld/Common/ErrorHandler.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include "fastlz.h"

using namespace llvm;
using namespace llvm::sys;
using namespace llvm::object;

#if defined(_WIN32)
    LLD_HAS_DRIVER(mingw)
    #include "lib/windows/crt2.hpp"
    #include "lib/windows/libgcc.hpp"
    #include "lib/windows/libkernel32.hpp"
    #include "lib/windows/libmingw32.hpp"
    #include "lib/windows/libmingwex.hpp"
    #include "lib/windows/libmsvcrt.hpp"
#elif defined(__linux__)
    LLD_HAS_DRIVER(elf)
    #include "lib/linux/crt1.hpp"
    #include "lib/linux/crti.hpp"
    #include "lib/linux/crtn.hpp"
    #include "lib/linux/libc.hpp"
    #include "lib/linux/libgcc_eh.hpp"
    #include "lib/linux/libgcc.hpp"
#endif

class Assembler {
private:
    const std::string* m_irCode;

public:
    void compileToObjectFile(const char* objectFileName, Module* module, CodeGenFileType fileType);
    void compileToExecutable(const char* objectFilePath, const char* executableFilePath, std::vector<std::filesystem::path>& linkLibraries);

private:
    static std::filesystem::path storeFileTmp(const char* name, const unsigned char* compressedData, size_t compressedSize, size_t originalSize);
};