#pragma once
#include "AST.hpp"

Type* getVariableType(const std::u8string& typeStr, const LLVMStructs& llvmStructs);

/// @brief Transforms Abstract syntax tree in Intermediate Representation of LLVM
class IRGenerator {
private:
    AST* m_root;
    LLVMStructs m_llvmStructs;

public:
    IRGenerator(const char* moduleID, const std::unique_ptr<AST>& rootBlock);

    void generateIRCode();
    Module* getModule();
    std::string getIRCodeString();
};
