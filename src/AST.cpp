#include "AST.hpp"

//===----------------------------------------------------------------------===//
// AST
//===----------------------------------------------------------------------===//

const std::u8string& AST::getName() const {
    assert(false && "This AST can't have a name.");
    static const std::u8string ilegal = u8"ilegal";
    return ilegal;
}

const IDataType* AST::getType([[maybe_unused]] const IRContext& context) {
    assert(false && "This AST can't have a type.");
    return nullptr;
}


 BlockAST::BlockAST(std::vector<std::unique_ptr<AST>> instructions, size_t line)
    :m_instructions(std::move(instructions)), line(line) {}


NumberAST::NumberAST(int value, size_t line) 
    :m_value(value), line(line) {}

const IDataType* NumberAST::getType([[maybe_unused]] const IRContext& context) {
    static const std::unique_ptr<IDataType> TYPE = std::make_unique<PrimitiveDataType>(PrimitiveType::INT);
    return TYPE.get();
}

CharAST::CharAST(char8_t character, size_t line) 
    :m_char(character), line(line) {}

const IDataType* CharAST::getType([[maybe_unused]] const IRContext& context) {
    static const std::unique_ptr<IDataType> TYPE = std::make_unique<PrimitiveDataType>(PrimitiveType::CHAR);
    return TYPE.get();
}

BoolAST::BoolAST(bool boolean, size_t line)
    :m_bool(boolean), line(line) {}

const IDataType* BoolAST::getType([[maybe_unused]] const IRContext& context) {
    static const std::unique_ptr<IDataType> TYPE = std::make_unique<PrimitiveDataType>(PrimitiveType::BOOL);
    return TYPE.get();
}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& name, std::unique_ptr<IDataType> type, size_t line)
    :m_name(name), m_type(std::move(type)), line(line) {}

const std::u8string& VariableDeclarationAST::getName() const {
    return m_name;
}

const IDataType* VariableDeclarationAST::getType([[maybe_unused]] const IRContext& context) {
    return m_type.get();
}

VariableReferenceAST::VariableReferenceAST(const std::u8string& name, size_t line)
    :m_name(std::move(name)), line(line) {}

const std::u8string& VariableReferenceAST::getName() const {
    return m_name;
}

const IDataType* VariableReferenceAST::getType(const IRContext& context) {
    return context.symbolTable.lookupVariable(m_name)->type;
}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS, size_t line) 
    :m_op(op)
    , m_LHS(std::move(LHS))
    , m_RHS(std::move(RHS)), line(line) {}

const IDataType* BinaryOperatorAST::getType([[maybe_unused]] const IRContext& context) {
    return m_LHS->getType(context);
}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args, size_t line)
    :m_calleeIdentifier(std::move(callee))
    , m_args(std::move(args)), line(line) {}

const std::u8string& FuncCallAST::getName() const {
    return m_calleeIdentifier;
}

const IDataType* FuncCallAST::getType(const IRContext& context) {
    return context.symbolTable.lookupFunction(m_calleeIdentifier)->type;
}

FunctionPrototypeAST::FunctionPrototypeAST(const std::u8string& name, std::unique_ptr<IDataType> returnType, std::vector<std::unique_ptr<TypeIdentifierPair>> args, bool isDefined, size_t line)
    : m_name(std::move(name))
    , m_returnType(std::move(returnType))
    , m_args(std::move(args))
    , m_isDefined(isDefined) , line(line){}

const std::u8string& FunctionPrototypeAST::getName() const {
    return m_name;
}

const IDataType* FunctionPrototypeAST::getType([[maybe_unused]] const IRContext& context) {
    return m_returnType.get();
}

const std::vector<std::unique_ptr<TypeIdentifierPair>>& FunctionPrototypeAST::getArgs() const {
    return m_args;
}

bool FunctionPrototypeAST::isDefined() const {
    return m_isDefined;
}

FunctionAST::FunctionAST(std::unique_ptr<FunctionPrototypeAST> prototype, std::unique_ptr<BlockAST> body, size_t line)
    : m_prototype(std::move(prototype))
    , m_body(std::move(body)) , line(line){}

const std::u8string& FunctionAST::getName() const {
    return m_prototype->getName();
}

const IDataType* FunctionAST::getType(const IRContext& context) {
    return m_prototype->getType(context);
}

ReturnAST::ReturnAST(std::unique_ptr<AST> expr, size_t line) 
    : m_expr(std::move(expr)), line(line) {}

const IDataType* ReturnAST::getType(const IRContext& context) {
    return m_expr->getType(context);
}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else, size_t line)
    : m_cond(std::move(cond))
    , m_then(std::move(then))
    , m_else(std::move(_else)), line(line) {}

BreakAST::BreakAST(size_t line)
    : line(line) {}

LoopAST::LoopAST(std::unique_ptr<BlockAST> body, size_t line) 
    : m_body(std::move(body)), line(line) {}
    

ArrayInitializationAST::ArrayInitializationAST(const std::u8string& name, std::vector<std::unique_ptr<AST>> elements, size_t line)
    : m_name(name)
    , m_elements(std::move(elements)), line(line) {}

AccessArrayElementAST::AccessArrayElementAST(const std::u8string& name, std::unique_ptr<AST> index, size_t line)
    : m_name(name)
    , m_index(std::move(index))
    , m_type(nullptr) , line(line){}

