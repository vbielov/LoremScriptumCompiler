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
    // https://www.reddit.com/r/C_Programming/comments/1ac62ll/comment/kjtttcg/
    #if defined(_WIN32)
        auto chkstr_ms = std::make_unique<FunctionPrototypeAST>(
            u8"___chkstk_ms", 
            std::make_unique<PrimitiveDataType>(PrimitiveType::VOID),
            std::vector<std::unique_ptr<TypeIdentifierPair>>(),
            false
        );
        chkstr_ms->codegen(m_context);

        std::vector<std::unique_ptr<AST>> block;
        block.emplace_back(std::make_unique<FuncCallAST>(
            u8"___chkstk_ms", std::vector<std::unique_ptr<AST>>()
        ));

        auto chkstk = std::make_unique<FunctionAST>(
            std::make_unique<FunctionPrototypeAST>(
                u8"__chkstk",
                std::make_unique<PrimitiveDataType>(PrimitiveType::VOID),
                std::vector<std::unique_ptr<TypeIdentifierPair>>(),
                true
            ),
            std::make_unique<BlockAST>(std::move(block))
        );
        chkstk->codegen(m_context);
    #endif

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
