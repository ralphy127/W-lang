#pragma once

#include <variant>
#include <memory>
#include <vector>
#include "AstNode.hpp"
#include "token/Token.hpp"

class Expr : public AstNode {
};

class LiteralExpr : public Expr {
public:
    explicit LiteralExpr(Token);

    const Token& getLiteral() const { return _literal; }

private:
    Token _literal;
};

class VariableExpr : public Expr {
public:
    explicit VariableExpr(Token);

    const Token& getName() const { return _variableName; }

private:
    Token _variableName;
};

class BinaryExpr : public Expr {
public:
    explicit BinaryExpr(std::unique_ptr<Expr> left, Token, std::unique_ptr<Expr> right);

    const Token& getOperator() const { return _operator; }
    const Expr& getLeft() const;
    const Expr& getRight() const;

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
private:
    Token _operator;
    std::unique_ptr<Expr> _right;
};

class CallExpr : public Expr {
public:
    explicit CallExpr(Token, std::vector<std::unique_ptr<Expr>> args);

    const Token& getName() const { return _funcName; }
    const std::vector<std::unique_ptr<Expr>>& getArgs() const { return _args; }

private:
    Token _funcName;
    std::vector<std::unique_ptr<Expr>> _args;
};
