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
    , m_name(name)
    , m_isGlobal(false) {}

const std::u8string& VariableDeclarationAST::getName() const {
    return m_name;
}

const std::u8string &VariableDeclarationAST::getType() const {
    return m_type;
}

bool VariableDeclarationAST::isGlobal() const {
    return m_isGlobal;
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

llvm::Type* getVariableType(const std::u8string& typeStr, const LLVMStructs& llvmStructs, bool isPointer) {
    if (!isPointer) {
        if (typeStr == u8"numerus") {
            return llvm::Type::getInt32Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"littera") {
            return llvm::Type::getInt8Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"nihil") {
            return llvm::Type::getVoidTy(*(llvmStructs.theContext));
        }
    } else {
        if (typeStr == u8"numerus") {
            return llvm::PointerType::getInt32Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"littera") {
            return llvm::PointerType::getInt8Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"nihil") {
            return llvm::PointerType::getVoidTy(*(llvmStructs.theContext));
        }
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
    Type* type = getVariableType(m_type, llvmStructs, false);
    // get scope
    auto insertBlock = llvmStructs.builder->GetInsertBlock();
    if (insertBlock != nullptr) {
        // it's a stack allocated variable
        llvm::Function* theFunction = insertBlock->getParent();
        llvm::IRBuilder<> tmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
        // stack allocation
        llvm::AllocaInst* alloca = tmpB.CreateAlloca(
            type, nullptr, (const char*)(m_name.c_str())
        );
        // store name in name table
        llvmStructs.namedValues[(const char*)(m_name.c_str())] = alloca;
        return alloca;
    }
    // it's a global variable
    // NOTE(Vlad): why the hell it's "new", do I have to delete it?
    GlobalVariable* globalVariable = new GlobalVariable(
        *llvmStructs.theModule, 
        type, 
        false, 
        GlobalValue::WeakAnyLinkage, 
        ConstantPointerNull::get(PointerType::get(type, 0)), 
        (const char*)(m_name.c_str())
    );
    llvmStructs.namedValues[(const char*)(m_name.c_str())] = globalVariable;
    m_isGlobal = true;
    return globalVariable;
}

Value* VariableReferenceAST::codegen(LLVMStructs& llvmStructs) {
    Value* value = llvmStructs.namedValues[(const char*)m_name.c_str()];
    bool isGlobal = false;
    if (!value) {
        value = llvmStructs.theModule->getGlobalVariable((const char*)m_name.c_str());
        isGlobal = true;
    }
    if (!value) {
        std::cerr << RED << "Error: Unknown variable name: " << (const char*)(m_name.c_str()) << RESET << std::endl;
        return nullptr;
    }

    // reference by a pointer
    return value;
}

Value* BinaryOperatorAST::codegen(LLVMStructs& llvmStructs) {
    Value* left = m_LHS->codegen(llvmStructs);
    Value* right = m_RHS->codegen(llvmStructs);
    if (!left || !right) {
        return nullptr;
    }

    if(m_op == u8"=") {
       auto insertBlock = llvmStructs.builder->GetInsertBlock();
        if (insertBlock != nullptr) {
            llvmStructs.builder->CreateStore(right, left); // because left is a variable
            return nullptr;
        }
        // it must be a global assigment
        if (dynamic_cast<VariableDeclarationAST*>(m_LHS.get()) || dynamic_cast<VariableReferenceAST*>(m_LHS.get())) {
            ((GlobalVariable*)left)->setInitializer((Constant*)right); // how the fuck do I check if right side is a constant...
        }
        return nullptr;
    }

    // Load variables to registers
    if (left->getType()->isPointerTy()) {
        left = llvmStructs.builder->CreateLoad(Type::getInt32Ty(*(llvmStructs.theContext)), left, "loadtmp");
    }
    if (right->getType()->isPointerTy()) {
        right = llvmStructs.builder->CreateLoad(Type::getInt32Ty(*(llvmStructs.theContext)), right, "loadtmp");
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
        Value* value = arg->codegen(llvmStructs);
        if (!(value->getType()->isPointerTy())) {
            // it's a stack allocated temporary variable to call the function
            llvm::Function* theFunction = llvmStructs.builder->GetInsertBlock()->getParent();
            llvm::IRBuilder<> tmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
            llvm::AllocaInst* alloca = tmpB.CreateAlloca(value->getType(), nullptr, "tempVar");
            llvmStructs.namedValues["tempVar"] = alloca;

            llvmStructs.builder->CreateStore(value, alloca);
            value = alloca;
        }
        argsV.push_back(value);
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
        argTypes.push_back(PointerType::get(IntegerType::get(llvmStructs.theModule->getContext(), 32), 0));
    }     

    FunctionType* funcType = FunctionType::get(getVariableType(m_returnType, llvmStructs, false), argTypes, false);
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
        auto value = m_expr->codegen(llvmStructs);
        // if it's a pointer, load it's value
        if (dynamic_cast<VariableReferenceAST*>(m_expr.get())) {
            // TODO(Vlad): Somehow determine elementary type of the value, because it's a pointer.
            value = llvmStructs.builder->CreateLoad(Type::getInt32Ty(*(llvmStructs.theContext)), value, "loadtmp");
        }
        if (value) {
            return llvmStructs.builder->CreateRet(value);
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
