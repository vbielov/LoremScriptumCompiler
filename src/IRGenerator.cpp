#include "IRGenerator.hpp"

IRGenerator::IRGenerator(const char* moduleID, const std::unique_ptr<AST>& rootBlock) 
    : m_root(rootBlock.get())
    , m_llvmStructs{
        std::make_unique<LLVMContext>(),
        std::make_unique<Module>(moduleID, *m_llvmStructs.theContext),
        std::make_unique<IRBuilder<>>(*m_llvmStructs.theContext),
        std::map<std::string, Value*>(),
        {}
    } {}

void IRGenerator::generateIRCode() {
    m_root->codegen(m_llvmStructs);
}

Module* IRGenerator::getModule() {
    return m_llvmStructs.theModule.get();
}

std::string IRGenerator::getIRCodeString() {
    std::string IRCode;
    llvm::raw_string_ostream outStream(IRCode);
    m_llvmStructs.theModule->print(outStream, nullptr);
    return outStream.str();
}

Type* getVariableType(const std::u8string& typeStr, const LLVMStructs& llvmStructs, bool isPointer) {
    if (!isPointer) {
        if (typeStr == u8"numerus") {
            return Type::getInt32Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"litera") {
            return Type::getInt8Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"nihil") {
            return Type::getVoidTy(*(llvmStructs.theContext));
        } else if (typeStr == u8"asertio") {
            return Type::getInt1Ty(*(llvmStructs.theContext));
        }
    } else {
        if (typeStr == u8"numerus") {
            return PointerType::getInt32Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"litera") {
            return PointerType::getInt8Ty(*(llvmStructs.theContext));
        } else if (typeStr == u8"nihil") {
            return PointerType::getVoidTy(*(llvmStructs.theContext));
        } else if (typeStr == u8"asertio") {
            return PointerType::getInt1Ty(*(llvmStructs.theContext));
        }
    }
}

Value* BlockAST::codegen(LLVMStructs& llvmStructs) {
    std::vector<Value*> values;
    for(auto& node : m_instructions) {
        values.push_back(node->codegen(llvmStructs));
    }
    return nullptr;
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
        Function* theFunction = insertBlock->getParent();
        IRBuilder<> tmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
        // stack allocation
        AllocaInst* alloca = tmpB.CreateAlloca(
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
    if (!value) {
        value = llvmStructs.theModule->getGlobalVariable((const char*)m_name.c_str());
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

    // Assigment
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

    // TODO(Vlad): Add ==, !, ^, ¬

    // Load variables to registers
    if (left->getType()->isPointerTy()) {
        left = llvmStructs.builder->CreateLoad(Type::getInt32Ty(*(llvmStructs.theContext)), left, "loadtmp");
    }
    if (right->getType()->isPointerTy()) {
        right = llvmStructs.builder->CreateLoad(Type::getInt32Ty(*(llvmStructs.theContext)), right, "loadtmp");
    }

    if (m_op == u8">") {
        return llvmStructs.builder->CreateICmpSGT(left, right, "cmptmp");
    } else if (m_op == u8"<") {
        return llvmStructs.builder->CreateICmpSLT(left, right, "cmptmp");
    } else if (m_op == u8">=") {
        return llvmStructs.builder->CreateICmpSGE(left, right, "cmptmp");
    } else if (m_op == u8"<=") {
        return llvmStructs.builder->CreateICmpSLE(left, right, "cmptmp");
    } else if (m_op == u8"+") {
        return llvmStructs.builder->CreateAdd(left, right, "addtmp");
    } else if (m_op == u8"-") {
        return llvmStructs.builder->CreateSub(left, right, "subtmp");
    } else if (m_op == u8"*") {
        return llvmStructs.builder->CreateMul(left, right, "multmp");
    } else if (m_op == u8"/") {
        return llvmStructs.builder->CreateSDiv(left, right, "divtmp");
    } else if (m_op == u8"%") {
        return llvmStructs.builder->CreateSRem(left, right, "modtmp");
    }

    std::cerr << RED << "Error: Invalid binary operator" << RESET << std::endl;
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
            Function* theFunction = llvmStructs.builder->GetInsertBlock()->getParent();
            IRBuilder<> tmpB(&theFunction->getEntryBlock(), theFunction->getEntryBlock().begin());
            AllocaInst* alloca = tmpB.CreateAlloca(value->getType(), nullptr, "tempVar");
            llvmStructs.namedValues["tempVar"] = alloca;

            llvmStructs.builder->CreateStore(value, alloca);
            value = alloca;
        }
        argsV.push_back(value);
        if (!argsV.back()) {
            return nullptr;
        }
    }

    // don't store result of a void function
    if (calleeF->getReturnType() == Type::getVoidTy(*(llvmStructs.theContext))) {
        llvmStructs.builder->CreateCall(calleeF, argsV);
        return nullptr;
    }

    return llvmStructs.builder->CreateCall(calleeF, argsV, "calltmp");
}

