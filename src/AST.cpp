#include "AST.hpp"

//===----------------------------------------------------------------------===//
// AST
//===----------------------------------------------------------------------===//

const std::u8string& AST::getName() const {
    assert(false && "This AST can't have a name.");
    static std::u8string ilegal = u8"ilegal";
    return ilegal;
}

const IDataType* AST::getType() const {
    assert(false && "This AST can't have a type.");
    return nullptr;
}

BlockAST::BlockAST(std::vector<std::unique_ptr<AST>> instructions)
    : m_instructions(std::move(instructions)) {}

NumberAST::NumberAST(int value) 
    : m_value(value) {}

const IDataType* NumberAST::getType() const {
    static const std::unique_ptr<PrimitiveDataType> TYPE = std::make_unique<PrimitiveDataType>(PrimitiveType::INT);
    return TYPE.get();
}

CharAST::CharAST(char8_t character) 
    : m_char(character) {}

const IDataType* CharAST::getType() const {
    static const std::unique_ptr<PrimitiveDataType> TYPE = std::make_unique<PrimitiveDataType>(PrimitiveType::CHAR);
    return TYPE.get();
}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& name, std::unique_ptr<PrimitiveDataType> type)
    : m_name(name)
    , m_type(std::move(type)) {}

const std::u8string& VariableDeclarationAST::getName() const {
    return m_name;
}

const IDataType* VariableDeclarationAST::getType() const {
    return m_type.get();
}

VariableReferenceAST::VariableReferenceAST(const std::u8string& name)
    : m_name(std::move(name)) {}

const std::u8string& VariableReferenceAST::getName() const {
    return m_name;
}

const IDataType* VariableReferenceAST::getType() const {
    // TODO(Vlad): when syntax table is done 
    return nullptr;
}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS) 
    : m_op(op)
    , m_LHS(std::move(LHS))
    , m_RHS(std::move(RHS)) {}

const IDataType* BinaryOperatorAST::getType() const {
    const IDataType* leftType = m_LHS->getType();
    return leftType;
}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args)
    : m_calleeIdentifier(std::move(callee))
    , m_args(std::move(args)) {}

const std::u8string& FuncCallAST::getName() const {
    return m_calleeIdentifier;
}

const IDataType* FuncCallAST::getType() const {
    // TODO(Vlad): when syntax table is done
    return nullptr;
}

FunctionPrototypeAST::FunctionPrototypeAST(const std::u8string& name, std::unique_ptr<IDataType> returnType, std::vector<std::unique_ptr<AST>> args)
    : m_name(std::move(name))
    , m_returnType(std::move(returnType))
    , m_args(std::move(args)) {}

const std::u8string& FunctionPrototypeAST::getName() const {
    return m_name;
}

const IDataType* FunctionPrototypeAST::getType() const {
    return m_returnType.get();
}

const std::vector<std::unique_ptr<AST>>& FunctionPrototypeAST::getArgs() const {
    return m_args;
}

FunctionAST::FunctionAST(std::unique_ptr<FunctionPrototypeAST> prototype, std::unique_ptr<BlockAST> body)
    : m_prototype(std::move(prototype))
    , m_body(std::move(body)) {}

const std::u8string& FunctionAST::getName() const {
    return m_prototype->getName();
}

const IDataType* FunctionAST::getType() const {
    return m_prototype->getType();
}

ReturnAST::ReturnAST(std::unique_ptr<AST> expr) 
    : m_expr(std::move(expr)) {}

const IDataType* ReturnAST::getType() const {
    // TODO(Vlad): when syntax table is done
    return nullptr;
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else)
    : m_cond(std::move(cond))
    , m_then(std::move(then))
    , m_else(std::move(_else)) {}

BreakAST::BreakAST() {}

LoopAST::LoopAST(std::unique_ptr<BlockAST> body) 
    : m_body(std::move(body)) {}

ArrayDeclarationAST::ArrayDeclarationAST(const std::u8string& name, std::unique_ptr<ArrayDataType> type)
    : m_name(name)
    , m_type(std::move(type))
    , m_arrElements() {}


ArrayDeclarationAST::ArrayDeclarationAST(const std::u8string& name, std::unique_ptr<ArrayDataType> type, std::vector<std::unique_ptr<AST>> arrElements)
    : m_name(name)
    , m_type(std::move(type))
    , m_arrElements(std::move(arrElements)) {}


const IDataType* ArrayDeclarationAST::getType() const {
    return m_type.get();
}

AccessArrayElementAST::AccessArrayElementAST(const std::u8string& name, std::unique_ptr<AST> index)
    : m_name(name)
    , m_index(std::move(index)) {}

const std::u8string& AccessArrayElementAST::getName() const {
    return m_name;
}

const IDataType* AccessArrayElementAST::getType() const {
    // TODO(Vlad): when syntax table is done
    return nullptr;
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
    ostr << "VariableDeclarationAST(" << (const char*)(std::u8string(types::VALUES[(int)m_type->type]).c_str()) << " "
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
    ostr << "FunctionPrototypeAST(" << "TODO: returnType" << " " << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;

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

void ArrayDeclarationAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "ArrayAST(" << (const char*)(std::u8string(types::VALUES[(int)m_type->type]).c_str()) << "[" << m_type->size << "] " << (const char*)(m_name.c_str()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (const auto& element : m_arrElements) {
        element->printTree(ostr, newIndent, element == *m_arrElements.end());
    }
}

void AccessArrayElementAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "AccessArrayElement(" << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_index->printTree(ostr, newIndent, true);
}

