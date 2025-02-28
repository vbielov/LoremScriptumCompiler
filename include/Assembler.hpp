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
#include <iostream>
#include "lld/Common/Driver.h"
#include "lld/Common/ErrorHandler.h"

using namespace llvm;
using namespace llvm::sys;
using namespace llvm::object;

LLD_HAS_DRIVER(mingw)

class Assembler {
private:
    const std::string* m_irCode;

public:
    void compileToObjectFile(const char* objectFileName, Module* module);
    void compileToExecutable(const char* executableFileName, Module* srcModule);
};