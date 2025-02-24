#include "AST.hpp"

void AST::printTree(const std::string& prefix, bool isLeft) const {
    std::cout << prefix;
    std::cout << (isLeft ? "├── " : "└── ");
    std::cout << TOKEN_TYPE_LABELS[(int)m_token.type] << ": " << (const char*)m_token.value.c_str() << std::endl; // Assuming Token has a value member

    if (m_left)
        m_left->printTree(prefix + (isLeft ? "│   " : "    "), true);
    if (m_right)
        m_right->printTree(prefix + (isLeft ? "│   " : "    "), false);
}

NumberAST::NumberAST(int value) : m_value(value) {}

llvm::Value* NumberAST::codegen() {
    return nullptr;
}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& type, const std::u8string& name)
    : m_type(type), m_name(name) {}

llvm::Value* VariableDeclarationAST::codegen() {
    return nullptr;
}

VariableReferenceAST::VariableReferenceAST(const std::u8string &name)
    : m_name(std::move(name)) {}

llvm::Value *VariableReferenceAST::codegen() {
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

FunctionAST::FunctionAST(const std::u8string& returnType, const std::u8string& name, std::vector<VariableDeclarationAST> args, std::unique_ptr<AST> body)
    : m_returnType(std::move(returnType)), m_name(std::move(name)), m_args(std::move(args)), m_body(std::move(body)) {}

llvm::Value* FunctionAST::codegen() {
    return nullptr;
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<AST> then, std::unique_ptr<AST> _else)
    : m_cond(std::move(cond)), m_then(std::move(then)), m_else(std::move(_else)) {}

llvm::Value* IfAST::codegen() {
    return nullptr;
}

ForAST::ForAST(const std::u8string& varName, std::unique_ptr<AST> start, std::unique_ptr<AST> end, std::unique_ptr<AST> step, std::unique_ptr<AST> body)
    : m_varName(std::move(varName)), m_start(std::move(start)), m_end(std::move(end)), m_step(std::move(step)), m_body(std::move(body)) {}

llvm::Value* ForAST::codegen() {
    return nullptr;
}

