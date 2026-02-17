#pragma once

#include <string>
#include <cstdint>
#include <expected>
#include <token/Token.hpp>

enum class LexerErrorType {
    UnterminatedString,
    UnterminatedBlockComment,
};

struct LexerError {
    std::uint32_t line;
    std::uint32_t column;
    LexerErrorType type;
};

class Lexer {
public:
    Lexer(std::string source);

    std::expected<Token, LexerError> getTokenAndAdvance();
    bool tokenizedAll() const { return _pos >= _source.size(); }

private:
    char getChar() const;
    void advance(char ch);
    char getCharAndAdvance();
    void skipWhitespaces();
    std::expected<void, LexerError> skipComments();
    bool match(char expected);
    bool matchAndAdvanceIfNeeded(std::string_view expected);

    std::string _source;
    std::uint64_t _pos{0ull};
    std::uint32_t _line{1u};
    std::uint32_t _col{1u};
};
