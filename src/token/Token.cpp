#include "Token.hpp"

Token::Token(
    Type type,
    std::uint32_t line,
    std::uint32_t column,
    Value val)
    : _value{val}
    , _type{type}
    , _line{line}
    , _column{column} {}

bool Token::isLiteral() const {
    return _type == Type::Null or
           _type == Type::Int or
           _type == Type::Float or
           _type == Type::String;
}

bool Token::isOperator() const {
    return _type == Type::Plus or
           _type == Type::Minus or
           _type == Type::Equal or
           _type == Type::NotEqual or
           _type == Type::Incr or
           _type == Type::Greater or
           _type == Type::Less;
}

bool Token::isStatementStart() const {
    return _type == Type::Var or
           _type == Type::Print or
           _type == Type::If or
           _type == Type::Elif or
           _type == Type::Else or
           _type == Type::While or
           _type == Type::Repeat or
           _type == Type::Return ;
}
