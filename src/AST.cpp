#include "AST.hpp"

//===----------------------------------------------------------------------===//
// AST
//===----------------------------------------------------------------------===//

BlockAST::BlockAST(std::vector<std::unique_ptr<AST>> instructions)
    : m_instructions(std::move(instructions)) {}

NumberAST::NumberAST(int value) 
    : m_value(value) {}

CharAST::CharAST(char8_t character) 
    : m_char(character) {}

VariableDeclarationAST::VariableDeclarationAST(const std::u8string& type, const std::u8string& name)
    : m_type(type)
    , m_name(name) {}

const std::u8string& VariableDeclarationAST::getName() const {
    return m_name;
}

const std::u8string &VariableDeclarationAST::getType() const {
    return m_type;
}

VariableReferenceAST::VariableReferenceAST(const std::u8string &name)
    : m_name(std::move(name)) {}

BinaryOperatorAST::BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS) 
    : m_op(std::move(op))
    , m_LHS(std::move(LHS))
    , m_RHS(std::move(RHS)) {}

FuncCallAST::FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args)
    : m_calleeIdentifier(std::move(callee))
    , m_args(std::move(args)) {}

FunctionPrototypeAST::FunctionPrototypeAST(const std::u8string& returnType, const std::u8string& name, std::vector<std::unique_ptr<VariableDeclarationAST>> args)
    : m_returnType(std::move(returnType))
    , m_name(std::move(name))
    , m_args(std::move(args)) {}

const std::u8string& FunctionPrototypeAST::getName() const {
    return m_name;
}

FunctionAST::FunctionAST(std::unique_ptr<FunctionPrototypeAST> prototype, std::unique_ptr<BlockAST> body)
    : m_prototype(std::move(prototype))
    , m_body(std::move(body)) {}

ReturnAST::ReturnAST(std::unique_ptr<AST> expr) 
    : m_expr(std::move(expr)) {}

IfAST::IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else)
    : m_cond(std::move(cond))
    , m_then(std::move(then))
    , m_else(std::move(_else)) {}

ForAST::ForAST(const std::u8string& varName, std::unique_ptr<AST> start, std::unique_ptr<AST> end, std::unique_ptr<AST> step, std::unique_ptr<BlockAST> body)
    : m_varName(std::move(varName))
    , m_start(std::move(start))
    , m_end(std::move(end))
    , m_step(std::move(step))
    , m_body(std::move(body)) {}

//===----------------------------------------------------------------------===//
// Printing AST Tree
//===----------------------------------------------------------------------===//

void printIndent(const std::string &indent, bool isLast) {
    std::cout << indent;
    if (isLast) {
        std::cout << "└── ";
    } else {
        std::cout << "├── ";
    }
}

void BlockAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "BlockAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_instructions.size(); i++) {
        m_instructions[i]->printTree(newIndent, i == m_instructions.size() - 1);
    }
}

void NumberAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "NumberAST(" << m_value << ")" << std::endl;
}

void CharAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "CharAST('" << (char)m_char << "')" << std::endl;
}

void VariableDeclarationAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "VariableDeclarationAST(" << std::string(m_type.begin(), m_type.end()) << " "
              << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

void VariableReferenceAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "VariableReferenceAST(" << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;
}

void BinaryOperatorAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "BinaryOperatorAST('" << std::string(m_op.begin(), m_op.end()) << "')" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_LHS->printTree(newIndent, false);
    m_RHS->printTree(newIndent, true);
}

void FuncCallAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "FuncCallAST(" << std::string(m_calleeIdentifier.begin(), m_calleeIdentifier.end()) << ")" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(newIndent, i == m_args.size() - 1);
    }
}

void FunctionPrototypeAST::printTree(const std::string &indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "FunctionPrototypeAST(" << std::string(m_returnType.begin(), m_returnType.end()) << " " << std::string(m_name.begin(), m_name.end()) << ")" << std::endl;

    std::string newIndent = indent + (isLast ? "    " : "│   ");
    for (size_t i = 0; i < m_args.size(); i++) {
        m_args[i]->printTree(newIndent, i == m_args.size() - 1);
    }
}

void FunctionAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "FunctionAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_prototype->printTree(newIndent, false);
    m_body->printTree(newIndent, true);
}

void ReturnAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "ReturnAST" << std::endl;
    if (m_expr) {
        std::string newIndent = indent + (isLast ? "    " : "│   ");
        m_expr->printTree(newIndent, true);
    }
}

void IfAST::printTree(const std::string& indent, bool isLast) const {
    printIndent(indent, isLast);
    std::cout << "IfAST" << std::endl;
    std::string newIndent = indent + (isLast ? "    " : "│   ");
    m_cond->printTree(newIndent, false);
    m_then->printTree(newIndent, m_else == nullptr);
    if (m_else) m_else->printTree(newIndent, true);
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

//===----------------------------------------------------------------------===//
// Code Generation 
//===----------------------------------------------------------------------===//

llvm::Type* getVariableType(const std::u8string& typeStr, const LLVMStructs& llvmStructs) {
    if (typeStr == u8"numerus") {
        return llvm::Type::getInt32Ty(*(llvmStructs.theContext));
    } else if (typeStr == u8"littera") {
        return llvm::Type::getInt8Ty(*(llvmStructs.theContext));
    } else if (typeStr == u8"nihil") {
        return llvm::Type::getVoidTy(*(llvmStructs.theContext));
    }
}

Value* BlockAST::codegen(LLVMStructs& llvmStructs) {
    std::vector<Value*> values;
    for(auto& node : m_instructions) {
        values.push_back(node->codegen(llvmStructs));
    }
    return values.back();
}

Value* NumberAST::codegen(LLVMStructs& llvmStructs) {
    // signed 32bit integer
    return ConstantInt::get(*llvmStructs.theContext, APInt(32, m_value, true));
}

Value* CharAST::codegen(LLVMStructs& llvmStructs) {
    // unsigned 8bit character
    return ConstantInt::get(*llvmStructs.theContext, APInt(8, m_char, false));
}

Value* VariableDeclarationAST::codegen(LLVMStructs& llvmStructs) {
    // get scope
    llvm::Function* theFunction = llvmStructs.builder->GetInsertBlock()->getParent();
    llvm::IRBuilder<> tmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());

    // stack allocation
    llvm::AllocaInst* alloca = tmpB.CreateAlloca(
        getVariableType(m_type, llvmStructs), nullptr, (const char*)(m_name.c_str())
    );

    // store name in name table
    llvmStructs.namedValues[(const char*)(m_name.c_str())] = alloca;
    return alloca;
}