Value* FunctionPrototypeAST::codegen(LLVMStructs& llvmStructs) {    
    std::vector<Type*> argTypes;
    argTypes.reserve(m_args.size());
    for (const auto& arg : m_args) {
        argTypes.push_back(PointerType::get(getVariableType(arg->getType(), llvmStructs, false), 0));
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

    m_body->codegen(llvmStructs);
    
    // NOTE(Vlad): If you do this, then user has no ability to create custom return, 
    // because terminator will go in the middle of a basic block.
    // if(m_prototype->getReturnType() == u8"nihil") {
        // llvmStructs.builder->CreateRetVoid(); // for some reason functions with void type NEED to have a return
    // }

    if(verifyFunction(*func, &(llvm::errs()))) {
        // We had error reading the body => remove function
        func->eraseFromParent();
        return nullptr;
    }

    return func;
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
    Value* condition = m_cond->codegen(llvmStructs);
    if (!condition) {
        return nullptr;
    }

    condition = llvmStructs.builder->CreateICmpNE(condition, ConstantInt::get(*(llvmStructs.theContext), APInt(1, 0, false)), "ifcond");

    Function* function = llvmStructs.builder->GetInsertBlock()->getParent();
    BasicBlock* thenBlock = BasicBlock::Create(*(llvmStructs.theContext), "then", function);
    BasicBlock* elseBlock = BasicBlock::Create(*(llvmStructs.theContext), "else");
    BasicBlock* mergeBlock = BasicBlock::Create(*(llvmStructs.theContext), "ifcont");

    llvmStructs.builder->CreateCondBr(condition, thenBlock, elseBlock);

    // Then
    llvmStructs.builder->SetInsertPoint(thenBlock);

    Value* thenValue = m_then->codegen(llvmStructs);

    // NOTE(Vlad): I don't know if it's legal.
    if (!llvmStructs.builder->GetInsertBlock()->getTerminator()) {
        llvmStructs.builder->CreateBr(mergeBlock);
    }
    thenBlock = llvmStructs.builder->GetInsertBlock();

    // Else
    function->insert(function->end(), elseBlock);
    llvmStructs.builder->SetInsertPoint(elseBlock);
    
    Value* elseValue = m_else->codegen(llvmStructs);

    // NOTE(Vlad): I don't know if it's legal.
    if (!llvmStructs.builder->GetInsertBlock()->getTerminator()) {
        llvmStructs.builder->CreateBr(mergeBlock);
    }

    elseBlock = llvmStructs.builder->GetInsertBlock();

    // Merge
    function->insert(function->end(), mergeBlock);
    llvmStructs.builder->SetInsertPoint(mergeBlock);
    return nullptr;
}

Value* BreakAST::codegen(LLVMStructs& llvmStructs) {
    BasicBlock* returnBlock = llvmStructs.afterLoop.top();
    if (!returnBlock) {
        std::cerr << RED << "Error: there is no point for break to return" << RESET << std::endl;
        return nullptr;
    }
    llvmStructs.builder->CreateBr(returnBlock);
    return nullptr;
}

Value* LoopAST::codegen(LLVMStructs& llvmStructs) {
    Function* function = llvmStructs.builder->GetInsertBlock()->getParent();
    BasicBlock* loopBlock = BasicBlock::Create(*(llvmStructs.theContext), "loop", function);
    BasicBlock* afterBlock = BasicBlock::Create(*(llvmStructs.theContext), "afterLoop");
    llvmStructs.afterLoop.push(afterBlock);

    llvmStructs.builder->CreateBr(loopBlock);
    llvmStructs.builder->SetInsertPoint(loopBlock);

    m_body->codegen(llvmStructs);
    llvmStructs.builder->CreateBr(loopBlock);
    loopBlock = llvmStructs.builder->GetInsertBlock(); // do I have to do this?

    function->insert(function->end(), afterBlock);
    llvmStructs.builder->SetInsertPoint(afterBlock);

    llvmStructs.afterLoop.pop();
    return nullptr;
}

