#pragma once

#include <memory>
#include <vector>
#include <string>
#include "AstNode.hpp"
#include "Expressions.hpp"
#include "token/Token.hpp"
#include "Visitor.hpp"

class Stmt : public AstNode {
public:
    virtual RuntimeValue accept(Visitor&) const = 0;
};

class VarDefinitionStmt : public Stmt {
public:
    explicit VarDefinitionStmt(Token name, std::unique_ptr<Expr> initializer);

    const Token& getName() const { return _name; }
    bool hasInitializer() const { return _initializer != nullptr; }
    const Expr& getInitializer() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitVarDefinitionStmt(*this); }

private:
    Token _name;
    std::unique_ptr<Expr> _initializer;
};

class ReassignStmt : public Stmt {
public:
    explicit ReassignStmt(Token name, std::unique_ptr<Expr> value);

    const Token& getName() const { return _name; }
    const Expr& getValue() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitReassignStmt(*this); }

private:
    Token _name;
    std::unique_ptr<Expr> _value;
};

class BlockStmt : public Stmt {
public:
    BlockStmt() = default;
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements);

    const std::vector<std::unique_ptr<Stmt>>& getStatements() const { return _statements; }

    RuntimeValue accept(Visitor& v) const override { return v.visitBlockStmt(*this); }

private:
    std::vector<std::unique_ptr<Stmt>> _statements{};
};

struct ElseIfClause {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

class IfStmt : public Stmt {
public:
    explicit IfStmt(std::unique_ptr<Expr> condition, 
                    std::unique_ptr<Stmt> thenBlock,
                    std::vector<ElseIfClause> elseIfs = std::vector<ElseIfClause>{},
                    std::unique_ptr<Stmt> elseBlock = nullptr);

    const Expr& getCondition() const;
    const Stmt& getThenBlock() const;
    const std::vector<ElseIfClause>& getElseIfClauses() const { return _elseIfs; }
    const Stmt& getElseBlock() const;
    bool hasElseBlock() const { return _elseBlock != nullptr; }
    RuntimeValue accept(Visitor& v) const override { return v.visitIfStmt(*this); }

private:
    std::unique_ptr<Expr> _condition;
    std::unique_ptr<Stmt> _thenBlock;
    std::vector<ElseIfClause> _elseIfs;
    std::unique_ptr<Stmt> _elseBlock;
};

class LoopStmt : public Stmt {
public:
    explicit LoopStmt(std::unique_ptr<Stmt> body);

    const Stmt& getBody() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitLoopStmt(*this); }

private:
    std::unique_ptr<Stmt> _body;
};

class RepeatStmt : public Stmt {
public:
    explicit RepeatStmt(std::unique_ptr<Expr> count, std::unique_ptr<Stmt> body);

    const Expr& getCount() const;
    const Stmt& getBody() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitRepeatStmt(*this); }

private:
    std::unique_ptr<Expr> _count;
    std::unique_ptr<Stmt> _body;
};

class PrintStmt : public Stmt {
public:
    explicit PrintStmt(std::unique_ptr<Expr> expression);

    const Expr& getExpression() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitPrintStmt(*this); }

private:
    std::unique_ptr<Expr> _expression;
};

class ReturnStmt : public Stmt {
public:
    explicit ReturnStmt(std::unique_ptr<Expr> value);

    // TODO consider std::optional to avoid hasValue/getValue boilerplate, look at other examples
    const Expr& getValue() const;
    bool hasValue() const { return _value != nullptr; }
    RuntimeValue accept(Visitor& v) const override { return v.visitReturnStmt(*this); }

private:
    std::unique_ptr<Expr> _value;
};

class BreakStmt : public Stmt {
public:
    RuntimeValue accept(Visitor& v) const override { return v.visitBreakStmt(*this); }
};

class FunctionStmt : public Stmt {
public:
    explicit FunctionStmt(Token name, 
                         std::vector<Token> parameters,
                         std::unique_ptr<Stmt> body);

    const Token& getName() const { return _name; }
    const std::vector<Token>& getParameters() const { return _parameters; }
    const Stmt& getBody() const;
    RuntimeValue accept(Visitor& v) const override { return v.visitFunctionStmt(*this); }

private:
    Token _name;
    std::vector<Token> _parameters;
    std::unique_ptr<Stmt> _body;
};

class ExpressionStmt : public Stmt {
public:
    explicit ExpressionStmt(std::unique_ptr<Expr>);

    const Expr& getExpression() const { return *_expression; }
    RuntimeValue accept(Visitor& v) const override { return v.visitExpressionStmt(*this); }

private:
    std::unique_ptr<Expr> _expression;
};