Value* VariableReferenceAST::codegen(LLVMStructs& llvmStructs) {
    std::string str = (const char*)m_name.c_str();
    Value* value = llvmStructs.namedValues[str];
    // reference by value, not a pointer
    return llvmStructs.builder->CreateLoad(getVariableType(u8"numerus", llvmStructs), value, "loadtmp");

    if (!value) {
        std::cerr << RED << "Error: Unknown variable name" << RESET << std::endl;
    }
    return nullptr;
}

Value* BinaryOperatorAST::codegen(LLVMStructs& llvmStructs) {
    Value* left = m_LHS->codegen(llvmStructs);
    Value* right = m_RHS->codegen(llvmStructs);
    if (!left || !right) {
        return nullptr;
    }

    // TODO: Add more types, operators
    if (m_op == u8"+") {
        return llvmStructs.builder->CreateAdd(left, right, "addtmp");
    } else if (m_op == u8"-") {
        return llvmStructs.builder->CreateSub(left, right, "subtmp");
    } else if (m_op == u8"*") {
        return llvmStructs.builder->CreateMul(left, right, "multmp");
    } else if (m_op == u8"<") {
        return llvmStructs.builder->CreateICmpULT(left, right, "cmptmp");
    } else if (m_op == u8"=") {
        llvmStructs.builder->CreateStore(right, left); // because left is a variable
    } else {
       std::cerr << RED << "Error: Invalid binary operator" << RESET << std::endl;
    }
    return nullptr;
}

Value* FuncCallAST::codegen(LLVMStructs& llvmStructs) {
    Function* calleeF = llvmStructs.theModule->getFunction((const char*)(m_calleeIdentifier.c_str()));
    if (!calleeF) {
        std::cerr << RED << "Error: Uknown function referenced" << RESET << std::endl;
        return nullptr;
    }

    if (calleeF->arg_size() != m_args.size()) {
        std::cerr << RED << "Error: Incorrect number of arguments passed" << RESET << std::endl;
        return nullptr;
    }

    // TODO: Check if types are right

    std::vector<Value*> argsV;
    for (auto& arg : m_args) {
        argsV.push_back(arg->codegen(llvmStructs));
        if (!argsV.back()) {
            return nullptr;
        }
    }

    return llvmStructs.builder->CreateCall(calleeF, argsV, "calltmp");
}

Value* FunctionPrototypeAST::codegen(LLVMStructs& llvmStructs) {    
    std::vector<Type*> argTypes;
    argTypes.reserve(m_args.size());
    for (const auto& arg : m_args) {
        argTypes.push_back(getVariableType(arg->getType(), llvmStructs));
    }     

    FunctionType* funcType = FunctionType::get(getVariableType(m_returnType, llvmStructs), argTypes, false);
    Function* func = Function::Create(funcType, Function::ExternalLinkage, (const char*)m_name.c_str(), llvmStructs.theModule.get());
 
    int index = 0;
    for(auto& arg : func->args()) {
        arg.setName((const char*)(m_args[index++]->getName().c_str()));
    }

    return func;
}

Value* FunctionAST::codegen(LLVMStructs& llvmStructs) {
    // check for an existing function from imports
    const char* funcName = (const char*)(m_prototype->getName().c_str());
    Function* func = llvmStructs.theModule->getFunction(funcName);

    if (!func) {
        func = (Function*)m_prototype->codegen(llvmStructs);
    }

    if (!func) {
        return nullptr;
    }

    if (!func->empty()) {
        std::cerr << RED << "Error: Function cannot be redefined" << RESET << std::endl;
        return nullptr;
    }

    // Create block to start insertion into.
    BasicBlock* block = BasicBlock::Create(*(llvmStructs.theContext), "entry", func);
    llvmStructs.builder->SetInsertPoint(block);

    // Record the function arguments in the namedValues map
    llvmStructs.namedValues.clear();
    for (auto& arg : func->args()) {
        llvmStructs.namedValues[std::string(arg.getName())] = &arg;
    }

    if (Value* retVal = m_body->codegen(llvmStructs)) {
        verifyFunction(*func);
        return func;
    }

    // We had error reading the body => remove function
    func->eraseFromParent();
    return nullptr;
}

Value* ReturnAST::codegen(LLVMStructs& llvmStructs) {
    if (m_expr) {
        auto v = m_expr->codegen(llvmStructs);
        if (v) {
            return llvmStructs.builder->CreateRet(v);
        }
        return nullptr;
    }
    return llvmStructs.builder->CreateRetVoid();
}

Value* IfAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}

Value* ForAST::codegen(LLVMStructs& llvmStructs) {
    return nullptr;
}
