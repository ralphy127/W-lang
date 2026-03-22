#include "Statements.hpp"
#include <cassert>

VarDefinitionStmt::VarDefinitionStmt(
    Token name,
    std::unique_ptr<Expr> initializer,
    SourceRange srcRange)
    : Stmt{srcRange}
    , _name{std::move(name)}
    , _initializer{std::move(initializer)} {
    
    assert(_name.valueIs<std::string>());
}

const Expr& VarDefinitionStmt::getInitializer() const {
    assert(_initializer.get());
    return *_initializer;
}

ReassignStmt::ReassignStmt(Token name, std::unique_ptr<Expr> value, SourceRange srcRange)
    : Stmt{srcRange}
    , _name{std::move(name)}
    , _value{std::move(value)} {
    
    assert(_name.valueIs<std::string>());
    assert(_value.get());
}

const Expr& ReassignStmt::getValue() const {
    assert(_value.get());
    return *_value;
}

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements, SourceRange srcRange)
    : Stmt{srcRange}
    , _statements{std::move(statements)} {
    
    for (const auto& statement : _statements) {
        assert(statement.get());
    }
}

IfStmt::IfStmt(SourceRange srcRange,
               std::unique_ptr<Expr> condition, 
               std::unique_ptr<Stmt> thenBlock,
               std::vector<ElseIfClause> elseIfs,
               std::unique_ptr<Stmt> elseBlock)
    : Stmt{srcRange}
    , _condition{std::move(condition)}
    , _thenBlock{std::move(thenBlock)}
    , _elseIfs{std::move(elseIfs)}
    , _elseBlock{std::move(elseBlock)} {
    
    assert(_condition.get());
    assert(_thenBlock.get());
    for (const auto& elseIfClause : _elseIfs) {
        assert(elseIfClause.condition.get());
        assert(elseIfClause.body.get());
    }
}

const Expr& IfStmt::getCondition() const {
    assert(_condition.get());
    return *_condition;
}

const Stmt& IfStmt::getThenBlock() const {
    assert(_thenBlock.get());
    return *_thenBlock;
}

const Stmt& IfStmt::getElseBlock() const {
    assert(_elseBlock.get());
    return *_elseBlock;
}

LoopStmt::LoopStmt(std::unique_ptr<Stmt> body, SourceRange srcRange)
    : Stmt{srcRange}
    , _body{std::move(body)} {
    
    assert(_body.get());
}

const Stmt& LoopStmt::getBody() const {
    assert(_body.get());
    return *_body;
}

RepeatStmt::RepeatStmt(
    std::unique_ptr<Expr> count,
    std::unique_ptr<Stmt> body,
    SourceRange srcRange)
    : Stmt{srcRange}
    , _count{std::move(count)}
    , _body{std::move(body)} {
    
    assert(_count.get());
    assert(_body.get());
}

const Expr& RepeatStmt::getCount() const {
    assert(_count.get());
    return *_count;
}

const Stmt& RepeatStmt::getBody() const {
    assert(_body.get());
    return *_body;
}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> value, SourceRange srcRange)
    : Stmt{srcRange}
    , _value{std::move(value)} {
}

const Expr& ReturnStmt::getValue() const {
    assert(_value.get());
    return *_value;
}

FunctionStmt::FunctionStmt(
    Token name,
    std::vector<Token> parameters,
    std::unique_ptr<Stmt> body,
    SourceRange srcRange)
    : Stmt{srcRange}
    , _name{std::move(name)}
    , _parameters{std::move(parameters)}
    , _body{std::move(body)} {
    
    assert(_name.valueIs<std::string>());
    assert(_body.get());
}

const Stmt& FunctionStmt::getBody() const {
    assert(_body.get());
    return *_body;
}

ExpressionStmt::ExpressionStmt(std::unique_ptr<Expr> expression, SourceRange srcRange)
    : Stmt{srcRange}
    , _expression{std::move(expression)} {

    assert(_expression.get());
}

ImportStmt::ImportStmt(Token moduleName, SourceRange srcRange)
    : Stmt{srcRange}
    , _moduleName{std::move(moduleName)} {

    assert(_moduleName.getType() == Token::Type::Ident);
}