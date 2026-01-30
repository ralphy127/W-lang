#include "tokens/Token.hpp"

Token::Token(
    Type type,
    std::string text,
    std::uint32_t line,
    std::uint32_t column,
    Value val)
    : _value{val}
    , _text{std::move(text)}
    , _type{type}
    , _line{line}
    , _column{column} {}
