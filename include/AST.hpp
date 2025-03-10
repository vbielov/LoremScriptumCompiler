#pragma once
#include <iostream>
#include <string>
#include "Types.hpp"
#include "Lexer.hpp"
#define RED "\033[31m"
#define RESET "\033[0m"

void printIndent(std::ostream& ostr, const std::string& indent, bool isLast);

/// @brief Abstract Syntax Tree: Base class
class AST {
public:
    virtual ~AST() = default;

    /// @warning Will assert, if node has no name
    virtual const std::u8string& getName() const;

    /// @warning Will assert, if node has no type
    virtual const IDataType* getType() const; 

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
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 

};


class CharAST : public AST {
private:
    char8_t m_char;

public:
    CharAST(char8_t character);
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class VariableDeclarationAST : public AST {
protected:
    std::unique_ptr<PrimitiveDataType> m_type;
    std::u8string m_name;
public:
    VariableDeclarationAST(const std::u8string& name, std::unique_ptr<PrimitiveDataType> type);
    const std::u8string& getName() const override;
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class VariableReferenceAST : public AST {
private:
    std::u8string m_name;

public:
    VariableReferenceAST(const std::u8string& name);
    const std::u8string& getName() const override;
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override;
};


class BinaryOperatorAST : public AST {
private:
    std::u8string m_op;  // TODO(Vlad): replace with enum?
    std::unique_ptr<AST> m_LHS, m_RHS;

public:
    BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS);
    const IDataType* getType() const override; 
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
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


// Function declaration
class FunctionPrototypeAST : public AST {
private:
    std::u8string m_name;
    std::unique_ptr<IDataType> m_returnType;
    std::vector<std::unique_ptr<AST>> m_args; // this should be only declarations

public:
    FunctionPrototypeAST(const std::u8string& name, std::unique_ptr<IDataType> returnType, std::vector<std::unique_ptr<AST>> args);
    const std::u8string& getName() const override;
    const IDataType* getType() const override; 
    const std::vector<std::unique_ptr<AST>>& getArgs() const; 
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
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};


class ReturnAST : public AST {
private:
    std::unique_ptr<AST> m_expr;

public:
    ReturnAST(std::unique_ptr<AST> expr);
    const IDataType* getType() const override; 
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


class ArrayDeclarationAST : public AST {
private:
    std::u8string m_name;
    std::unique_ptr<ArrayDataType> m_type;
    std::vector<std::unique_ptr<AST>> m_arrElements; // for array initialization with [1, 2, 3]

public:
    ArrayDeclarationAST(const std::u8string& name, std::unique_ptr<ArrayDataType> type);
    ArrayDeclarationAST(const std::u8string& name, std::unique_ptr<ArrayDataType> type, std::vector<std::unique_ptr<AST>> arrElements);
    const IDataType* getType() const override; 
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
    const IDataType* getType() const override; 
    Value* codegen(LLVMStructs& llvmStructs) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
};
