#include "AST.hpp"
#include "IRContext.hpp"
#include "ErrorHandler.hpp"

llvm::Value* BlockAST::codegen(IRContext& context) {
    context.symbolTable.enterScope();
    for (auto& node : m_instructions) {
        node->codegen(context);
    }
    context.symbolTable.exitScope();
    return nullptr;
}

const char* cStr(const std::u8string& str) {
    return (const char*)(str.c_str());
}

llvm::Value* NumberAST::codegen(IRContext& context) {
    // signed 32bit integer
    return llvm::ConstantInt::get(*context.context, llvm::APInt(32, m_value, true));
}

llvm::Value* CharAST::codegen(IRContext& context) {
    // unsigned 8bit character
    return llvm::ConstantInt::get(*context.context, llvm::APInt(8, m_char, false));
}

llvm::Value* BoolAST::codegen(IRContext& context) {
    // unsigned 1bit boolean
    return llvm::ConstantInt::get(*context.context, llvm::APInt(1, m_bool, false));
}

llvm::Value* ArrayAST::codegen(IRContext &context) {
    return nullptr;
}

llvm::Value* VariableDeclarationAST::codegen(IRContext& context) {
    llvm::BasicBlock* insertBlock = context.builder->GetInsertBlock();
    llvm::Type* type = m_type->getLLVMType(*context.context);
    
    if (insertBlock) {
        // stack allocated
        llvm::IRBuilder<> tmpBuilder(insertBlock, insertBlock->begin());
        llvm::AllocaInst* stackVariable = tmpBuilder.CreateAlloca(type, nullptr, cStr(m_name));
        context.symbolTable.addVariable(m_name, m_type.get(), stackVariable);
        return stackVariable;
    }
    // global
    llvm::GlobalVariable* globalVariable = new llvm::GlobalVariable(
        *context.theModule, 
        type, 
        false, 
        llvm::GlobalValue::WeakAnyLinkage,
        llvm::ConstantPointerNull::get(llvm::PointerType::get(type, 0)),
        cStr(m_name)
    );
    context.symbolTable.addGlobal(m_name, m_type.get(), globalVariable);
    return globalVariable;
}

llvm::Value* VariableReferenceAST::codegen(IRContext& context) {
    auto entry = context.symbolTable.lookupVariable(m_name);
    if (entry)
        return entry->value;

    logError(m_line, u8"Syntax Error: variable '" + m_name + u8"' not defined!");
    return nullptr;
}

llvm::Value* BinaryOperatorAST::codegen(IRContext& context) {
    llvm::Value* left = m_LHS->codegen(context);
    llvm::Value* right = m_RHS->codegen(context);
    if (!left || !right)
        return nullptr;

    if (m_op == operators::ASSIGN) {
        llvm::BasicBlock* insertBlock = context.builder->GetInsertBlock();

        if (insertBlock) {
            // Store instruction
            // Load value from pointer if it's an reference, array indexing, ect.
            llvm::Type* leftType = m_LHS->getType(context)->getLLVMType(*context.context);
            llvm::Type* rightType = m_RHS->getType(context)->getLLVMType(*context.context);
            
            if (right->getType()->isPointerTy())
                right = context.builder->CreateLoad(rightType, right, "loadtmp");
            
            // Cast values if they are both integers 
            if (leftType != rightType) {
                // TODO(Vlad): Error
                if (!leftType->isIntegerTy() || !rightType->isIntegerTy())
                {
                    m_RHS->printTree(std::cout, "", false);
                    logError(m_line, u8"Syntax Error: Type does not match!");
                    return nullptr;
                }

                right = context.builder->CreateIntCast(right, leftType, true, "conv");
            }

            context.builder->CreateStore(right, left);
        
        } else if (auto globalVar = llvm::dyn_cast<llvm::GlobalVariable>(left)) {
            // Global initialization
            auto constant = llvm::dyn_cast<llvm::Constant>(right);
            // TODO(Vlad): Error
            if (!constant) {
                    logError(m_line, u8"Syntax Error: right side isnt a constant!");
                    return nullptr;
                }
            globalVar->setInitializer(constant);
        }
        return nullptr;
    }

    // Load value from pointer if it's an reference, array indexing, ect.
    if (left->getType()->isPointerTy())
        left = context.builder->CreateLoad(m_LHS->getType(context)->getLLVMType(*context.context), left, "loadtmp");
    if (right->getType()->isPointerTy())
        right = context.builder->CreateLoad(m_LHS->getType(context)->getLLVMType(*context.context), right, "loadtmp");

    if (m_op == operators::EQUAL) {
        return context.builder->CreateICmpEQ(left, right, "eqtmp");
    } else if (m_op == operators::NOT_EQUAL) {
        return context.builder->CreateICmpNE(left, right, "neqtmp");
    } else if (m_op == operators::GREATER) {
        return context.builder->CreateICmpSGT(left, right, "cmptmp");
    } else if (m_op == operators::LESSER) {
        return context.builder->CreateICmpSLT(left, right, "cmptmp");
    } else if (m_op == operators::GREATER_OR_EQUAL) {
        return context.builder->CreateICmpSGE(left, right, "cmptmp");
    } else if (m_op == operators::LESSER_OR_EQUAL) {
        return context.builder->CreateICmpSLE(left, right, "cmptmp");
    } else if (m_op == operators::PLUS) {
        return context.builder->CreateAdd(left, right, "addtmp");
    } else if (m_op == operators::MINUS) {
        return context.builder->CreateSub(left, right, "subtmp");
    } else if (m_op == operators::MULTIPLY) {
        return context.builder->CreateMul(left, right, "multmp");
    } else if (m_op == operators::DIVIDE) {
        return context.builder->CreateSDiv(left, right, "divtmp");
    } else if (m_op == operators::MODULO) {
        return context.builder->CreateSRem(left, right, "modtmp");
    } else if (m_op == operators::AND) {
        return context.builder->CreateAnd(left, right, "andtmp");
    } else if (m_op == operators::OR) {
        return context.builder->CreateOr(left, right, "ortmp");
    } else if (m_op == operators::NOT) {
        return context.builder->CreateXor(left, llvm::ConstantInt::getBool(llvm::Type::getInt1Ty(*context.context), true), "negtmp");
    }

    // TODO(Vlad): Error
    logError(m_line, u8"Syntax Error: wrong operator!");

    return nullptr;
}

