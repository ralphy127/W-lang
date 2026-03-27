#pragma once

#include <string>
#include <cstdint>
#include <expected>
#include <vector>
#include <token/Token.hpp>

enum class LexerErrorType {
    UnterminatedString,
    UnterminatedBlockComment,
    UnknownToken,
    EmptySource,
};

struct LexerError {
    std::uint32_t line;
    std::uint32_t column;
    std::uint32_t length;
    LexerErrorType type;
};

struct LexerResult {
    std::vector<Token> tokens;
    std::vector<LexerError> errors;
};

class Lexer {
public:
    Lexer(std::string source, FileId);

    LexerResult tokenize();

private:
    LexerError createError(LexerErrorType, const std::string& badSource);
    bool tryTokenizeSingleChar(Token& token, char ch);
    bool tryTokenizeKeyword(Token& token);
    bool tryTokenizeNumber(Token& token, char ch);
    std::expected<void, LexerError> tryTokenizeString(Token& token);
    void tokenizeIdentifier(Token& token);
    std::expected<Token, LexerError> getTokenAndAdvance();
    bool tokenizedAll() const { return _pos >= _source.size(); }
    char getChar() const;
    void advance(char ch);
    char getCharAndAdvance();
    bool skipWhitespaces();
    bool skipMultilineComment();
    std::expected<void, LexerError> skipComments();
    bool match(char expected);
    bool matchLookahead(char expected);
    bool matchAndAdvanceIfNeeded(std::string_view expected);

    const std::string _source;
    const size_t _sourceLength;
    const FileId _fileId;
    size_t _pos{0ull};
    std::uint32_t _line{1u};
    std::uint32_t _col{1u};
};
