#include "IRContext.hpp"
#include "ErrorHandler.hpp"

ScopeEntry::ScopeEntry(const std::u8string* name, const IDataType* type, llvm::Value* value)
    : name(name), type(type), value(value) {}

Scope::Scope()
    : variables() {}

SymbolTable::SymbolTable()
    : m_scopes()
    , m_globals()
    , m_functions()
    , m_structsTypeMap() {}

void SymbolTable::enterScope() {
    m_scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (!m_scopes.empty())
        m_scopes.pop_back();
}

void SymbolTable::clearScopes() {
    while (!m_scopes.empty()) {
        m_scopes.pop_back();
    }
}

void SymbolTable::addVariable(const std::u8string& name, const IDataType* type, llvm::Value* value) {
    if (m_scopes.size() == 0)
        m_scopes.emplace_back();

    m_scopes.back().variables.emplace_back(&name, type, value);
}

void SymbolTable::addGlobal(const std::u8string& name, const IDataType* type, llvm::GlobalVariable* value) {
    m_globals.emplace_back(&name, type, value);
}

void SymbolTable::addFunction(const std::u8string& name, const IDataType* type, llvm::Value* value) {
    m_functions.emplace_back(&name, type, value);
}

void SymbolTable::addStruct(const std::u8string& name, const StructDataType* type) {
    m_structsTypeMap[name] = type;
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

    auto iter = std::find_if(m_globals.begin(), m_globals.end(), [&name](const ScopeEntry& entry) {
        return *entry.name == name;
    });
    if (iter != m_globals.end())
        return iter.base();

    queueUndefinedError(u8"variable not found: '" + name + u8"' check your scopes!");
    return nullptr;
}

const ScopeEntry* SymbolTable::lookupFunction(const std::u8string& name) const { 
    auto iter = std::find_if(m_functions.begin(), m_functions.end(), [&name](const ScopeEntry& entry) {
        return *entry.name == name;
    });

    if (iter != m_functions.end())
        return iter.base();

    queueUndefinedError(u8"function not found: '" + name + u8"' check your scopes!");
    return nullptr;
}

const StructDataType* SymbolTable::lookupStruct(const std::u8string& name) const {
    auto iter = m_structsTypeMap.find(name);
    if (iter != m_structsTypeMap.end())
        return iter->second;
    queueUndefinedError(u8"struct not found: '" + name + u8"' !");
    return nullptr;
}
