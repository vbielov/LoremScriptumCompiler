#pragma once
#include <unordered_map>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/DerivedTypes.h"
#include "Syntax.hpp"

// Must have same order as in Syntax.hpp, for correct indexing in debug messages
enum class PrimitiveType {
    INT, BOOL, CHAR, VOID
};

inline const std::unordered_map<std::u8string_view, PrimitiveType> PRIMITIVE_TYPE_MAP = {
    { types::INT, PrimitiveType::INT },
    { types::BOOL, PrimitiveType::BOOL },
    { types::CHAR, PrimitiveType::CHAR },
    { types::VOID, PrimitiveType::VOID }
};


class IDataType {
public:
    virtual ~IDataType() {};
    virtual llvm::Type* getLLVMType(llvm::LLVMContext& context) const = 0;
};


class PrimitiveDataType : public IDataType {
public:
    PrimitiveType type;
    PrimitiveDataType(PrimitiveType type);
    llvm::Type* getLLVMType(llvm::LLVMContext& context) const override; 
};


class ArrayDataType : public IDataType {
public:
    PrimitiveType type;
    size_t size;
   ArrayDataType(PrimitiveType type, size_t size);
    llvm::Type* getLLVMType(llvm::LLVMContext& context) const override; 
};


struct StructAttribute {
    std::u8string name;
    std::unique_ptr<IDataType> type;
};


class StructDataType : public IDataType {
public:
    std::vector<std::unique_ptr<StructAttribute>> attributes;
    StructDataType(std::vector<std::unique_ptr<StructAttribute>> attributes);
    llvm::Type* getLLVMType(llvm::LLVMContext& context) const override; 
};