#include "Expressions.hpp"
#include <cassert>

LiteralExpr::LiteralExpr(Token token)
    : _literal{std::move(token)} {

    assert(_literal.isLiteral() && "LiteralExpr must hold a literal token");
}

VariableExpr::VariableExpr(Token token)
    : _variableName{std::move(token)} {

    assert(token.getType() == Token::Type::Ident && _variableName.valueIs<std::string>() && "VariableExpr must hold a ident string token");
}

BinaryExpr::BinaryExpr(std::unique_ptr<Expr> left, Token token, std::unique_ptr<Expr> right)
    : _operator{std::move(token)}
    , _left{std::move(left)}
    , _right{std::move(right)} {
    
    assert(_operator.isOperator() && "BinaryExpr must hold an operator token");
    assert(_left.get() && "left side of binary expression is null");
    assert(_right.get() && "right side of binary expression is null");
}

const Expr& BinaryExpr::getLeft() const {
    assert(_left.get() && "left side of binary expression is null");
    return *_left;
}

const Expr& BinaryExpr::getRight() const {
    assert(_right.get() && "right side of binary expression is null");
    return *_right;
}

UnaryExpr::UnaryExpr(Token token, std::unique_ptr<Expr> right)
    : _operator{std::move(token)}
    , _right{std::move(right)} {

    assert(_operator.getType() == Token::Type::Incr && "UnaryExpr must hold unary operator token");
    assert(_right.get() && "right side of unary expression is null");
}

const Expr& UnaryExpr::getRight() const {
    assert(_right.get() && "right side of unary expression is null");
    return *_right;
}

CallExpr::CallExpr(std::unique_ptr<Expr> callee, std::vector<std::unique_ptr<Expr>> args)
    : _callee{std::move(callee)}
    , _args{std::move(args)} {

    assert(_callee != nullptr && "CallExpr callee is null");
    for (const auto& arg : _args) {
        assert(arg.get() && "CallExpr argument is null");
    }
}

DotExpr::DotExpr(std::unique_ptr<Expr> left, Token right)
    : _left{std::move(left)}
    , _right{std::move(right)}
{
    assert(_left != nullptr && "DotExpr lhs is null");
    assert(_right.getType() == Token::Type::Ident && "DotExpr rhs must be an ident token");
}
