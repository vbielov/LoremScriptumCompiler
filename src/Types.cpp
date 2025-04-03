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

ArrayDataType::ArrayDataType(std::unique_ptr<IDataType> elementType, size_t size)
    : elementType(std::move(elementType))
    , size(size) {}

llvm::Type* ArrayDataType::getLLVMType(llvm::LLVMContext& context) const {
    llvm::Type* type = elementType->getLLVMType(context);
    return llvm::ArrayType::get(type, size);
}

std::u8string ArrayDataType::toString() const {
    std::u8string type = elementType->toString();
    std::string sizeStr = std::to_string(size);
    return (type + u8"[" + std::u8string(sizeStr.begin(), sizeStr.end()) + u8"]");
}

TypeIdentifierPair::TypeIdentifierPair(std::unique_ptr<IDataType> type, const std::u8string& identifier)
    : type(std::move(type)), identifier(identifier) {}

StructDataType::StructDataType(const std::u8string& name)
    : name(name), attributes() {}

StructDataType::StructDataType(const std::u8string &name, std::vector<TypeIdentifierPair> attributes)
    : name(name), attributes(std::move(attributes)) {}

llvm::Type* StructDataType::getLLVMType(llvm::LLVMContext& context) const {
    auto type = llvm::StructType::getTypeByName(context, (const char*)name.c_str());
    if (type)
        return type;
    std::vector<llvm::Type*> attrTypes;
    attrTypes.reserve(attributes.size());
    for (const auto& attr : attributes) {
        attrTypes.push_back(attr.type->getLLVMType(context));
    }
    llvm::StructType* structType = llvm::StructType::create(context, attrTypes, (const char*)name.c_str());
    return structType;
}

std::u8string StructDataType::toString() const {
    std::u8string str = name + u8" {";
    for(const auto& attr : attributes) {
        str += attr.type->toString() + u8" " + attr.identifier;
        if (&attr != &attributes.back()) {
            str += u8", ";
        }
    }
    str += u8"}";
    return str;
}
