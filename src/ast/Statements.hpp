#pragma once

#include <memory>
#include <vector>
#include <string>
#include "AstNode.hpp"
#include "Expressions.hpp"
#include "token/Token.hpp"

class Stmt : public AstNode {
};

class VarDefinitionStmt : public Stmt {
public:
    explicit VarDefinitionStmt(Token name, std::unique_ptr<Expr> initializer);

    const Token& getName() const { return _name; }
    bool hasInitializer() const { return _initializer != nullptr; }
    const Expr& getInitializer() const;

private:
    Token _name;
    std::unique_ptr<Expr> _initializer;
};

class AssignStmt : public Stmt {
public:
    explicit AssignStmt(Token name, std::unique_ptr<Expr> value);

    const Token& getName() const { return _name; }
    const Expr& getValue() const;

private:
    Token _name;
    std::unique_ptr<Expr> _value;
};

class BlockStmt : public Stmt {
public:
    BlockStmt() = default;
    explicit BlockStmt(std::vector<std::unique_ptr<Stmt>> statements);

    const std::vector<std::unique_ptr<Stmt>>& getStatements() const { return _statements; }

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

private:
    std::unique_ptr<Stmt> _body;
};

class RepeatStmt : public Stmt {
public:
    explicit RepeatStmt(std::unique_ptr<Expr> count, std::unique_ptr<Stmt> body);

    const Expr& getCount() const;
    const Stmt& getBody() const;

private:
    std::unique_ptr<Expr> _count;
    std::unique_ptr<Stmt> _body;
};

class PrintStmt : public Stmt {
public:
    explicit PrintStmt(std::unique_ptr<Expr> expression);

    const Expr& getExpression() const;

private:
    std::unique_ptr<Expr> _expression;
};

class ReturnStmt : public Stmt {
public:
    explicit ReturnStmt(std::unique_ptr<Expr> value);

    const Expr& getValue() const;
    bool hasValue() const { return _value != nullptr; }

private:
    std::unique_ptr<Expr> _value;
};

class BreakStmt : public Stmt {
public:
    explicit BreakStmt() = default;
};

class FunctionStmt : public Stmt {
public:
    explicit FunctionStmt(Token name, 
                         std::vector<Token> parameters,
                         std::unique_ptr<Stmt> body);

    const Token& getName() const { return _name; }
    const std::vector<Token>& getParameters() const { return _parameters; }
    const Stmt& getBody() const;

private:
    Token _name;
    std::vector<Token> _parameters;
    std::unique_ptr<Stmt> _body;
};
