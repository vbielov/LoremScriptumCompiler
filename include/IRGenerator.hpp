#pragma once
#include "AST.hpp"

/// @brief Transforms Abstract syntax tree in Intermediate Representation of LLVM
class IRGenerator {
private:
    AST* m_root;
    IRContext m_context;

public:
    IRGenerator(const char* moduleID, const std::unique_ptr<AST>& rootBlock);

    void generateIRCode();
    llvm::Module* getModule();
    std::string getIRCodeString();
};
