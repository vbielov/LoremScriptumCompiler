#include "AST.hpp"

void printIndent(std::ostream& ostr, const std::string& indent, bool isLast) {
    ostr << indent;
    if (isLast) {
        ostr << "└── ";
    } else {
        ostr << "├── ";
    }
}

BlockAST::BlockAST(std::vector<std::unique_ptr<AST>> instructions)
    : m_instructions(std::move(instructions)) {}

Value* BlockAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void BlockAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "BlockAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_instructions.size(); i++) {
        m_instructions[i]->printTree(ostr, newIndent, i == m_instructions.size() - 1);
    }
}

NumberAST::NumberAST(int value) : m_value(value) {}

Value* NumberAST::codegen(LLVMStructs& llvmStructs) {
    return ConstantInt::get(*llvmStructs.theContext, APInt(32, m_value, true));
}

void NumberAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "NumberAST(" << m_value << ")" << std::endl;
}

CharAST::CharAST(char8_t character) : m_char(character) {}

Value* CharAST::codegen(LLVMStructs& llvmStructs) {
    return ConstantInt::get(*llvmStructs.theContext, APInt(8, m_char, false));
}

void CharAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "CharAST('" << (char)m_char << "')" << std::endl;
}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& type, const std::u8string& name)
    : m_type(type), m_name(name) {}

Value* VariableDeclarationAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void VariableDeclarationAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "VariableDeclarationAST(" << std::string(m_type.begin(), m_type.end()) << " "
         << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

VariableReferenceAST::VariableReferenceAST(const std::u8string& name)
    : m_name(std::move(name)) {}

Value* VariableReferenceAST::codegen(LLVMStructs& llvmStructs) {
    std::string str = (const char*)m_name.c_str();
    Value* value = llvmStructs.namedValues[str];
    if (!value) {
        std::cerr << RED << "Error: Unknown variable name" << RESET << std::endl;
    }
    return value;
}

void VariableReferenceAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "VariableReferenceAST(" << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS)
    : m_op(std::move(op)), m_LHS(std::move(LHS)), m_RHS(std::move(RHS)) {}

Value* BinaryOperatorAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void BinaryOperatorAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "BinaryOperatorAST('" << std::string(m_op.begin(), m_op.end()) << "')" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_LHS->printTree(ostr, newIndent, false);
    m_RHS->printTree(ostr, newIndent, true);
}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args)
    : m_calleeIdentifier(std::move(callee)), m_args(std::move(args)) {}

Value* FuncCallAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void FuncCallAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FuncCallAST(" << std::string(m_calleeIdentifier.begin(), m_calleeIdentifier.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(ostr, newIndent, i == m_args.size() - 1);
    }
}

FunctionAST::FunctionAST(const std::u8string& returnType, const std::u8string& name, std::vector<std::unique_ptr<VariableDeclarationAST>> args, std::unique_ptr<BlockAST> body)
    : m_returnType(std::move(returnType)), m_name(std::move(name)), m_args(std::move(args)), m_body(std::move(body)) {}

Value* FunctionAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void FunctionAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FunctionAST(" << std::string(m_returnType.begin(), m_returnType.end()) << " "
         << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(ostr, newIndent, i == m_args.size() - 1);
    }
    m_body->printTree(ostr, newIndent, true);
}

ReturnAST::ReturnAST(std::unique_ptr<AST> expr) : m_expr(std::move(expr)) {}

Value* ReturnAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void ReturnAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    ostr << "ReturnAST" << std::endl;
    m_expr->printTree(ostr, newIndent, true);
}

BreakAST::BreakAST() {}

Value* BreakAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void BreakAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    ostr << "BreakAST" << std::endl;
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else)
    : m_cond(std::move(cond)), m_then(std::move(then)), m_else(std::move(_else)) {}

Value* IfAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void IfAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "IfAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_cond->printTree(ostr, newIndent, false);
    m_then->printTree(ostr, newIndent, m_else == nullptr);
    if (m_else) m_else->printTree(ostr, newIndent, true);
}

LoopAST::LoopAST(std::unique_ptr<BlockAST> body) : m_body(std::move(body)) {}

Value* LoopAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

void LoopAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "LoopAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_body->printTree(ostr, newIndent, true);
}
