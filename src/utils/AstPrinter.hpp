#pragma once

#include "ast/AstVisitor.hpp"
#include <iosfwd>
#include <string_view>

class Stmt;

class AstPrinter : public AstVisitor {
public:
    AstPrinter();
    explicit AstPrinter(std::ostream& out);

    void print(const std::vector<std::unique_ptr<Stmt>>& stmts);

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
    RuntimeValue visitCallExpr(const CallExpr&) override;
    RuntimeValue visitDotExpr(const DotExpr&) override;
    RuntimeValue visitVectorExpr(const VectorExpr&) override;
    RuntimeValue visitLogicalExpr(const LogicalExpr&) override;

private:
    void printIndent();
    void printLine(std::string_view text);
    void printKey(std::string_view key);

    std::ostream& _out;
    std::uint32_t _indent{0u};
};