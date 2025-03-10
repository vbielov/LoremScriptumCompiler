#include "Types.hpp"

PrimitiveDataType::PrimitiveDataType(PrimitiveType type)
    : type(type) {}

llvm::Type* PrimitiveDataType::getLLVMType(LLVMStructs& llvmStructs) const {
    switch(type) {
        case PrimitiveType::INT:
            return Type::getInt32Ty(*(llvmStructs.theContext));
        case PrimitiveType::BOOL:
            return Type::getInt1Ty(*(llvmStructs.theContext));
        case PrimitiveType::CHAR:
            return Type::getInt8Ty(*(llvmStructs.theContext));
        case PrimitiveType::VOID:
            return Type::getVoidTy(*(llvmStructs.theContext));
        default:
            assert(false && "Unknown type");
            return nullptr;
    }
}

ArrayDataType::ArrayDataType(PrimitiveType type, size_t size) 
    : type(type)
    , size(size) {}

llvm::Type* ArrayDataType::getLLVMType(LLVMStructs& llvmStructs) const {
    switch(type) {
        case PrimitiveType::INT:
            return ArrayType::getInt32Ty(*(llvmStructs.theContext));
        case PrimitiveType::BOOL:
            return ArrayType::getInt1Ty(*(llvmStructs.theContext));
        case PrimitiveType::CHAR:
            return ArrayType::getInt8Ty(*(llvmStructs.theContext));
        case PrimitiveType::VOID:
            return ArrayType::getVoidTy(*(llvmStructs.theContext));
        default:
            assert(false && "Unknown type");
            return nullptr;
    }
}

StructDataType::StructDataType(std::vector<std::unique_ptr<StructAttribute>> attributes)
    : attributes(std::move(attributes)) {}
    
llvm::Type* StructDataType::getLLVMType([[maybe_unused]] LLVMStructs& llvmStructs) const {
    // TODO(Vlad)...
    return nullptr;
}
