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
