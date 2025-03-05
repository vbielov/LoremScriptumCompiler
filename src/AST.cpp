#include "AST.hpp"

//===----------------------------------------------------------------------===//
// AST
//===----------------------------------------------------------------------===//

Type* getTypeFromStr(const std::u8string& typeStr, const LLVMStructs& llvmStructs)
{
    if (typeStr == u8"numerus") {
        return Type::getInt32Ty(*(llvmStructs.theContext));
    } else if (typeStr == u8"litera") {
        return Type::getInt8Ty(*(llvmStructs.theContext));
    } else if (typeStr == u8"nihil") {
        return Type::getVoidTy(*(llvmStructs.theContext));
    } else if (typeStr == u8"asertio") {
        return Type::getInt1Ty(*(llvmStructs.theContext));
    }
    return nullptr;
}

const std::u8string& AST::getName() const {
    assert(false && "This AST can't have a name.");
    static std::u8string ilegal = u8"ilegal";
    return ilegal;
}

Type* AST::getType(LLVMStructs& llvmStructs) const {
    return this->getElementType(llvmStructs);
}

Type* AST::getElementType([[maybe_unused]] LLVMStructs& llvmStructs) const {
    assert(false && "This AST can't have a type.");
    return nullptr;
}

BlockAST::BlockAST(std::vector<std::unique_ptr<AST>> instructions)
    : m_instructions(std::move(instructions)) {}

NumberAST::NumberAST(int value) 
    : m_value(value) {}

Type* NumberAST::getElementType(LLVMStructs& llvmStructs) const {
    return Type::getInt32Ty(*(llvmStructs.theContext));
}

CharAST::CharAST(char8_t character) 
    : m_char(character) {}

Type* CharAST::getElementType(LLVMStructs& llvmStructs) const {
    return Type::getInt8Ty(*(llvmStructs.theContext));
}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& type, const std::u8string& name)
    : m_type(type)
    , m_name(name) {}

const std::u8string& VariableDeclarationAST::getName() const {
    return m_name;
}

Type* VariableDeclarationAST::getType(LLVMStructs& llvmStructs) const {
    return PointerType::getUnqual(getElementType(llvmStructs));
}

Type* VariableDeclarationAST::getElementType(LLVMStructs& llvmStructs) const {
    return getTypeFromStr(m_type, llvmStructs);
}

VariableReferenceAST::VariableReferenceAST(const std::u8string& name)
    : m_name(std::move(name)) {}

const std::u8string& VariableReferenceAST::getName() const {
    return m_name;
}

Type* VariableReferenceAST::getType(LLVMStructs& llvmStructs) const {
    auto iter = llvmStructs.namedValues.find(m_name);
    if (iter == llvmStructs.namedValues.end()) {
        std::cerr << RED << "Error: Variable " << (const char*)(m_name.c_str()) << " is not defined" << RESET << std::endl;
        return nullptr;  
    }
    return iter->second.value->getType();
}

Type* VariableReferenceAST::getElementType(LLVMStructs& llvmStructs) const {
    auto iter = llvmStructs.namedValues.find(m_name);
    if (iter == llvmStructs.namedValues.end()) {
        std::cerr << RED << "Error: Variable " << (const char*)(m_name.c_str()) << " is not defined" << RESET << std::endl;
        return nullptr;  
    }
    return iter->second.valueType;
}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS) 
    : m_op(std::move(op))
    , m_LHS(std::move(LHS))
    , m_RHS(std::move(RHS)) {}

Type* BinaryOperatorAST::getElementType(LLVMStructs& llvmStructs) const {
    Type* leftType = m_LHS->getElementType(llvmStructs);
    Type* rightType = m_RHS->getElementType(llvmStructs);
    if (leftType != rightType) {
        std::cerr << RED << "Error: It's not allowed to apply any operators to values with different types" << RESET << std::endl;
        return nullptr;       
    }
    return leftType;
}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args)
    : m_calleeIdentifier(std::move(callee))
    , m_args(std::move(args)) {}

const std::u8string& FuncCallAST::getName() const {
    return m_calleeIdentifier;
}

Type* FuncCallAST::getElementType(LLVMStructs& llvmStructs) const {
    auto iter = llvmStructs.namedValues.find(m_calleeIdentifier);
    if (iter == llvmStructs.namedValues.end()) {
        std::cerr << RED << "Error: Function" << (const char*)(m_calleeIdentifier.c_str()) << " is not defined" << RESET << std::endl;
        return nullptr;  
    }
    return iter->second.valueType;
}

FunctionPrototypeAST::FunctionPrototypeAST(const std::u8string& returnType, const std::u8string& name, std::vector<std::unique_ptr<VariableDeclarationAST>> args)
    : m_returnType(std::move(returnType))
    , m_name(std::move(name))
    , m_args(std::move(args)) {}

const std::u8string& FunctionPrototypeAST::getName() const {
    return m_name;
}

Type* FunctionPrototypeAST::getElementType(LLVMStructs& llvmStructs) const {
    return getTypeFromStr(m_returnType, llvmStructs);
}

const std::vector<std::unique_ptr<VariableDeclarationAST>>& FunctionPrototypeAST::getArgs() const {
    return m_args;
}

FunctionAST::FunctionAST(std::unique_ptr<FunctionPrototypeAST> prototype, std::unique_ptr<BlockAST> body)
    : m_prototype(std::move(prototype))
    , m_body(std::move(body)) {}

