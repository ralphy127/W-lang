#include "Statements.hpp"

VarDefinitionStmt::VarDefinitionStmt(
    Token name,
    std::unique_ptr<Expr> initializer,
    SourceRange srcRange)
    : Stmt{srcRange}
    , _name{std::move(name)}
    , _initializer{std::move(initializer)} {}

ReassignStmt::ReassignStmt(std::unique_ptr<Expr> target, std::unique_ptr<Expr> value, SourceRange srcRange)
    : Stmt{srcRange}
    , _target{std::move(target)}
    , _value{std::move(value)} {}

BlockStmt::BlockStmt(std::vector<std::unique_ptr<Stmt>> statements, SourceRange srcRange)
    : Stmt{srcRange}
    , _statements{std::move(statements)} {}

IfStmt::IfStmt(
    SourceRange srcRange,
    std::unique_ptr<Expr> condition, 
    std::unique_ptr<Stmt> thenBlock,
    std::vector<ElseIfClause> elseIfs,
    std::unique_ptr<Stmt> elseBlock)
    : Stmt{srcRange}
    , _condition{std::move(condition)}
    , _thenBlock{std::move(thenBlock)}
    , _elseIfs{std::move(elseIfs)}
    , _elseBlock{std::move(elseBlock)} {}

std::optional<std::reference_wrapper<const Stmt>> IfStmt::getElseBlock() const {
    return _elseBlock ? std::optional{std::cref(*_elseBlock)} : std::nullopt;
}

LoopStmt::LoopStmt(std::unique_ptr<Stmt> body, SourceRange srcRange)
    : Stmt{srcRange}
    , _body{std::move(body)} {}


RepeatStmt::RepeatStmt(
    std::unique_ptr<Expr> count,
    std::unique_ptr<Stmt> body,
    SourceRange srcRange)
    : Stmt{srcRange}
    , _count{std::move(count)}
    , _body{std::move(body)} {}

ReturnStmt::ReturnStmt(std::unique_ptr<Expr> value, SourceRange srcRange)
    : Stmt{srcRange}
    , _value{std::move(value)} {
}

std::optional<std::reference_wrapper<const Expr>> ReturnStmt::getValue() const {
    return _value ? std::optional{std::cref(*_value)} : std::nullopt;
}

FunctionStmt::FunctionStmt(
    Token name,
    std::vector<Token> parameters,
    std::unique_ptr<Stmt> body,
    SourceRange srcRange)
    : Stmt{srcRange}
    , _name{std::move(name)}
    , _parameters{std::move(parameters)}
    , _body{std::move(body)} {}

ExpressionStmt::ExpressionStmt(std::unique_ptr<Expr> expression, SourceRange srcRange)
    : Stmt{srcRange}
    , _expression{std::move(expression)} {}

ImportStmt::ImportStmt(Token moduleName, SourceRange srcRange)
    : Stmt{srcRange}
    , _moduleName{std::move(moduleName)} {}
