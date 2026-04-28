#pragma once

#include <memory>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include "AstNode.hpp"
#include "Expressions.hpp"
#include "token/Token.hpp"
#include "AstVisitor.hpp"

class Stmt : public AstNode {
public:
    Stmt(SourceRange srcRange) : AstNode{srcRange} {}
};

class VarDefinitionStmt : public Stmt {
public:
    explicit VarDefinitionStmt(Token name, std::unique_ptr<Expr> initializer, SourceRange);

    const Token& getName() const { return _name; }
    const Expr& getInitializer() const;
    RuntimeValue accept(AstVisitor& v) const override { return v.visitVarDefinitionStmt(*this); }

private:
    Token _name;
    std::unique_ptr<Expr> _initializer;
};

class ReassignStmt : public Stmt {
public:
    explicit ReassignStmt(std::unique_ptr<Expr> target, std::unique_ptr<Expr> value, SourceRange);

    const Expr& getTarget() const { return *_target; }
    const Expr& getValue() const { return *_value; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitReassignStmt(*this); }

private:
    std::unique_ptr<Expr> _target;
    std::unique_ptr<Expr> _value;
};

class BlockStmt : public Stmt {
public:
    BlockStmt() : Stmt{SourceRange{}} {}
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements, SourceRange);

    const std::vector<std::unique_ptr<Stmt>>& getStatements() const { return _statements; }

    RuntimeValue accept(AstVisitor& v) const override { return v.visitBlockStmt(*this); }

private:
    std::vector<std::unique_ptr<Stmt>> _statements{};
};

struct ElseIfClause {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

class IfStmt : public Stmt {
public:
    explicit IfStmt(SourceRange srcRange,
                    std::unique_ptr<Expr> condition, 
                    std::unique_ptr<Stmt> thenBlock,
                    std::vector<ElseIfClause> elseIfs = std::vector<ElseIfClause>{},
                    std::unique_ptr<Stmt> elseBlock = nullptr);

    const Expr& getCondition() const;
    const Stmt& getThenBlock() const;
    const std::vector<ElseIfClause>& getElseIfClauses() const { return _elseIfs; }
    std::optional<std::reference_wrapper<const Stmt>> getElseBlock() const;
    RuntimeValue accept(AstVisitor& v) const override { return v.visitIfStmt(*this); }

private:
    std::unique_ptr<Expr> _condition;
    std::unique_ptr<Stmt> _thenBlock;
    std::vector<ElseIfClause> _elseIfs;
    std::unique_ptr<Stmt> _elseBlock;
};

class LoopStmt : public Stmt {
public:
    explicit LoopStmt(std::unique_ptr<Stmt> body, SourceRange);

    const Stmt& getBody() const;
    RuntimeValue accept(AstVisitor& v) const override { return v.visitLoopStmt(*this); }

private:
    std::unique_ptr<Stmt> _body;
};

class RepeatStmt : public Stmt {
public:
    explicit RepeatStmt(std::unique_ptr<Expr> count, std::unique_ptr<Stmt> body, SourceRange);

    const Expr& getCount() const;
    const Stmt& getBody() const;
    RuntimeValue accept(AstVisitor& v) const override { return v.visitRepeatStmt(*this); }

private:
    std::unique_ptr<Expr> _count;
    std::unique_ptr<Stmt> _body;
};

class ReturnStmt : public Stmt {
public:
    explicit ReturnStmt(std::unique_ptr<Expr> value, SourceRange);

    std::optional<std::reference_wrapper<const Expr>> getValue() const;
    RuntimeValue accept(AstVisitor& v) const override { return v.visitReturnStmt(*this); }

private:
    std::unique_ptr<Expr> _value;
};

class BreakStmt : public Stmt {
public:
    BreakStmt(SourceRange srcRange) : Stmt{srcRange} {}
    RuntimeValue accept(AstVisitor& v) const override { return v.visitBreakStmt(*this); }
};

class FunctionStmt : public Stmt {
public:
    explicit FunctionStmt(Token name, 
                         std::vector<Token> parameters,
                         std::unique_ptr<Stmt> body,
                         SourceRange);

    const Token& getName() const { return _name; }
    const std::vector<Token>& getParameters() const { return _parameters; }
    const Stmt& getBody() const;
    RuntimeValue accept(AstVisitor& v) const override { return v.visitFunctionStmt(*this); }

private:
    Token _name;
    std::vector<Token> _parameters;
    std::unique_ptr<Stmt> _body;
};

class ExpressionStmt : public Stmt {
public:
    explicit ExpressionStmt(std::unique_ptr<Expr>, SourceRange);

    const Expr& getExpression() const { return *_expression; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitExpressionStmt(*this); }

private:
    std::unique_ptr<Expr> _expression;
};

class ImportStmt : public Stmt {
public:
    explicit ImportStmt(Token moduleName, SourceRange);

    const Token& getModuleName() const { return _moduleName; }

    RuntimeValue accept(AstVisitor& v) const override { return v.visitImportStmt(*this); }

private:
    Token _moduleName;
};
