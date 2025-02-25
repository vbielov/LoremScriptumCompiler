#pragma once
#include <memory>
#include "Lexer.hpp"
#include "llvm/IR/Value.h"
#include <iostream>
#include <string>

/// @brief Abstract Syntax Tree: Base class for all expression node.
/// NOTE(Vlad): It is interface
class AST {
public:
    virtual ~AST() = default;
    virtual llvm::Value* codegen() = 0;
    virtual void printTree(const std::string& indent, bool isLast) const = 0;
};

void printIndent(const std::string& indent, bool isLast); 

class BlockAST : public AST {
private:
    std::vector<std::unique_ptr<AST>> m_instructions;
public:
    BlockAST(std::vector<std::unique_ptr<AST>> instructions);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};

class NumberAST : public AST {
private:
    int m_value;
public:
    NumberAST(int value);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class CharAST : public AST {
private:
    char8_t m_char;
public:
    CharAST(char8_t character);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class VariableDeclarationAST : public AST {
private:
    std::u8string m_type;
    std::u8string m_name;
public:
    VariableDeclarationAST(const std::u8string& type, const std::u8string& name);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class VariableReferenceAST : public AST {
private:
    std::u8string m_name;
public:
    VariableReferenceAST(const std::u8string& name);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class BinaryOperatorAST : public AST {
private:
    std::u8string m_op; // TODO(Vlad): replace with enum?
    std::unique_ptr<AST> m_LHS, m_RHS;
public:
    BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class FuncCallAST : public AST {
private:
    std::u8string m_calleeIdentifier;
    std::vector<std::unique_ptr<AST>> m_args;
public:
    FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class FunctionAST : public AST {
private:
    std::u8string m_returnType;
    std::u8string m_name;
    std::vector<std::unique_ptr<VariableDeclarationAST>> m_args;    
    std::unique_ptr<BlockAST> m_body;
public:
    FunctionAST(const std::u8string& returnType, const std::u8string& name, std::vector<std::unique_ptr<VariableDeclarationAST>> args, std::unique_ptr<BlockAST> body);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class IfAST : public AST {
private:
    std::unique_ptr<AST> m_cond;
    std::unique_ptr<BlockAST> m_then;
    std::unique_ptr<BlockAST> m_else;
public:
    IfAST(std::unique_ptr<AST> cond, std::unique_ptr<BlockAST> then, std::unique_ptr<BlockAST> _else);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};


class ForAST : public AST {
private:
    std::u8string m_varName;
    std::unique_ptr<AST> m_start;
    std::unique_ptr<AST> m_end;
    std::unique_ptr<AST> m_step;
    std::unique_ptr<BlockAST> m_body;
public:
    ForAST(const std::u8string& varName, std::unique_ptr<AST> start, std::unique_ptr<AST> end, std::unique_ptr<AST> step, std::unique_ptr<BlockAST> body);
    llvm::Value* codegen() override;
    void printTree(const std::string& indent, bool isLast) const override;
};