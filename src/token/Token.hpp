#pragma once

#include <cstdint>
#include <string>
#include <utility>
#include <variant>
#include <core/types.hpp>

template<typename T, typename V>
concept VariantAlternative =
    requires {
        typename std::variant_alternative_t<0, V>;
    } and
    ([]<std::size_t... Is>(std::index_sequence<Is...>) {
        return (std::same_as<std::decay_t<T>,std::variant_alternative_t<Is, V>> or ...);
    })(std::make_index_sequence<std::variant_size_v<V>>{});

class Token {
public:
    using Value = std::variant<std::monostate, std::int32_t, double, std::string>;

    enum class Type {
        Unknown,
        Eof,

        Func,
        Var,
        Return,
        If,
        Elif,
        Else,
        Break,
        Loop,
        Repeat,
        Import,

        Greater,
        Less,
        Equal,
        NotEqual,
        And,
        Or,

        Ident,

        Null,
        True,
        False,
        Int,
        Float,
        String,

        LParen,
        RParen,
        LBrace,
        RBrace,
        LBracket,
        RBracket,
        Semi,
        BrSemi,
        Colon,
        Comma,
        Dot,
        Assign,
        Reassign,

        Incr,
        Plus,
        Minus,
        Multiply,
        Divide
    };

    Token() = default;

    explicit Token(
        Type type,
        FileId fileId,
        std::uint32_t line,
        std::uint32_t column,
        Value val = std::monostate{});

    Type getType() const noexcept { return _type; }
    void setType(Type type) noexcept { _type = type; }

    FileId getFileId() const { return _fileId; }

    std::uint32_t getLine() const noexcept { return _line; }
    std::uint32_t getColumn() const noexcept { return _column; }

    template<VariantAlternative<Value> T>
    T getValue() const { return std::get<T>(_value); }
    template<VariantAlternative<Value> T>
    T& getValue() { return std::get<T>(_value); }
    template<VariantAlternative<Value> T>
    void setValue(T&& value) { _value = std::forward<T>(value); }

    template<VariantAlternative<Value> T>
    bool valueIs() const noexcept { return std::holds_alternative<T>(_value); }

    bool isLiteral() const;
    bool isOperator() const;
    bool isLogicalOperator() const;
    bool isStatementStart() const;

private:
    Value _value;
    FileId _fileId;
    Type _type;
    std::uint32_t _line;
    std::uint32_t _column;
};

std::string toString(Token::Type);
std::string toSourceString(Token::Type);
