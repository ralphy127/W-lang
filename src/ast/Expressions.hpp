#pragma once

#include <variant>
#include <memory>
#include <vector>
#include "AstNode.hpp"
#include "Visitor.hpp"
#include "token/Token.hpp"
#include "runtime/RuntimeValue.hpp"

class Expr : public AstNode {
public:
    virtual RuntimeValue accept(Visitor&) const = 0;
};

class LiteralExpr : public Expr {
public:
    explicit LiteralExpr(Token);

    const Token& getLiteral() const { return _literal; }
    RuntimeValue accept(Visitor& v) const override { return v.visitLiteralExpr(*this); }

private:
    Token _literal;
};

class VariableExpr : public Expr {
public:
    explicit VariableExpr(Token);

    const Token& getName() const { return _variableName; }
    RuntimeValue accept(Visitor& v) const override { return v.visitVariableExpr(*this); }

private:
    Token _variableName;
};

class BinaryExpr : public Expr {
public:
    explicit BinaryExpr(std::unique_ptr<Expr> left, Token, std::unique_ptr<Expr> right);

    const Token& getOperator() const { return _operator; }
    const Expr& getLeft() const;
    const Expr& getRight() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitBinaryExpr(*this); }

private:
    Token _operator;
    std::unique_ptr<Expr> _left;
    std::unique_ptr<Expr> _right;
};

class UnaryExpr : public Expr {
public:
    explicit UnaryExpr(Token, std::unique_ptr<Expr> right);

    const Token& getOperator() const { return _operator; }
    const Expr& getRight() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitUnaryExpr(*this); }

private:
    Token _operator;
    std::unique_ptr<Expr> _right;
};

class CallExpr : public Expr {
public:
    explicit CallExpr(Token, std::vector<std::unique_ptr<Expr>> args);

    const Token& getName() const { return _funcName; }
    const std::vector<std::unique_ptr<Expr>>& getArgs() const { return _args; }
    RuntimeValue accept(Visitor& v) const override { return v.visitCallExpr(*this); }

private:
    Token _funcName;
    std::vector<std::unique_ptr<Expr>> _args;
};
