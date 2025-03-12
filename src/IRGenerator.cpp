#include "IRGenerator.hpp"

IRGenerator::IRGenerator(const char* moduleID, const std::unique_ptr<AST>& rootBlock)
    : m_root(rootBlock.get())
    , m_context{
        std::make_unique<llvm::LLVMContext>(),
        std::make_unique<llvm::Module>(moduleID, *m_context.context),
        std::make_unique<llvm::IRBuilder<>>(*m_context.context),
        SymbolTable(),
        std::stack<llvm::BasicBlock*>() 
    } {}

void IRGenerator::generateIRCode() {
    m_root->codegen(m_context);
}

llvm::Module* IRGenerator::getModule() {
    return m_context.theModule.get();
}

std::string IRGenerator::getIRCodeString() {
    std::string IRCode;
    llvm::raw_string_ostream outStream(IRCode);
    m_context.theModule->print(outStream, nullptr);
    return outStream.str();
}
