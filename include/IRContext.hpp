#pragma once
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "Types.hpp"
#include <memory>
#include <map>
#include <stack>
#include <vector>

struct ScopeEntry {
    const std::u8string* name;
    const IDataType* type;
    llvm::Value* value;

    ScopeEntry(const std::u8string* name, const IDataType* type, llvm::Value* value);
};

struct Scope {
    std::vector<ScopeEntry> variables;
    std::vector<ScopeEntry> functions;

    Scope();
};

class SymbolTable {
private:
    std::vector<Scope> m_scopes; 
public:
    SymbolTable();
    void enterScope();
    void exitScope();
    void addVariable(const std::u8string& name, const IDataType* type, llvm::Value* value);
    void addFunction(const std::u8string& name, const IDataType* type, llvm::Value* value);
    const ScopeEntry* lookupVariable(const std::u8string& name) const;
    const ScopeEntry* lookupFunction(const std::u8string& name) const;
};

struct IRContext {
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> theModule;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    SymbolTable symbolTable;
    std::stack<llvm::BasicBlock*> afterLoop; // needed for break in a nestes for loop
};