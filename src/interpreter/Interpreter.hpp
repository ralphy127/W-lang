#pragma once

#include <unordered_map>
#include "ast/Visitor.hpp"
#include "ast/Statements.hpp"

class Interpreter : public Visitor {
public:
    explicit Interpreter(std::vector<std::unique_ptr<Stmt>>);

    void interpret();

    RuntimeValue visitVarDefinitionStmt(const VarDefinitionStmt&) override;
    RuntimeValue visitAssignStmt(const AssignStmt&) override;
    RuntimeValue visitBlockStmt(const BlockStmt&) override;
    RuntimeValue visitIfStmt(const IfStmt&) override;
    RuntimeValue visitLoopStmt(const LoopStmt&) override;
    RuntimeValue visitRepeatStmt(const RepeatStmt&) override;
    RuntimeValue visitPrintStmt(const PrintStmt&) override;
    RuntimeValue visitReturnStmt(const ReturnStmt&) override;
    RuntimeValue visitBreakStmt(const BreakStmt&) override;
    RuntimeValue visitFunctionStmt(const FunctionStmt&) override;
    RuntimeValue visitExpressionStmt(const ExpressionStmt&) override;
    
    RuntimeValue visitLiteralExpr(const LiteralExpr&) override;
    RuntimeValue visitVariableExpr(const VariableExpr&) override;
    RuntimeValue visitBinaryExpr(const BinaryExpr&) override;
    RuntimeValue visitUnaryExpr(const UnaryExpr&) override;
    RuntimeValue visitCallExpr(const CallExpr&) override;

private:
    const std::vector<std::unique_ptr<Stmt>> _statements;
    std::unordered_map<std::string, const FunctionStmt&> _functions;
};