#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <variant>

class Token {
public:
    using Value = std::variant<std::monostate, std::int64_t, double>;

    enum class Type {
        Eof,
        Unknown,

        Fn,
        Var,
        Print,
        Return,
        If,
        Elif,
        Else,
        Greater,

        Ident,
        Int,
        Float,

        LParen,   // (
        RParen,   // )
        LBrace,   // {
        RBrace,   // }
        Semi,     // ;
        Assign,   // =
        Arrow     // =>
    };

    explicit Token(
        Type type,
        std::string text,
        std::uint32_t line,
        std::uint32_t column,
        Value val = std::monostate{});

    Type getType() const noexcept { return _type; }
    std::uint32_t getLine() const noexcept { return _line; }
    std::uint32_t getColumn() const noexcept { return _column; }
    const std::string& getText() const noexcept { return _text; }

    template<typename T>
    bool valueIs() const noexcept { return std::holds_alternative<T>(_value); }

    template<typename T>
    T getValue() const { return std::get<T>(_value); }
private:
    Value _value;
    std::string _text;
    Type _type;
    std::uint32_t _line;
    std::uint32_t _column;
};
