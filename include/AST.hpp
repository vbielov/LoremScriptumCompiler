#pragma once
#include "Token.hpp"
#include <memory>
#include "llvm/IR/Value.h"

/// @brief Abstract Syntax Tree: Base class for all expression node.
/// NOTE(Vlad): It is interface
class AST {
private:
    AST* m_left;
    Token m_token;
    AST* m_right;
public:
    virtual ~AST() = default;
    virtual llvm::Value* codegen() = 0;
};


class NumberAST : public AST {
private:
    int m_value;
public:
    NumberAST(int value);
    llvm::Value* codegen() override;
};


class VariableAST : public AST {
private:
    std::u8string m_name;
public:
    VariableAST(const std::u8string& name);
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


/// @brief This class represents the "prototype" for a function,
/// which captures its name, and its argument names (thus implicitly the number
/// of arguments the function takes).
class FuncDeclarationAST : public AST {
private:
    std::u8string m_name;
    std::vector<std::u8string> m_args;    
public:
    FuncDeclarationAST(const std::u8string& name, std::vector<std::u8string> args);
    llvm::Value* codegen() override;
};


/// @brief This class represents a function definition itself.
class FuncDefinitionAST : public AST {
private:
    std::unique_ptr<FuncDeclarationAST> m_declaration;
    std::unique_ptr<AST> m_body;
public:
    FuncDefinitionAST(std::unique_ptr<FuncDeclarationAST> declaration, std::unique_ptr<AST> body);
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