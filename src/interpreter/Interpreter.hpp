#pragma once

#include <unordered_map>
#include <functional>
#include "ast/Visitor.hpp"
#include "ast/Statements.hpp"
#include "runtime/Environment.hpp"

class Interpreter : public Visitor {
public:
    explicit Interpreter(std::vector<std::unique_ptr<Stmt>>);

    void interpret();

    RuntimeValue visitVarDefinitionStmt(const VarDefinitionStmt&) override;
    RuntimeValue visitReassignStmt(const ReassignStmt&) override;
    RuntimeValue visitBlockStmt(const BlockStmt&) override;
    RuntimeValue visitIfStmt(const IfStmt&) override;
    RuntimeValue visitLoopStmt(const LoopStmt&) override;
    RuntimeValue visitRepeatStmt(const RepeatStmt&) override;
    RuntimeValue visitReturnStmt(const ReturnStmt&) override;
    RuntimeValue visitBreakStmt(const BreakStmt&) override;
    RuntimeValue visitFunctionStmt(const FunctionStmt&) override;
    RuntimeValue visitExpressionStmt(const ExpressionStmt&) override;
    RuntimeValue visitImportStmt(const ImportStmt&) override;
    
    RuntimeValue visitLiteralExpr(const LiteralExpr&) override;
    RuntimeValue visitVariableExpr(const VariableExpr&) override;
    RuntimeValue visitBinaryExpr(const BinaryExpr&) override;
    RuntimeValue visitUnaryExpr(const UnaryExpr&) override;
    RuntimeValue handleUserDefinedFunctionCall(
        const VariableExpr&,
        const std::vector<std::unique_ptr<Expr>>& callArgs);
    RuntimeValue visitCallExpr(const CallExpr&) override;
    RuntimeValue handleModuleCall(const Module&, const std::string& rightName);
    RuntimeValue handleVectorMethodCall(const Vector&, const std::string& rightName);
    RuntimeValue visitDotExpr(const DotExpr&) override;
    RuntimeValue visitVectorExpr(const VectorExpr&) override;

private:
    const std::vector<std::unique_ptr<Stmt>> _statements;
    std::unordered_map<std::string, std::reference_wrapper<const FunctionStmt>> _functions;
    std::shared_ptr<Environment> _globalEnvironment{std::make_shared<Environment>()};
    std::shared_ptr<Environment> _currentEnvironment{_globalEnvironment};
    std::uint32_t _scopeDepth{1u};
};