#include "AST.hpp"

NumberAST::NumberAST(int value) : m_value(value) {}

llvm::Value* NumberAST::codegen() {
    return nullptr;
}

VariableAST::VariableAST(const std::u8string& name) 
    : m_name(std::move(name)) {}

llvm::Value* VariableAST::codegen() {
    return nullptr;
}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS) 
    : m_op(std::move(op)), m_LHS(std::move(LHS)), m_RHS(std::move(RHS)) {}

llvm::Value* BinaryOperatorAST::codegen() {
    return nullptr;
}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args)
    : m_calleeIdentifier(std::move(callee)), m_args(std::move(args)) {}

llvm::Value* FuncCallAST::codegen() {
    return nullptr;
}

FuncDeclarationAST::FuncDeclarationAST(const std::u8string& name, std::vector<std::u8string> args)
    : m_name(std::move(name)), m_args(std::move(args)) {}

llvm::Value* FuncDeclarationAST::codegen() {
    return nullptr;
}

FuncDefinitionAST::FuncDefinitionAST(std::unique_ptr<FuncDeclarationAST> declaration, std::unique_ptr<AST> body)
    : m_declaration(std::move(declaration)), m_body(std::move(body)) {}

llvm::Value* FuncDefinitionAST::codegen() {
    return nullptr;
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<AST> then, std::unique_ptr<AST> _else)
    : m_cond(std::move(cond)), m_then(std::move(then)), m_else(std::move(_else)) {}

llvm::Value* IfAST::codegen() {
    return nullptr;
}

ForAST::ForAST(const std::u8string& varName, std::unique_ptr<AST> start, std::unique_ptr<AST> end, std::unique_ptr<AST> step, std::unique_ptr<AST> body)
    : m_varName(std::move(varName)), m_start(std::move(start)), m_end(std::move(end)), m_body(std::move(body)) {}

llvm::Value* ForAST::codegen() {
    return nullptr;
}