llvm::Value* FuncCallAST::codegen(IRContext& context) {
    llvm::Function* function = context.theModule->getFunction(cStr(m_calleeIdentifier));
    const ScopeEntry* entry = context.symbolTable.lookupFunction(m_calleeIdentifier);
    // TODO(Vlad): Error
    if (!function)
        return nullptr; 

    llvm::Type* type = entry->type->getLLVMType(*context.context);
    bool hasReturn = !type->isVoidTy();

    bool isExtern = true;
    if(function->arg_size() > 0) {
        const auto& nameLastArg = (function->args().end()-1)->getName();
        isExtern = nameLastArg != RETURN_ARG_NAME;
    }

    if (function->arg_size() != (m_args.size() + (!isExtern && hasReturn ? 1 : 0))) {
        std::cout << cStr(m_calleeIdentifier) << std::endl;
        return nullptr;
    }

    std::vector<llvm::Value*> arguments;
    arguments.reserve(function->arg_size());
    for (auto& arg : m_args) {
        llvm::Value* argValue = arg->codegen(context);
        if (!argValue)
            return nullptr;
        
        // functions arguments are always pointers, not value. => Create local variables if needed
        if (!argValue->getType()->isPointerTy()) {
            llvm::BasicBlock* currentBlock = context.builder->GetInsertBlock();
            llvm::IRBuilder<> tmpBuilder(currentBlock, currentBlock->begin());
            llvm::Type* argType = arg->getType(context)->getLLVMType(*context.context);
            llvm::AllocaInst* stackVariable = tmpBuilder.CreateAlloca(argType, nullptr, "argTmp");
            context.builder->CreateStore(argValue, stackVariable);
            argValue = stackVariable;
        }

        arguments.push_back(argValue);
    }
    
    if (!hasReturn || isExtern) {
        return context.builder->CreateCall(function, arguments);
    }

    // TODO(Vlad): Error
    if (!entry){
        logError(m_line, u8"Syntax Error: wrong operator!");
        return nullptr;
    }
    llvm::BasicBlock* currentBlock = context.builder->GetInsertBlock();
    llvm::IRBuilder<> tmpBuilder(currentBlock, currentBlock->begin());
    llvm::AllocaInst* returnVariable = tmpBuilder.CreateAlloca(type, nullptr, RETURN_ARG_NAME);
    arguments.push_back(returnVariable);
    context.builder->CreateCall(function, arguments);
    return returnVariable;
}