const std::u8string& FunctionAST::getName() const {
    return m_prototype->getName();
}

Type* FunctionAST::getElementType(LLVMStructs& llvmStructs) const {
    return m_prototype->getElementType(llvmStructs);
}

ReturnAST::ReturnAST(std::unique_ptr<AST> expr) 
    : m_expr(std::move(expr)) {}

Type* ReturnAST::getType(LLVMStructs& llvmStructs) const {
    return m_expr->getType(llvmStructs);
}

Type* ReturnAST::getElementType(LLVMStructs& llvmStructs) const {
    return m_expr->getElementType(llvmStructs);
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else)
    : m_cond(std::move(cond))
    , m_then(std::move(then))
    , m_else(std::move(_else)) {}

BreakAST::BreakAST() {}

LoopAST::LoopAST(std::unique_ptr<BlockAST> body) 
    : m_body(std::move(body)) {}

ArrayAST::ArrayAST(const std::u8string& type, const std::u8string& name, int size)
    : m_type(type)
    , m_name(name)
    , m_size(size) {}

ArrayAST::ArrayAST(const std::u8string &type, const std::u8string &name, int size, std::vector<std::unique_ptr<AST>> arrElements)
    : m_type(type)
    , m_name(name)
    , m_size(size)
    , m_arrElements(std::move(arrElements)) {}

const std::u8string& ArrayAST::getName() const {
    return m_name;
}

Type* ArrayAST::getType(LLVMStructs& llvmStructs) const {
    return ArrayType::get(getElementType(llvmStructs), m_size);
}

Type* ArrayAST::getElementType(LLVMStructs& llvmStructs) const {
    return getTypeFromStr(m_type, llvmStructs);
}

AccessArrayElementAST::AccessArrayElementAST(const std::u8string& name, std::unique_ptr<AST> index)
    : m_name(name)
    , m_index(std::move(index)) {}

const std::u8string& AccessArrayElementAST::getName() const {
    return m_name;
}

Type* AccessArrayElementAST::getElementType(LLVMStructs& llvmStructs) const {
    auto iter = llvmStructs.namedValues.find(m_name);
    if (iter == llvmStructs.namedValues.end()) {
        GlobalVariable* globalArr = llvmStructs.theModule->getGlobalVariable((const char*)m_name.c_str());
        if (globalArr) {
            Type* type = globalArr->getValueType();
            ArrayType* arrType = dyn_cast<ArrayType>(type);
            return arrType->getElementType();
        }
        std::cerr << RED << "Error: Array " << (const char*)(m_name.c_str()) << " is not defined" << RESET << std::endl;
        return nullptr;  
    }
    ArrayType* arrType = dyn_cast<ArrayType>(iter->second.valueType);
    return arrType->getElementType();
}

//===----------------------------------------------------------------------===//
// Printing AST Tree
//===----------------------------------------------------------------------===//

void printIndent(std::ostream& ostr, const std::string& indent, bool isLast) {
    ostr << indent;
    if (isLast) {
        ostr << "└── ";
    } else {
        ostr << "├── ";
    }
}

void BlockAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "BlockAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_instructions.size(); i++) {
        m_instructions[i]->printTree(ostr, newIndent, i == m_instructions.size() - 1);
    }
}

void NumberAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "NumberAST(" << m_value << ")" << std::endl;
}

void CharAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "CharAST('" << (char)m_char << "')" << std::endl;
}

void VariableDeclarationAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "VariableDeclarationAST(" << std::string(m_type.begin(), m_type.end()) << " "
         << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

void VariableReferenceAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "VariableReferenceAST(" << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

void BinaryOperatorAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "BinaryOperatorAST('" << std::string(m_op.begin(), m_op.end()) << "')" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_LHS->printTree(ostr, newIndent, false);
    m_RHS->printTree(ostr, newIndent, true);
}

void FuncCallAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FuncCallAST(" << std::string(m_calleeIdentifier.begin(), m_calleeIdentifier.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(ostr, newIndent, i == m_args.size() - 1);
    }
}

void FunctionPrototypeAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FunctionPrototypeAST(" << std::string(m_returnType.begin(), m_returnType.end()) << " " << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;

    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(ostr, newIndent, i == m_args.size() - 1);
    }
}

void FunctionAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FunctionAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_prototype->printTree(ostr, newIndent, false);
    m_body->printTree(ostr, newIndent, true);
}

void ReturnAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    ostr << "ReturnAST" << std::endl;
    if (m_expr) {
        m_expr->printTree(ostr, newIndent, true);
    }
}

void BreakAST::printTree(std::ostream &ostr, const std::string& indent, bool isLast) const
{
    printIndent(ostr, indent, isLast);
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    ostr << "BreakAST" << std::endl;
}

void IfAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "IfAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_cond->printTree(ostr, newIndent, false);
    m_then->printTree(ostr, newIndent, m_else == nullptr);
    if (m_else) m_else->printTree(ostr, newIndent, true);
}

void LoopAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "LoopAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_body->printTree(ostr, newIndent, true);
}

void ArrayAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "ArrayAST(" << (const char*)(m_type.c_str()) << "[" << m_size << "] " << (const char*)(m_name.c_str()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (const auto& element : m_arrElements) {
        element->printTree(ostr, newIndent, element == *m_arrElements.end());
    }
}

void AccessArrayElementAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "AccessArrayElement(" << std::string(m_name.begin(), m_name.end()) << "[" << m_index << "])" << std::endl;
}
