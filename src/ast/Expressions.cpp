#include "Expressions.hpp"

LiteralExpr::LiteralExpr(Token literalToken, SourceRange srcRange)
    : Expr{srcRange}
    , _literal{std::move(literalToken)} {}

VariableExpr::VariableExpr(Token token, SourceRange srcRange)
    : Expr{srcRange}
    , _variableName{std::move(token)} {}

std::optional<LValue> VariableExpr::getLValueOpt() const {
    return LValue{LValue::Variable{_variableName.getValue<std::string>()}};
}

BinaryExpr::BinaryExpr(
    std::unique_ptr<Expr> left,
    Token token,
    std::unique_ptr<Expr> right,
    SourceRange srcRange)
    : Expr{srcRange}
    , _operator{std::move(token)}
    , _left{std::move(left)}
    , _right{std::move(right)} {}

UnaryExpr::UnaryExpr(Token token, std::unique_ptr<Expr> right, SourceRange srcRange)
    : Expr{srcRange}
    , _operator{std::move(token)}
    , _right{std::move(right)} {}

CallExpr::CallExpr(
    std::unique_ptr<Expr> callee,
    std::vector<std::unique_ptr<Expr>> args,
    SourceRange srcRange)
    : Expr{srcRange}
    , _callee{std::move(callee)}
    , _args{std::move(args)} {}

DotExpr::DotExpr(std::unique_ptr<Expr> left, Token right, SourceRange srcRange)
    : Expr{srcRange}
    , _left{std::move(left)}
    , _right{std::move(right)} {}

std::optional<LValue> DotExpr::getLValueOpt() const {
    if (_right.getType() == Token::Type::Ident) {
        return LValue{LValue::Property{*_left, _right.getValue<std::string>()}};
    }
    return std::nullopt;
}

VectorExpr::VectorExpr(std::vector<std::unique_ptr<Expr>> initializers, SourceRange srcRange)
    : Expr{srcRange}
    , _initializers{std::move(initializers)} {}

LogicalExpr::LogicalExpr(
    std::unique_ptr<Expr> left,
    Token token,
    std::unique_ptr<Expr> right,
    SourceRange srcRange)
    : Expr{srcRange}
    , _operator{std::move(token)}
    , _left{std::move(left)}
    , _right{std::move(right)} {}
