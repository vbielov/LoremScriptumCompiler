#pragma once
#include <unordered_map>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "Syntax.hpp"

enum class PrimitiveType {
    INT, BOOL, CHAR, VOID
};

inline const std::unordered_map<std::u8string_view, PrimitiveType> STR_TO_PRIMITIVE_MAP = {
    { types::INT, PrimitiveType::INT },
    { types::BOOL, PrimitiveType::BOOL },
    { types::CHAR, PrimitiveType::CHAR },
    { types::VOID, PrimitiveType::VOID }
};

class IDataType {
public:
    virtual ~IDataType() {};
    virtual llvm::Type* getLLVMType(llvm::LLVMContext& context) const = 0;
    virtual std::u8string toString() const = 0;
};


class PrimitiveDataType : public IDataType {
public:
    PrimitiveType type;
    PrimitiveDataType(PrimitiveType type);
    llvm::Type* getLLVMType(llvm::LLVMContext& context) const override; 
    std::u8string toString() const override;
};


class ArrayDataType : public IDataType {
public:
    PrimitiveType type;
    size_t size;
    ArrayDataType(PrimitiveType type, size_t size);
    llvm::Type* getLLVMType(llvm::LLVMContext& context) const override; 
    std::u8string toString() const override;
};

struct TypeIdentifierPair {
    std::unique_ptr<IDataType> type;
    std::u8string identifier;

    TypeIdentifierPair(std::unique_ptr<IDataType> type, const std::u8string& identifier);
};

class StructDataType : public IDataType {
public:
    std::u8string name;
    std::vector<TypeIdentifierPair> attributes;

    StructDataType(const std::u8string& name);
    StructDataType(const std::u8string& name, std::vector<TypeIdentifierPair> attributes);
    llvm::Type* getLLVMType(llvm::LLVMContext& context) const override; 
    std::u8string toString() const override;
};