llvm::Value* FunctionPrototypeAST::codegen(IRContext& context) {
    std::vector<llvm::Type*> argTypes;
    argTypes.reserve(m_args.size() + 1);
    for (const auto& arg : m_args) {
        llvm::Type* type = arg->type->getLLVMType(*context.context);
        argTypes.push_back(llvm::PointerType::get(type, 0));
    }

    // NOTE(Vlad): can lead to problems, because main and extern defined functions, should not have "return arguments"
    bool isExtern = !m_isDefined || m_name == u8"main";
    llvm::Type* returnType = m_returnType->getLLVMType(*context.context);
    if (!isExtern && !returnType->isVoidTy()) {
        argTypes.push_back(llvm::PointerType::get(returnType, 0));
        returnType = llvm::Type::getVoidTy(*context.context);
    }
    
    llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, argTypes, false);
    llvm::Function* function = llvm::Function::Create(funcType, llvm::Function::ExternalLinkage, cStr(m_name), *context.theModule);

    for (size_t i = 0; i < m_args.size(); i++) {
        function->getArg(i)->setName(cStr(m_args[i]->identifier));
    }
    if(!isExtern && function->arg_size() > m_args.size()) {
        function->getArg(function->arg_size()-1)->setName(RETURN_ARG_NAME);
    }

    context.symbolTable.addFunction(m_name, m_returnType.get(), function);
    return function;
}

llvm::Value* FunctionAST::codegen(IRContext& context) {
    llvm::Function* function = dyn_cast<llvm::Function>(m_prototype->codegen(context));
    // TODO: Error
    if (!function->empty()){ // it's a redifinition
        logError(m_line, u8"Syntax Error: redefinition is not allowed!");
        return nullptr;
    }
    
    llvm::BasicBlock* entryBlock = llvm::BasicBlock::Create(*context.context, "entry", function);
    context.builder->SetInsertPoint(entryBlock);

    context.symbolTable.clearScopes();

    // Record function arguments in the syntax table
    const auto& arguments = m_prototype->getArgs();
    for (size_t i = 0; i < arguments.size(); i++) {
        context.symbolTable.addVariable(arguments[i]->identifier, arguments[i]->type.get(), function->getArg(i));
    }
    m_body->codegen(context);

    context.symbolTable.clearScopes(); 

    // Automatically add return for void functions
    if (!context.builder->GetInsertBlock()->getTerminator())
        context.builder->CreateRetVoid(); 

    // TODO: Error
    if (llvm::verifyFunction(*function, &(llvm::errs()))) {
        function->eraseFromParent();
        return nullptr;
    }

    context.builder->SetInsertPoint((llvm::BasicBlock*)nullptr);

    return function;
}

llvm::Value* ReturnAST::codegen(IRContext& context) {
    if (!m_expr)
        return context.builder->CreateRetVoid();

    llvm::Value* value = m_expr->codegen(context);
    if (!value)
        return nullptr;

    if (value->getType()->isPointerTy())
        value = context.builder->CreateLoad(m_expr->getType(context)->getLLVMType(*context.context), value, "loadtmp");

    llvm::BasicBlock* currentBlock = context.builder->GetInsertBlock();
    // TODO(Vlad): Error
    if (!currentBlock){    
        logError(m_line, u8"Syntax Error: return in global scope is not allowed!");
        return nullptr;
    }

    llvm::Function* function = currentBlock->getParent();
    if (function->getName() == "main") // main needs a value return
        return context.builder->CreateRet(value);

    llvm::Argument* returnArg = function->getArg(function->arg_size()-1);
    assert(returnArg->getName() == RETURN_ARG_NAME);
    context.builder->CreateStore(value, returnArg);
    return context.builder->CreateRetVoid();
}

llvm::Value* BreakAST::codegen(IRContext& context) {
    llvm::BasicBlock* returnBlock = context.afterLoop.top();
    // TODO(Vlad): Error
    if (!returnBlock){
        logError(m_line, u8"Syntax Error: break outside of loop is not allowed!");
        return nullptr;
    }

    context.builder->CreateBr(returnBlock);
    return nullptr;
}

