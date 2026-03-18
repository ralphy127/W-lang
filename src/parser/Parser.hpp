#pragma once

#include <vector>
#include <memory>
#include <expected>
#include <functional>
#include "token/Token.hpp"
#include "ast/Statements.hpp"
#include "ast/Expressions.hpp"

struct ParserError {
    std::uint32_t line;
    std::uint32_t column;
    std::string message;
};

struct ParserResult {
    std::vector<std::unique_ptr<Stmt>> statements;
    std::vector<ParserError> errors;
};

class Parser {
public:
    explicit Parser(std::vector<Token>);

    ParserResult parse();

private:
    class ParserException : public std::runtime_error {
    public:
        std::uint32_t line;
        std::uint32_t column;
        explicit ParserException(std::uint32_t line, std::uint32_t column, const std::string msg);
        ~ParserException() noexcept override = default;
    };

    bool parsedAll() const { return _current >= _tokens.size(); }

    void advance() { _current++; }
    const Token& getPreviousToken() const;
    const Token& getToken() const;
    const Token& getTokenAndAdvance();
    
    bool match(Token::Type type) const { return getToken().getType() == type; }
    bool matchAndAdvanceIfNeeded(Token::Type);
    bool matchAndAdvanceIfNeeded(const std::vector<Token::Type>&);
    bool matchLookahead(Token::Type curr, Token::Type next);

    void throwParserException(const std::string& errorMessage);

    const Token& consume(Token::Type, const std::string& errorMessage);

    std::unique_ptr<Stmt> parseStatement();
    std::unique_ptr<Stmt> parseDefinition();
    std::unique_ptr<Stmt> parseFunctionDefinition();
    std::unique_ptr<Stmt> parseVarDefinition();
    std::unique_ptr<Stmt> parseBlock(std::string_view blockIdent);
    std::unique_ptr<Stmt> parseReturn();
    std::unique_ptr<Stmt> parseIf();
    std::unique_ptr<Stmt> parseLoop();
    std::unique_ptr<Stmt> parseRepeat();
    std::unique_ptr<Stmt> parseImport();
    std::unique_ptr<Stmt> parseReassign();

    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseOr();
    std::unique_ptr<Expr> parseAnd();
    std::unique_ptr<Expr> parseEquality();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parsePrimary();
    std::unique_ptr<Expr> parseTerm();
    std::unique_ptr<Expr> parseFunctionCall(std::unique_ptr<Expr> callee);
    std::unique_ptr<Expr> parseUnary();

    const std::vector<Token> _tokens;
    std::uint32_t _current{0u};
};