const std::u8string& AccessArrayElementAST::getName() const {
    return m_name;
}

const IDataType* AccessArrayElementAST::getType(const IRContext& context) {
    const ScopeEntry* entry = context.symbolTable.lookupVariable(m_name);
    if (!entry)
        return nullptr;

    const StructDataType* structType = dynamic_cast<const StructDataType*>(entry->type);
    if (structType && s_structsTypeMap.find(structType->name) != s_structsTypeMap.end()) {
        VariableReferenceAST* ref = dynamic_cast<VariableReferenceAST*>(m_index.get());
        if (!ref) {
            return nullptr;
        }
        for (const auto& attr : s_structsTypeMap[structType->name]->attributes) {
            if (attr.identifier == ref->getName())
                return attr.type.get();
        }
        // TODO(Vlad): Error
        return nullptr;
    }
 
    if (!m_type) {
        const ArrayDataType* arrType = dynamic_cast<const ArrayDataType*>(entry->type);
        m_type = std::make_unique<PrimitiveDataType>(arrType->type);
    }

    return m_type.get();
}

StructAST::StructAST(std::unique_ptr<StructDataType> type, size_t line)
    : m_type(std::move(type)) , line(line){}

const std::u8string& StructAST::getName() const {
    return m_type->name;
}

const IDataType* StructAST::getType([[maybe_unused]] const IRContext &context) {
    return m_type.get();
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

size_t BlockAST::getLine() const
{
    return line;
}

void NumberAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "NumberAST(" << m_value << ")" << std::endl;
}

size_t NumberAST::getLine() const
{
    return line;
}

void CharAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "CharAST('" << (char)m_char << "')" << std::endl;
}

size_t CharAST::getLine() const
{
    return line;
}

size_t VariableReferenceAST::getLine() const
{
    return line;
}
void VariableDeclarationAST::printTree(std::ostream &ostr, const std::string &indent, bool isLast) const
{
    printIndent(ostr, indent, isLast);
    ostr << "VariableDeclarationAST(" << (const char*)m_type->toString().c_str() << " "
         << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

size_t VariableDeclarationAST::getLine() const
{
    return line;
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

size_t BinaryOperatorAST::getLine() const
{
    return line;
}

void FuncCallAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FuncCallAST(" << std::string(m_calleeIdentifier.begin(), m_calleeIdentifier.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(ostr, newIndent, i == m_args.size() - 1);
    }
}

size_t FuncCallAST::getLine() const
{
    return line;
}

void FunctionPrototypeAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr    << "FunctionPrototypeAST(" << (const char*)m_returnType->toString().c_str() << " " 
            << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;

    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        printIndent(ostr, newIndent, i == m_args.size() - 1);
        ostr << (const char*)m_args[i]->type->toString().c_str() << " " << (const char*)m_args[i]->identifier.c_str() << std::endl;
    }
}

size_t FunctionPrototypeAST::getLine() const
{
    return line;
}

void FunctionAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "FunctionAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_prototype->printTree(ostr, newIndent, false);
    m_body->printTree(ostr, newIndent, true);
}

size_t FunctionAST::getLine() const
{
    return line;
}

void ReturnAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    ostr << "ReturnAST" << std::endl;
    if (m_expr) m_expr->printTree(ostr, newIndent, true);
}

size_t ReturnAST::getLine() const
{
    return line;
}

void BreakAST::printTree(std::ostream &ostr, const std::string& indent, bool isLast) const
{
    printIndent(ostr, indent, isLast);
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    ostr << "BreakAST" << std::endl;
}

size_t BreakAST::getLine() const
{
    return line;
}

void IfAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "IfAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_cond->printTree(ostr, newIndent, false);
    m_then->printTree(ostr, newIndent, m_else == nullptr);
    if (m_else) m_else->printTree(ostr, newIndent, true);
}

size_t IfAST::getLine() const
{
    return line;
}

void LoopAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "LoopAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_body->printTree(ostr, newIndent, true);
}

size_t LoopAST::getLine() const
{
    return line;
}

void AccessArrayElementAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "AccessArrayElementAST(" << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_index->printTree(ostr, newIndent, true);
}

size_t AccessArrayElementAST::getLine() const
{
    return line;
}

void ArrayInitializationAST::printTree(std::ostream& ostr, const std::string& indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "ArrayInitializationAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (const auto& element : m_elements) {
        element->printTree(ostr, newIndent, element == m_elements.back());
    }
}

size_t ArrayInitializationAST::getLine() const
{
    return line;
}

void BoolAST::printTree(std::ostream &ostr, const std::string &indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "BoolAST(" << (m_bool ? "true" : "false") << ")" << std::endl;
}

size_t BoolAST::getLine() const
{
    return line;
}

void StructAST::printTree(std::ostream &ostr, const std::string &indent, bool isLast) const {
    printIndent(ostr, indent, isLast);
    ostr << "StructAST(" << (const char*)getName().c_str() << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (const auto& attr : m_type->attributes) {
        printIndent(ostr, newIndent, &attr == &m_type->attributes.back());
        ostr << (const char*)attr.type->toString().c_str() << " " << (const char*)attr.identifier.c_str() << std::endl;
    }
}

size_t StructAST::getLine() const
{
    return line;
}
