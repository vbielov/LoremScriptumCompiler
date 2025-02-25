#include "AST.hpp"

void printIndent(const std::string &indent, bool isLast) {
    std::cout << indent;
    if (isLast) {
        std::cout << "└── ";
    } else {
        std::cout << "├── ";
    }
}

BlockAST::BlockAST(std::vector<std::unique_ptr<AST>> instructions)
    : m_instructions(std::move(instructions)) {}

llvm::Value* BlockAST::codegen() {
    return nullptr;
}

void BlockAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "BlockAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_instructions.size(); i++) {
        m_instructions[i]->printTree(newIndent, i == m_instructions.size() - 1);
    }
}

NumberAST::NumberAST(int value) : m_value(value) {}

llvm::Value* NumberAST::codegen() {
    return nullptr;
}

void NumberAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "NumberAST(" << m_value << ")" << std::endl;
}

CharAST::CharAST(char8_t character) : m_char(character) {}

llvm::Value* CharAST::codegen() {
    return nullptr;
}

void CharAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "CharAST('" << (char)m_char << "')" << std::endl;
}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& type, const std::u8string& name)
    : m_type(type), m_name(name) {}

llvm::Value* VariableDeclarationAST::codegen() {
    return nullptr;
}

void VariableDeclarationAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "VariableDeclarationAST(" << std::string(m_type.begin(), m_type.end()) << " "
              << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

VariableReferenceAST::VariableReferenceAST(const std::u8string &name)
    : m_name(std::move(name)) {}

llvm::Value* VariableReferenceAST::codegen() {
    return nullptr;
}

void VariableReferenceAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "VariableReferenceAST(" << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS) 
    : m_op(std::move(op)), m_LHS(std::move(LHS)), m_RHS(std::move(RHS)) {}

llvm::Value* BinaryOperatorAST::codegen() {
    return nullptr;
}

void BinaryOperatorAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "BinaryOperatorAST('" << std::string(m_op.begin(), m_op.end()) << "')" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_LHS->printTree(newIndent, false);
    m_RHS->printTree(newIndent, true);
}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args)
    : m_calleeIdentifier(std::move(callee)), m_args(std::move(args)) {}

llvm::Value* FuncCallAST::codegen() {
    return nullptr;
}

void FuncCallAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "FuncCallAST(" << std::string(m_calleeIdentifier.begin(), m_calleeIdentifier.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(newIndent, i == m_args.size() - 1);
    }
}

FunctionAST::FunctionAST(const std::u8string& returnType, const std::u8string& name, std::vector<std::unique_ptr<VariableDeclarationAST>> args, std::unique_ptr<BlockAST> body)
    : m_returnType(std::move(returnType)), m_name(std::move(name)), m_args(std::move(args)), m_body(std::move(body)) {}

llvm::Value* FunctionAST::codegen() {
    return nullptr;
}

void FunctionAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "FunctionAST(" << std::string(m_returnType.begin(), m_returnType.end()) << " "
              << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(newIndent, i == m_args.size() - 1);
    }
    m_body->printTree(newIndent, true);
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else)
    : m_cond(std::move(cond)), m_then(std::move(then)), m_else(std::move(_else)) {}

llvm::Value* IfAST::codegen() {
    return nullptr;
}

void IfAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "IfAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_cond->printTree(newIndent, false);
    m_then->printTree(newIndent, m_else == nullptr);
    if (m_else) m_else->printTree(newIndent, true);
}

ForAST::ForAST(const std::u8string& varName, std::unique_ptr<AST> start, std::unique_ptr<AST> end, std::unique_ptr<AST> step, std::unique_ptr<BlockAST> body)
    : m_varName(std::move(varName)), m_start(std::move(start)), m_end(std::move(end)), m_step(std::move(step)), m_body(std::move(body)) {}

llvm::Value* ForAST::codegen() {
    return nullptr;
}

void ForAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "ForAST(" << std::string(m_varName.begin(), m_varName.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_start->printTree(newIndent, false);
    m_end->printTree(newIndent, false);
    m_step->printTree(newIndent, false);
    m_body->printTree(newIndent, true);
}
