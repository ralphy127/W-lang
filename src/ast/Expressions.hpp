#pragma once

#include <variant>
#include <memory>
#include <vector>
#include "AstNode.hpp"
#include "AstVisitor.hpp"
#include "token/Token.hpp"
#include "runtime/RuntimeValue.hpp"

class Expr : public AstNode {
public:
    Expr(SourceRange srcRange) : AstNode{srcRange} {}
};

class LiteralExpr : public Expr {
public:
    explicit LiteralExpr(Token, SourceRange);

    const Token& getLiteral() const { return _literal; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitLiteralExpr(*this); }

private:
    Token _literal;
};

class VariableExpr : public Expr {
public:
    explicit VariableExpr(Token, SourceRange);

    const Token& getName() const { return _variableName; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitVariableExpr(*this); }
    std::optional<LValue> getLValue() const override;

private:
    Token _variableName;
};

class BinaryExpr : public Expr {
public:
    explicit BinaryExpr(
        std::unique_ptr<Expr> left,
        Token, std::unique_ptr<Expr> right,
        SourceRange);

    const Token& getOperator() const { return _operator; }
    const Expr& getLeft() const { return *_left; }
    const Expr& getRight() const { return *_right; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitBinaryExpr(*this); }

private:
    Token _operator;
    std::unique_ptr<Expr> _left;
    std::unique_ptr<Expr> _right;
};

class UnaryExpr : public Expr {
public:
    explicit UnaryExpr(Token, std::unique_ptr<Expr> right, SourceRange);

    const Token& getOperator() const { return _operator; }
    const Expr& getRight() const { return *_right; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitUnaryExpr(*this); }

private:
    Token _operator;
    std::unique_ptr<Expr> _right;
};

class CallExpr : public Expr {
public:
    explicit CallExpr(
        std::unique_ptr<Expr> callee,
        std::vector<std::unique_ptr<Expr>> args,
        SourceRange);

    const Expr& getCallee() const { return *_callee; }
    const std::vector<std::unique_ptr<Expr>>& getArgs() const { return _args; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitCallExpr(*this); }

private:
    std::unique_ptr<Expr> _callee;
    std::vector<std::unique_ptr<Expr>> _args;
};

class DotExpr : public Expr {
public:
    explicit DotExpr(std::unique_ptr<Expr> left, Token right, SourceRange);

    const Expr& getLeft() const { return *_left; }
    const Token& getRight() const { return _right; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitDotExpr(*this); }
    std::optional<LValue> getLValue() const override;

private:
    std::unique_ptr<Expr> _left;
    Token _right;
};

class VectorExpr : public Expr {
public:
    explicit VectorExpr(std::vector<std::unique_ptr<Expr>> initializers, SourceRange);

    const std::vector<std::unique_ptr<Expr>>& getInitializers() const { return _initializers; }

    RuntimeValue accept(AstVisitor& v) const override { return v.visitVectorExpr(*this); }

private:
    std::vector<std::unique_ptr<Expr>> _initializers;
};

class LogicalExpr : public Expr {
public:
    explicit LogicalExpr(
        std::unique_ptr<Expr> left,
        Token, std::unique_ptr<Expr> right,
        SourceRange);

    const Token& getOperator() const { return _operator; }
    const Expr& getLeft() const { return *_left; }
    const Expr& getRight() const { return *_right; }
    RuntimeValue accept(AstVisitor& v) const override { return v.visitLogicalExpr(*this); }

private:
    Token _operator;
    std::unique_ptr<Expr> _left;
    std::unique_ptr<Expr> _right;
};
