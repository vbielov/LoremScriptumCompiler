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

std::u8string PrimitiveDataType::toString() const {
    PrimitiveType cpType = type;
    auto it = std::find_if(STR_TO_PRIMITIVE_MAP.begin(), STR_TO_PRIMITIVE_MAP.end(), [&cpType](const auto& pt) {
        return pt.second == cpType;
    });
    assert(it != STR_TO_PRIMITIVE_MAP.end());
    return std::u8string(it->first);
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

std::u8string ArrayDataType::toString() const {
    PrimitiveType cpType = type;
    auto it = std::find_if(STR_TO_PRIMITIVE_MAP.begin(), STR_TO_PRIMITIVE_MAP.end(), [&cpType](const auto& pt) {
        return pt.second == cpType;
    });
    assert(it != STR_TO_PRIMITIVE_MAP.end());
    std::string sizeStr = std::to_string(size);
    return (std::u8string(it->first) + u8"[" + std::u8string(sizeStr.begin(), sizeStr.end()) + u8"]");
}

StructDataType::StructDataType(const std::u8string& name)
    : name(name), attributes() {}

StructDataType::StructDataType(const std::u8string &name, std::vector<TypeIdentifierPair> attributes)
    : name(name), attributes(std::move(attributes)) {
    s_structsTypeMap[name] = this;
}

llvm::Type* StructDataType::getLLVMType(llvm::LLVMContext& context) const {
    if (!attributes.empty()) {
        std::vector<llvm::Type*> attrTypes;
        attrTypes.reserve(attributes.size());
        for (const auto& attr : attributes) {
            attrTypes.push_back(attr.type->getLLVMType(context));
        }
        llvm::StructType* structType = llvm::StructType::create(context, attrTypes, (const char*)name.c_str());
        return structType;
    }
    return llvm::StructType::getTypeByName(context, (const char*)name.c_str());
}

std::u8string StructDataType::toString() const {
    return u8"TODO";
}

TypeIdentifierPair::TypeIdentifierPair(std::unique_ptr<IDataType> type, const std::u8string& identifier)
    : type(std::move(type)), identifier(identifier) {}
