#include "Expressions.hpp"
#include <cassert>

LiteralExpr::LiteralExpr(Token token, SourceRange srcRange)
    : Expr{srcRange}
    , _literal{std::move(token)} {

    assert(_literal.isLiteral());
}

VariableExpr::VariableExpr(Token token, SourceRange srcRange)
    : Expr{srcRange}
    , _variableName{std::move(token)} {

    assert(token.getType() == Token::Type::Ident and _variableName.valueIs<std::string>());
}

BinaryExpr::BinaryExpr(
    std::unique_ptr<Expr> left,
    Token token,
    std::unique_ptr<Expr> right,
    SourceRange srcRange)
    : Expr{srcRange}
    , _operator{std::move(token)}
    , _left{std::move(left)}
    , _right{std::move(right)} {
    
    assert(_operator.isOperator());
    assert(_left.get());
    assert(_right.get());
}

UnaryExpr::UnaryExpr(Token token, std::unique_ptr<Expr> right, SourceRange srcRange)
    : Expr{srcRange}
    , _operator{std::move(token)}
    , _right{std::move(right)} {

    assert(_operator.getType() == Token::Type::Incr);
    assert(_right.get());
}

const Expr& UnaryExpr::getRight() const {
    assert(_right.get());
    return *_right;
}

CallExpr::CallExpr(
    std::unique_ptr<Expr> callee,
    std::vector<std::unique_ptr<Expr>> args,
    SourceRange srcRange)
    : Expr{srcRange}
    , _callee{std::move(callee)}
    , _args{std::move(args)} {

    assert(_callee != nullptr);
}

DotExpr::DotExpr(std::unique_ptr<Expr> left, Token right, SourceRange srcRange)
    : Expr{srcRange}
    , _left{std::move(left)}
    , _right{std::move(right)}
{
    assert(_left != nullptr);
    assert(_right.getType() == Token::Type::Ident);
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
    , _right{std::move(right)} {
    
    assert(_operator.isLogicalOperator());
    assert(_left.get());
    assert(_right.get());
}
