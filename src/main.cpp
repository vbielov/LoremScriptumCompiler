#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/MCJIT.h"
#include "llvm/Support/TargetSelect.h"

int main() {
    std::cout << "Hello World!\n";

    // Create an LLVM context
    llvm::LLVMContext Context;

    // Create an LLVM module
    llvm::Module Module("TestModule", Context);

    // Create a function type (void testFunction())
    llvm::FunctionType *funcType = llvm::FunctionType::get(llvm::Type::getVoidTy(Context), false);
    llvm::Function *testFunction = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, "testFunction", Module);

    // Create a basic block and attach it to the function
    llvm::BasicBlock *entry = llvm::BasicBlock::Create(Context, "entry", testFunction);
    llvm::IRBuilder<> builder(entry);

    // Return from function
    builder.CreateRetVoid();

    // Print the generated LLVM IR
    Module.print(llvm::errs(), nullptr);

    return 0;
}