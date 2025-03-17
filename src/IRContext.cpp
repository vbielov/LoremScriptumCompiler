#include "IRContext.hpp"
#include "ErrorHandler.hpp"

ScopeEntry::ScopeEntry(const std::u8string* name, const IDataType* type, llvm::Value* value)
    : name(name), type(type), value(value) {}

Scope::Scope()
    : variables(), functions() {}

SymbolTable::SymbolTable()
    : m_scopes({Scope()}) {}

void SymbolTable::enterScope() {
    m_scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!m_scopes.empty())
        m_scopes.pop_back();
}

void SymbolTable::addVariable(const std::u8string& name, const IDataType* type, llvm::Value* value) {
    m_scopes.back().variables.emplace_back(&name, type, value);
}

void SymbolTable::addFunction(const std::u8string& name, const IDataType* type, llvm::Value* value) {
    m_scopes.back().functions.emplace_back(&name, type, value);
}

const ScopeEntry* SymbolTable::lookupVariable(const std::u8string& name) const {
    for (int i = (int)m_scopes.size()-1; i >= 0; i--) {
        const Scope& scope = m_scopes[i];
        auto iter = std::find_if(scope.variables.begin(), scope.variables.end(), [&name](const ScopeEntry& var) {
            return *var.name == name;
        });
        if (iter != scope.variables.end()) {
            return iter.base();
        }
        
    }
    queueUndefinedError(u8"variable not found: '" + name +u8"' check your scopes!");
    return nullptr;
}

const ScopeEntry* SymbolTable::lookupFunction(const std::u8string& name) const { 
    for (int i = (int)m_scopes.size()-1; i >= 0; i--) {
        const Scope& scope = m_scopes[i];
        auto iter = std::find_if(scope.functions.begin(), scope.functions.end(), [&name](const ScopeEntry& var) {
            return *var.name == name;
        });
        if (iter != scope.functions.end()) {
            return iter.base();
        }
    }
    queueUndefinedError(u8"function not found: '" + name +u8"' check your scopes!");
    return nullptr;
}
