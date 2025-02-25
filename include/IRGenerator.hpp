#pragma once
#include "AST.hpp"

/// @brief Transforms Abstract syntax tree in Intermediate Representation of LLVM
class IRGenerator {
private:
    AST* m_root;
public:
    IRGenerator(const std::unique_ptr<AST>& rootBlock);

    void generateIR();
};
