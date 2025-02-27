#include "IRGenerator.hpp"

IRGenerator::IRGenerator(const std::unique_ptr<AST>& rootBlock) 
    : m_root(rootBlock.get())
    , m_llvmStructs{
        std::make_unique<LLVMContext>(),
        std::make_unique<Module>("my cool jit", *m_llvmStructs.theContext),
        std::make_unique<IRBuilder<>>(*m_llvmStructs.theContext)
    } {}

void IRGenerator::generateIRCode() {
    m_root->codegen(m_llvmStructs);
}

Module* IRGenerator::getModule() {
    return m_llvmStructs.theModule.get();
}

std::string IRGenerator::getIRCodeString() {
    std::string IRCode;
    llvm::raw_string_ostream outStream(IRCode);
    m_llvmStructs.theModule->print(outStream, nullptr);
    return outStream.str();
}
