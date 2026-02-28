#include "Statements.hpp"
#include <cassert>

VarDefinitionStmt::VarDefinitionStmt(Token name, std::unique_ptr<Expr> initializer)
    : _name{std::move(name)}
    , _initializer{std::move(initializer)} {
    
    assert(_name.valueIs<std::string>() && "VarDefinitionStmt must hold a string token for name");
}

const Expr& VarDefinitionStmt::getInitializer() const {
    assert(_initializer.get() && "VardDefinitionStmt initializer is null");
    return *_initializer;
}

AssignStmt::AssignStmt(Token name, std::unique_ptr<Expr> value)
    : _name{std::move(name)}
    , _value{std::move(value)} {
    
    assert(_name.valueIs<std::string>() && "AssignStmt must hold a string token for name");
    assert(_value.get() && "AssignStmt value is null");
}

const Expr& AssignStmt::getValue() const {
    assert(_value.get() && "AssignStmt value is null");
    return *_value;
}

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
    : _statements{std::move(statements)} {
    
    for (const auto& statement : _statements) {
        assert(statement.get() && "BlockStmt contains null statement");
    }
}

IfStmt::IfStmt(std::unique_ptr<Expr> condition, 
               std::unique_ptr<Stmt> thenBlock,
               std::vector<ElseIfClause> elseIfs,
               std::unique_ptr<Stmt> elseBlock)
    : _condition{std::move(condition)}
    , _thenBlock{std::move(thenBlock)}
    , _elseIfs{std::move(elseIfs)}
    , _elseBlock{std::move(elseBlock)} {
    
    assert(_condition.get() && "IfStmt condition is null");
    assert(_thenBlock.get() && "IfStmt then block is null");
    for (const auto& elseIfClause : _elseIfs) {
        assert(elseIfClause.condition.get() && "IfStmt else-if clause condition is null");
        assert(elseIfClause.body.get() && "IfStmt else-if clause body is null");
    }
}

const Expr& IfStmt::getCondition() const {
    assert(_condition.get() && "IfStmt condition is null");
    return *_condition;
}

const Stmt& IfStmt::getThenBlock() const {
    assert(_thenBlock.get() && "IfStmt then block is null");
    return *_thenBlock;
}

const Stmt& IfStmt::getElseBlock() const {
    assert(_elseBlock.get() && "IfStmt else block is null");
    return *_elseBlock;
}

LoopStmt::LoopStmt(std::unique_ptr<Stmt> body)
    : _body{std::move(body)} {
    
    assert(_body.get() && "LoopStmt body is null");
}

const Stmt& LoopStmt::getBody() const {
    assert(_body.get() && "LoopStmt body is null");
    return *_body;
}

RepeatStmt::RepeatStmt(std::unique_ptr<Expr> count, std::unique_ptr<Stmt> body)
    : _count{std::move(count)}
    , _body{std::move(body)} {
    
    assert(_count.get() && "RepeatStmt count is null");
    assert(_body.get() && "RepeatStmt body is null");
}

const Expr& RepeatStmt::getCount() const {
    assert(_count.get() && "RepeatStmt count is null");
    return *_count;
}

const Stmt& RepeatStmt::getBody() const {
    assert(_body.get() && "RepeatStmt body is null");
    return *_body;
}

PrintStmt::PrintStmt(std::unique_ptr<Expr> expression)
    : _expression{std::move(expression)} {
    
    assert(_expression.get() && "PrintStmt expression is null");
}

const Expr& PrintStmt::getExpression() const {
    assert(_expression.get() && "PrintStmt expression is null");
    return *_expression;
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> value)
    : _value{std::move(value)} {
}

const Expr& ReturnStmt::getValue() const {
    assert(_value.get() && "ReturnStmt value is null");
    return *_value;
}

FunctionStmt::FunctionStmt(Token name, std::vector<Token> parameters, std::unique_ptr<Stmt> body)
    : _name{std::move(name)}
    , _parameters{std::move(parameters)}
    , _body{std::move(body)} {
    
    assert(_name.valueIs<std::string>() && "FunctionStmt must hold a string token for name");
    assert(_body.get() && "FunctionStmt body is null");
}

const Stmt& FunctionStmt::getBody() const {
    assert(_body.get() && "FunctionStmt body is null");
    return *_body;
}

ExpressionStmt::ExpressionStmt(std::unique_ptr<Expr> expression)
    : _expression{std::move(expression)} {

    assert(_expression.get() && "ExpressionStmt expression is null");
}