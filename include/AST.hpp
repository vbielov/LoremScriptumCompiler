#pragma once
#include <iostream>
#include <string>
#include "LLVMStructs.hpp"
#include "Lexer.hpp"
#define RED "\033[31m"
#define RESET "\033[0m"

Type* getTypeFromStr(const std::u8string& typeStr, const LLVMStructs& llvmStructs);

void printIndent(std::ostream& ostr, const std::string& indent, bool isLast);

/// @brief Abstract Syntax Tree: Base class
class AST {
public:
    virtual ~AST() = default;

    /// @warning Will assert, if node has no name
    virtual const std::u8string& getName() const;

    /// @warning Will assert, if node has no type
    /// @returns if there is no special type (pointer, array, contant), will return getElementType()
    virtual Type* getType(LLVMStructs& llvmStructs) const; 

    /// @warning Will assert, if node has no type
    virtual Type* getElementType(LLVMStructs& llvmStructs) const;

    /// @warning Abstract method, has to implemented in every AST
    virtual Value* codegen(LLVMStructs& llvmStructs) = 0;

    /// @warning Abstract method, has to implemented in every AST
    virtual void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const = 0;
};


class BlockAST : public AST {
private:
    std::vector<std::unique_ptr<AST>> m_instructions;

public:
    BlockAST(std::vector<std::unique_ptr<AST>> instructions);
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class NumberAST : public AST {
private:
    int m_value;

public:
    NumberAST(int value);
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 

};


class CharAST : public AST {
private:
    char8_t m_char;

public:
    CharAST(char8_t character);
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class VariableDeclarationAST : public AST {
protected:
    std::u8string m_type;
    std::u8string m_name;
public:
    VariableDeclarationAST(const std::u8string& type, const std::u8string& name);
    const std::u8string& getName() const override;
    Type* getType(LLVMStructs& llvmStructs) const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class VariableReferenceAST : public AST {
private:
    std::u8string m_name;

public:
    VariableReferenceAST(const std::u8string& name);
    const std::u8string& getName() const override;
    Type* getType(LLVMStructs& llvmStructs) const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override;
};


class BinaryOperatorAST : public AST {
private:
    std::u8string m_op;  // TODO(Vlad): replace with enum?
    std::unique_ptr<AST> m_LHS, m_RHS;

public:
    BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS);
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class FuncCallAST : public AST {
private:
    std::u8string m_calleeIdentifier;
    std::vector<std::unique_ptr<AST>> m_args;

public:
    FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args);
    const std::u8string& getName() const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


// Function declaration
class FunctionPrototypeAST : public AST {
private:
    std::u8string m_returnType;
    std::u8string m_name;
    std::vector<std::unique_ptr<VariableDeclarationAST>> m_args;
    bool m_returnsArray;
    int m_arrSize;
public:
    FunctionPrototypeAST(const std::u8string& returnType, const std::u8string& name, std::vector<std::unique_ptr<VariableDeclarationAST>> args, bool returnsArray, int arrSize);
    const std::u8string& getName() const override;
    Type* getType(LLVMStructs& llvmStructs) const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    const std::vector<std::unique_ptr<VariableDeclarationAST>>& getArgs() const; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override;
};


// Whole function
class FunctionAST : public AST {
private:
    std::unique_ptr<FunctionPrototypeAST> m_prototype;
    std::unique_ptr<BlockAST> m_body;
public:
    FunctionAST(std::unique_ptr<FunctionPrototypeAST> prototype, std::unique_ptr<BlockAST> body);
    const std::u8string& getName() const override;
    Type* getType(LLVMStructs& llvmStructs) const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class ReturnAST : public AST {
private:
    std::unique_ptr<AST> m_expr;

public:
    ReturnAST(std::unique_ptr<AST> expr);
    Type* getType(LLVMStructs& llvmStructs) const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class BreakAST : public AST {
public:
    BreakAST();
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class IfAST : public AST {
private:
    std::unique_ptr<AST> m_cond;
    std::unique_ptr<BlockAST> m_then;
    std::unique_ptr<BlockAST> m_else;

public:
    IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else);
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class LoopAST : public AST {
private:
    std::unique_ptr<BlockAST> m_body;

public:
    LoopAST(std::unique_ptr<BlockAST> body);
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};

// Array declaration for global/variable
class ArrayAST : public VariableDeclarationAST {
private:
    int m_size;
    std::vector<std::unique_ptr<AST>> m_arrElements;

public:
    ArrayAST(const std::u8string& type, const std::u8string& name, int size);
    ArrayAST(const std::u8string& type, const std::u8string& name, int size, std::vector<std::unique_ptr<AST>> arrElements);
    Type* getType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class AccessArrayElementAST : public AST {
private:
    std::u8string m_name;
    std::unique_ptr<AST> m_index; // you can access with variable reference ect.

public:
    AccessArrayElementAST(const std::u8string& name, std::unique_ptr<AST> index);
    const std::u8string& getName() const override;
    Type* getElementType(LLVMStructs& llvmStructs) const override;
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};
