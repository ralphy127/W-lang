#pragma once

#include <string>
#include <cstdint>
#include <token/Token.hpp>

class Lexer {
public:
    Lexer(std::string source);

    Token getNextTokenAndAdvance();
    bool tokenizedAll() const { return _pos >= _source.size(); }

private:
    char getNextChar() const;
    void advance(char ch);
    char getNextCharAndAdvance();
    void skipWhitespaces();
    void skipComments();
    bool match(char expected);
    bool matchAndAdvanceIfNeeded(std::string_view expected);

    std::string _source;
    std::uint64_t _pos{0ull};
    std::uint32_t _line{1u};
    std::uint32_t _col{1u};
};
