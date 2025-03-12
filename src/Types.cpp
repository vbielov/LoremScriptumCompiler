#include "Types.hpp"

PrimitiveDataType::PrimitiveDataType(PrimitiveType type)
    : type(type) {}

llvm::Type* PrimitiveDataType::getLLVMType(llvm::LLVMContext& context) const {
    switch(type) {
        case PrimitiveType::INT:
            return llvm::Type::getInt32Ty(context);
        case PrimitiveType::BOOL:
            return llvm::Type::getInt1Ty(context);
        case PrimitiveType::CHAR:
            return llvm::Type::getInt8Ty(context);
        case PrimitiveType::VOID:
            return llvm::Type::getVoidTy(context);
        default:
            assert(false && "Unknown type");
            return nullptr;
    }
}

ArrayDataType::ArrayDataType(PrimitiveType type, size_t size) 
    : type(type)
    , size(size) {}

llvm::Type* ArrayDataType::getLLVMType(llvm::LLVMContext& context) const {
    switch(type) {
        case PrimitiveType::INT:
            return llvm::ArrayType::get(llvm::Type::getInt32Ty(context), size);
        case PrimitiveType::BOOL:
            return llvm::ArrayType::get(llvm::Type::getInt1Ty(context), size);
        case PrimitiveType::CHAR:
            return llvm::ArrayType::get(llvm::Type::getInt8Ty(context), size);
        case PrimitiveType::VOID:
            assert(false && "Array of type void is not allowed my friend.");
            return llvm::ArrayType::get(llvm::Type::getVoidTy(context), size);
        default:
            assert(false && "Unknown type");
            return nullptr;
    }
}

StructDataType::StructDataType(std::vector<std::unique_ptr<StructAttribute>> attributes)
    : attributes(std::move(attributes)) {}
    
llvm::Type* StructDataType::getLLVMType([[maybe_unused]] llvm::LLVMContext& context) const {
    // TODO(Vlad)...
    return nullptr;
}
