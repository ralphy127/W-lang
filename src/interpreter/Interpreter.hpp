#pragma once

#include <unordered_map>
#include <functional>
#include "ast/AstVisitor.hpp"
#include "ast/Statements.hpp"
#include "runtime/Environment.hpp"

struct ParserError;
using AstResolver = std::function<std::vector<std::unique_ptr<Stmt>>(const std::string&)>;

class Interpreter : public AstVisitor {
public:
    explicit Interpreter(
        std::vector<std::unique_ptr<Stmt>>,
        AstResolver,
        std::string mainFolderPath);

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
    RuntimeValue visitCallExpr(const CallExpr&) override;
    RuntimeValue visitDotExpr(const DotExpr&) override;
    RuntimeValue visitVectorExpr(const VectorExpr&) override;
    RuntimeValue visitLogicalExpr(const LogicalExpr&) override;
    
private:
    RuntimeValue evaluate(const AstNode&);
    RuntimeValue handleModuleCall(const Module&, const std::string& rightName, const DotExpr& expr);
    
    const std::vector<std::unique_ptr<Stmt>> _statements;
    AstResolver _astResolver;
    // TODO! while working on some global file solution, remove this field 
    std::string _mainFolderPath;
    std::shared_ptr<Environment> _globalEnvironment{std::make_shared<Environment>()};
    std::shared_ptr<Environment> _currentEnvironment{_globalEnvironment};
    std::uint32_t _scopeDepth{1u};
    std::unordered_map<std::string, std::vector<std::unique_ptr<Stmt>>> _importedModuleAsts;
    SourceRange _currentRange{0u, {0u, 0u}, {0u, 0u}};
};