#pragma once
#include <memory>
#include "Lexer.hpp"
#include "llvm/IR/Value.h"
#include <iostream>
#include <string>

/// @brief Abstract Syntax Tree: Base class for all expression node.
/// NOTE(Vlad): It is interface
class AST {
private:
    AST* m_left;
    Token m_token;
    AST* m_right;
public:
    virtual ~AST() = default;
    void printTree(const std::string& prefix = "", bool isLeft = true) const;
    virtual llvm::Value* codegen() = 0;
};


class NumberAST : public AST {
private:
    int m_value;
public:
    NumberAST(int value);
    llvm::Value* codegen() override;
};


class VariableDeclarationAST : public AST {
private:
    std::u8string m_type;
    std::u8string m_name;
public:
    VariableDeclarationAST(const std::u8string& type, const std::u8string& name);
    llvm::Value* codegen() override;
};

class VariableReferenceAST : public AST {
private:
    std::u8string m_name;
public:
    VariableReferenceAST(const std::u8string& name);
    llvm::Value* codegen() override;
};


class BinaryOperatorAST : public AST {
private:
    std::u8string m_op; // TODO(Vlad): replace with enum?
    std::unique_ptr<AST> m_LHS, m_RHS;
public:
    BinaryOperatorAST(const std::u8string& op, std::unique_ptr<AST> LHS, std::unique_ptr<AST> RHS);
    llvm::Value* codegen() override;
};


/// @brief Expression class for function calls.
class FuncCallAST : public AST {
private:
    std::u8string m_calleeIdentifier;
    std::vector<std::unique_ptr<AST>> m_args;
public:
    FuncCallAST(const std::u8string& callee, std::vector<std::unique_ptr<AST>> args);
    llvm::Value* codegen() override;
};


class FunctionAST : public AST {
private:
    std::u8string m_returnType;
    std::u8string m_name;
    std::vector<VariableDeclarationAST> m_args;    
    std::unique_ptr<AST> m_body;
public:
    FunctionAST(const std::u8string& returnType, const std::u8string& name, std::vector<VariableDeclarationAST> args, std::unique_ptr<AST> body);
    llvm::Value* codegen() override;
};


class IfAST : public AST {
private:
    std::unique_ptr<AST> m_cond;
    std::unique_ptr<AST> m_then;
    std::unique_ptr<AST> m_else;
public:
    IfAST(std::unique_ptr<AST> cond, std::unique_ptr<AST> then, std::unique_ptr<AST> _else);
    llvm::Value* codegen() override;
};


class ForAST : public AST {
private:
    std::u8string m_varName;
    std::unique_ptr<AST> m_start;
    std::unique_ptr<AST> m_end;
    std::unique_ptr<AST> m_step;
    std::unique_ptr<AST> m_body;
public:
    ForAST(const std::u8string& varName, std::unique_ptr<AST> start, std::unique_ptr<AST> end, std::unique_ptr<AST> step, std::unique_ptr<AST> body);
    llvm::Value* codegen() override;
};