llvm::Value* IfAST::codegen(IRContext& context) {
    llvm::Value* condition = m_cond->codegen(context);
    if (!condition)
        return nullptr;

    if (condition->getType()->isPointerTy())
        condition = context.builder->CreateLoad(llvm::Type::getInt1Ty(*context.context), condition, "loadtmp");

    condition = context.builder->CreateICmpNE(condition, llvm::ConstantInt::get(*context.context, llvm::APInt(1, 0, false)), "ifcond");

    llvm::Function* function = context.builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBlock = llvm::BasicBlock::Create(*context.context, "then", function);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(*context.context, "else");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*context.context, "ifcont");
    context.builder->CreateCondBr(condition, thenBlock, elseBlock);

    // Then
    context.builder->SetInsertPoint(thenBlock);
    m_then->codegen(context);
    if (!context.builder->GetInsertBlock()->getTerminator())
        context.builder->CreateBr(mergeBlock);
    thenBlock = context.builder->GetInsertBlock();

    // Else
    function->insert(function->end(), elseBlock);
    context.builder->SetInsertPoint(elseBlock);
    m_else->codegen(context);
    if (!context.builder->GetInsertBlock()->getTerminator())
        context.builder->CreateBr(mergeBlock);
    elseBlock = context.builder->GetInsertBlock();

    // Merge
    function->insert(function->end(), mergeBlock);
    context.builder->SetInsertPoint(mergeBlock);
    return nullptr;
}

llvm::Value* LoopAST::codegen(IRContext& context) {
    llvm::Function* function = context.builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(*context.context, "loop", function);
    llvm::BasicBlock* afterBlock = llvm::BasicBlock::Create(*context.context, "afterLoop");
    context.afterLoop.push(afterBlock);

    context.builder->CreateBr(loopBlock);
    context.builder->SetInsertPoint(loopBlock);

    m_body->codegen(context);
    context.builder->CreateBr(loopBlock);
    loopBlock = context.builder->GetInsertBlock();

    function->insert(function->end(), afterBlock);
    context.builder->SetInsertPoint(afterBlock);

    context.afterLoop.pop();
    return nullptr;
}

llvm::Value* AccessArrayElementAST::codegen(IRContext &context) {
    const ScopeEntry* arrVar = context.symbolTable.lookupVariable(m_name);
    if (!arrVar){
        logError(m_line, u8"Syntax Error: Array '" + m_name + u8"' not found!");
        return nullptr;
    }
    
    const ArrayDataType* arrType = dynamic_cast<const ArrayDataType*>(arrVar->type);
    if (arrType) {
        llvm::ArrayType* type = dyn_cast<llvm::ArrayType>(arrType->getLLVMType(*context.context));
        llvm::Value* index = m_index->codegen(context);
        if (index->getType()->isPointerTy())
            index = context.builder->CreateLoad(llvm::Type::getInt32Ty(*context.context), index, "loadtmp");
    
        static llvm::Value* zero = llvm::ConstantInt::get(llvm::Type::getInt32Ty(*context.context), llvm::APInt(32, 0, true));
        return context.builder->CreateInBoundsGEP(type, arrVar->value, {zero, index}, "arrIdx");
    }
    const StructDataType* structType = dynamic_cast<const StructDataType*>(arrVar->type);
    if (structType) {
        int index = -1;
        auto iter = context.symbolTable.lookupStruct(structType->name);
        if (!iter) {
            logError(m_line, u8"Syntax Error: Struct with name: '" + structType->name + u8"' isn't declared!");
            return nullptr;
        }

        VariableReferenceAST* ref = dynamic_cast<VariableReferenceAST*>(m_index.get());
        NumberAST* number = dynamic_cast<NumberAST*>(m_index.get());

        if (ref) {
            for(size_t i = 0; i < iter->attributes.size(); i++) {
                if (iter->attributes[i].identifier == ref->getName()) {
                    index = i;
                    break;
                }
            }
            if (index == -1) {
                logError(m_line, u8"Syntax Error: Can't find '" + ref->getName() + u8"' attribute in '" + m_name + u8"' struct!");
                return nullptr;
            }
        } else if (number) {
            index = number->getValue();
            if (index >= (int)iter->attributes.size() || index < 0) {
                std::string indexStr = std::to_string(index);
                logError(m_line, u8"Syntax Error: Can't find " + std::u8string(indexStr.begin(), indexStr.end()) + u8" attribute in '" + m_name + u8"' struct!");
                return nullptr;
            }
        } else {
            logError(m_line, u8"Syntax Error: Wrong syntax accessing struct attribute!");
            return nullptr;
        }
        
        return context.builder->CreateStructGEP(iter->getLLVMType(*context.context), arrVar->value, index, "structIdx");
    }

    logError(m_line, u8"Syntax Error: Only structs/arrays can be accessed using []!");
    return nullptr;
}

llvm::Value* StructAST::codegen([[maybe_unused]] IRContext& context) {
    auto type = dynamic_cast<const StructDataType*>(getType(context));
    context.symbolTable.addStruct(type->name, type);
    return (llvm::Value*)type->getLLVMType(*context.context);
}