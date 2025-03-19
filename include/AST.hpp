#pragma once
#include <iostream>
#include <string>
#include "Types.hpp"
#include "IRContext.hpp"
#include "Lexer.hpp"
#include "ErrorHandler.hpp"
#define RED "\033[31m"
#define RESET "\033[0m"

// NOTE(Vlad):
// it appears multiple times in code, and needs to be equal everywhere
// because I search by the name in FuncCallAST
// We can fix it in the future, if we add "bool isExtern" to symbolTable for functions
inline const char* RETURN_ARG_NAME = "returnArg"; 

void printIndent(std::ostream& ostr, const std::string& indent, bool isLast);

/// @brief Abstract Syntax Tree: Base class
class AST {
public:
    AST() {}

    virtual ~AST() = default;

    /// @warning Will assert, if node has no name
    virtual const std::u8string& getName() const;

    /// @warning Will assert, if node has no type
    virtual const IDataType* getType(const IRContext& context); 

    /// @warning Abstract method, has to implemented in every AST
    virtual llvm::Value* codegen(IRContext& context) = 0;

    /// @warning Abstract method, has to implemented in every AST
    virtual void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const = 0;

    virtual size_t getLine() const = 0;
};


class BlockAST : public AST {
private:
    std::vector<std::unique_ptr<AST>> m_instructions;
    size_t m_line;

public:
    BlockAST(std::vector<std::unique_ptr<AST>> instructions, size_t line);
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class NumberAST : public AST {
private:
    int m_value;
    size_t m_line;

public:
    NumberAST(int value, size_t line);
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
    int getValue() const;
};


class CharAST : public AST {
private:
    char8_t m_char;
    size_t m_line;

public:
    CharAST(char8_t character, size_t line);
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class BoolAST : public AST {
private:
    bool m_bool;
    size_t m_line;

public:
    BoolAST(bool boolean, size_t line);
    const IDataType* getType(const IRContext& context) override;
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class VariableDeclarationAST : public AST {
private:
    std::u8string m_name;
    std::unique_ptr<IDataType> m_type;
    size_t m_line;

public:
    VariableDeclarationAST(const std::u8string& name, std::unique_ptr<IDataType> type, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class VariableReferenceAST : public AST {
private:
    std::u8string m_name;
    size_t m_line;

public:
    VariableReferenceAST(const std::u8string& name, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override;
    size_t getLine() const override;
};


class BinaryOperatorAST : public AST {
private:
    std::u8string m_op;
    std::unique_ptr<AST> m_LHS, m_RHS;
    size_t m_line;

public:
    BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS, size_t line);
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class FuncCallAST : public AST {
private:
    std::u8string m_calleeIdentifier;
    std::vector<std::unique_ptr<AST>> m_args;
    size_t m_line;

public:
    FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


// Function declaration
class FunctionPrototypeAST : public AST {
private:
    std::u8string m_name;
    std::unique_ptr<IDataType> m_returnType;
    std::vector<std::unique_ptr<TypeIdentifierPair>> m_args; // this should be only declarations
    bool m_isDefined;
    size_t m_line;

public:
    FunctionPrototypeAST(const std::u8string& name, std::unique_ptr<IDataType> returnType, std::vector<std::unique_ptr<TypeIdentifierPair>> args, bool isDefined, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    const std::vector<std::unique_ptr<TypeIdentifierPair>>& getArgs() const;
    bool isDefined() const;
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override;
    size_t getLine() const override;
};


// Whole function
class FunctionAST : public AST {
private:
    std::unique_ptr<FunctionPrototypeAST> m_prototype;
    std::unique_ptr<BlockAST> m_body;
    size_t m_line;

public:
    FunctionAST(std::unique_ptr<FunctionPrototypeAST> prototype, std::unique_ptr<BlockAST> body, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class ReturnAST : public AST {
private:
    std::unique_ptr<AST> m_expr;
    size_t m_line;

public:
    ReturnAST(std::unique_ptr<AST> expr, size_t line);
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class BreakAST : public AST {
private:
    size_t m_line;

public:
    BreakAST(size_t line);
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class IfAST : public AST {
private:
    std::unique_ptr<AST> m_cond;
    std::unique_ptr<BlockAST> m_then;
    std::unique_ptr<BlockAST> m_else;
    size_t m_line;

public:
    IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else, size_t line);
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class LoopAST : public AST {
private:
    std::unique_ptr<BlockAST> m_body;
    size_t m_line;

public:
    LoopAST(std::unique_ptr<BlockAST> body, size_t line);
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};

// Maybe just replace it already in parser with blockAST of assigments?
class ArrayInitializationAST : public AST {
private:
    std::u8string m_name;
    std::vector<std::unique_ptr<AST>> m_elements;
    size_t m_line;

public:
    ArrayInitializationAST(const std::u8string& name, std::vector<std::unique_ptr<AST>> elements, size_t line);
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};

class AccessArrayElementAST : public AST {
private:
    std::u8string m_name;
    std::unique_ptr<AST> m_index; 
    std::unique_ptr<IDataType> m_type; // this is cached type and will be defined after getType() is called
    size_t m_line;

public:
    AccessArrayElementAST(const std::u8string& name, std::unique_ptr<AST> index, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};


class StructAST : public AST {
private:
    std::unique_ptr<StructDataType> m_type;
    size_t m_line;

public:
    StructAST(std::unique_ptr<StructDataType> attributes, size_t line);
    const std::u8string& getName() const override;
    const IDataType* getType(const IRContext& context) override; 
    llvm::Value* codegen(IRContext& context) override;
    void printTree(std::ostream& ostr, const std::string& indent, bool isLast) const override; 
    size_t getLine() const override;
};