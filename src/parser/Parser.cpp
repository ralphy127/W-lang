#include "Parser.hpp"
#include "utils/Logging.hpp"
#include <cassert>
#include <format>

Parser::Parser(std::vector<Token> tokens)
    : _tokens{std::move(tokens)} {}

ParserResult Parser::parse() {
    LOG_INFO << "Starting parse with " << _tokens.size() << " tokens";
    std::vector<std::unique_ptr<Stmt>> statements;
    std::vector<ParserError> errors;

    while (not parsedAll()) {
        try {
            if (auto statement = parseDefinition()) {
                statements.push_back(std::move(statement));
                LOG_DEBUG << "Successfully parsed definition, total statements: " << statements.size();
            }
            else {
                LOG_WARN << "parseDefinition() returned nullptr, stopping parse loop";
                break;
            }
        }
        catch(const ParserException& e) {
            LOG_ERROR << "Parser error at line " << e.line << ", column " << e.column << ": " << e.what();
            errors.emplace_back(e.line, e.column, e.what());
        }
        catch(const std::exception& e) {
            LOG_ERROR << "Unexpected exception: " << e.what();
            const auto& token = getToken();
            errors.emplace_back(token.getLine(), token.getColumn(), e.what());
        }

        // TODO synchronize
    }

    LOG_INFO << "Parse completed: " << statements.size() << " statements, " << errors.size() << " errors";
    return {std::move(statements), std::move(errors)};
}

Parser::ParserException::ParserException(std::uint32_t line, std::uint32_t column, const std::string msg)
    : std::runtime_error{msg}
    , line{line}
    , column{column} {}

const Token& Parser::getPreviousToken() const {
    assert(_current > 0u);
    return _tokens[_current - 1u];
}

const Token& Parser::getToken() const {
    assert(not parsedAll());
    return _tokens[_current];
}

const Token& Parser::getTokenAndAdvance() {
    advance();
    return getPreviousToken();
}

bool Parser::matchAndAdvanceIfNeeded(Token::Type type) {
    if (match(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::matchAndAdvanceIfNeeded(const std::vector<Token::Type>& types) {
    const auto startPos = _current;
    for (const auto type : types) {
        if (not match(type)) {
            _current = startPos;
            return false;
        }
        advance();
    }
    return true;
}

void Parser::throwParserException(const std::string& errorMessage) {
    const auto& token = getToken();
    const auto line = token.getLine();
    const auto column = token.getColumn();
    throw ParserException{line, column, errorMessage};
}

const Token& Parser::consume(
    Token::Type type,
    const std::string& errorMessage) {

    if (not match(type)) {
        throwParserException(errorMessage);
    }
    LOG_DEBUG << std::format("Consuming token: {} at token index: {}", toString(type), _current);
    return getTokenAndAdvance();
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    LOG_DEBUG << "parseStatement() called at token index " << _current;

    if (parsedAll()) {
        LOG_DEBUG << "No more tokens to parse";
        return nullptr;
    }

    if (matchAndAdvanceIfNeeded(Token::Type::If)) { 
        LOG_DEBUG << "Detected If statement";
        return parseIf();
    }
    if (matchAndAdvanceIfNeeded(Token::Type::Loop)) {
        LOG_DEBUG << "Detected loop statement";
        return parseLoop();
    }
    if (matchAndAdvanceIfNeeded(Token::Type::Repeat)) {
        LOG_DEBUG << "Detected repeat statement";
        return parseRepeat();
    }
    if (match(Token::Type::Return)) {
        LOG_DEBUG << "Detected Return statement";
        return parseReturn();
    }

    LOG_WARN << "No statement matched in parseStatement()";
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parseDefinition() {
    LOG_DEBUG << "parseDefinition() called at token index " << _current;
    if (parsedAll()) {
        LOG_DEBUG << "No more tokens to parse";
        return nullptr;
    }

    if (matchAndAdvanceIfNeeded(Token::Type::Func)) {
        return parseFunctionDefinition();
    }
    if (matchAndAdvanceIfNeeded(Token::Type::Var)) {
        return parseVarDefinition();
    }

    return parseStatement();
}

std::unique_ptr<Stmt> Parser::parseFunctionDefinition() {
    LOG_DEBUG << "Parsing function definition starting at token index " << _current;
    const auto& nameToken = consume(Token::Type::Ident, "Expected function name after 'gig'");
    if (not nameToken.valueIs<std::string>()) {
        throwParserException("Expected string as a function name");
    }
    const auto& name = nameToken.getValue<std::string>();
    LOG_DEBUG << "Function name: " << name;

    consume(Token::Type::LParen, "Expected '(' after function name");

    std::vector<Token> parameters{};
    if (not match(Token::Type::RParen)) {
        LOG_DEBUG << "Parsing function parameters";
        do {
            parameters.push_back(consume(Token::Type::Ident, "Expected  function parameter name"));
            LOG_DEBUG << "Added function parameter: " << parameters.back().getValue<std::string>();
        }
        while (matchAndAdvanceIfNeeded(Token::Type::Comma));
    }

    LOG_DEBUG << "Function has " << parameters.size() << " parameter(s)";
    consume(Token::Type::RParen, "Expected ')' after function parameters");
    auto body = parseBlock("function");
    LOG_DEBUG << "Successfully parsed function definition for '" << name << "'";
    return std::make_unique<FunctionStmt>(nameToken, std::move(parameters), std::move(body));
}

std::unique_ptr<Stmt> Parser::parseVarDefinition() {
    LOG_DEBUG << "Parsing variable definition starting at token index " << _current;
    const auto& nameToken = consume(Token::Type::Ident, "Expected variable name after 'stash'");
    if (not nameToken.valueIs<std::string>()) {
        throwParserException("Expected string as a function name");
    }

    std::unique_ptr<Expr> initializer{nullptr};
    
    if (matchAndAdvanceIfNeeded(Token::Type::Assign)) {
        initializer = parseExpression();
    }

    consume(Token::Type::Semi, "Expected '...' after variable definition");
    return std::make_unique<VarDefinitionStmt>(nameToken, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::parseBlock(const std::string& blockIdent) {
    LOG_DEBUG << std::format("Parsing block ({}) starting at token index", blockIdent, _current);
    consume(Token::Type::LBrace, "Expected '{' opening block");
    std::vector<std::unique_ptr<Stmt>> statements{};
    while (not match(Token::Type::RBrace) and not parsedAll()) {
        statements.push_back(parseDefinition());
    }
    consume(Token::Type::RBrace, "Expected '}' closing block");
    LOG_DEBUG << "Block parsed with " << statements.size() << " statement(s)";
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::parseReturn() {
    consume(Token::Type::Return, "Expected return token");
    LOG_DEBUG << "Parsing return statement at token index" << _current;
    if (match(Token::Type::Semi)) {
        const auto& nextToken = getToken();
        LOG_DEBUG << "Return statement with no value (implicit null)";
        return std::make_unique<ReturnStmt>(std::make_unique<LiteralExpr>(
            Token{Token::Type::Null, nextToken.getLine(), nextToken.getColumn()}));
    }

    LOG_DEBUG << "Parsing return value expression";
    auto value = parseExpression();
    if (not value) {
        throwParserException("Expected an expression after 'yeet'");
    }
    consume(Token::Type::Semi, "Expected '...' after return value");
    LOG_DEBUG << "Return statement parsed successfully";
    return std::make_unique<ReturnStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::parseIf() {
    LOG_DEBUG << "Parsing 'perhaps' statement";

    consume(Token::Type::LParen, "Expected '(' after 'perhaps'");
    auto ifCondition = parseExpression();
    consume(Token::Type::RParen, "Expected ')' after condition");
    auto ifBody = parseBlock("perhaps");

    std::vector<ElseIfClause> elIfClauses{};
    while (not parsedAll() and matchAndAdvanceIfNeeded(Token::Type::Elif)) {
        consume(Token::Type::LParen, "Expected '(' after 'or_whatever'");
        auto elifCondition = parseExpression();
        consume(Token::Type::RParen, "Expected ')' after condition");
        auto elifBody = parseBlock("or_whatever");

        elIfClauses.emplace_back(std::move(elifCondition), std::move(elifBody));
    }

    std::unique_ptr<Stmt> elseClause{nullptr};
    if (not parsedAll() and matchAndAdvanceIfNeeded(Token::Type::Else)) {
        elseClause = parseBlock("screw_it");
    }

    return std::make_unique<IfStmt>(
        std::move(ifCondition),
        std::move(ifBody),
        std::move(elIfClauses),
        std::move(elseClause)
    );
}

std::unique_ptr<Stmt> Parser::parseLoop() {
    LOG_DEBUG << "Parsing 'do_until_bored' statement";
    return std::make_unique<LoopStmt>(parseBlock("do_until_bored"));
}

std::unique_ptr<Stmt> Parser::parseRepeat() {
    LOG_DEBUG << "Parsing 'repeat' statement";
    consume(Token::Type::LParen, "Expected '(' after 'repeat'");
    auto countExpr = parseExpression();
    if (not countExpr) {
        throwParserException("Expected loop count expression inside '()'");
    }

    consume(Token::Type::RParen, "Expected ')' after loop count expression");
    auto body = parseBlock("spin_around");

    LOG_DEBUG << "Successfully parsed 'repeat' statement";
    return std::make_unique<RepeatStmt>(std::move(countExpr), std::move(body));
}

std::unique_ptr<Expr> Parser::parseExpression() {
    LOG_DEBUG << "parseExpression() called at token index" << _current;
    if (auto expr = parseEquality()) {
        LOG_DEBUG << "Expression parsed successfully";
        return std::move(expr);
    }
    if (auto expr = parsePrimary()) {
        LOG_DEBUG << "Expression parsed successfully";
        return std::move(expr);
    }

    LOG_WARN << "Expected an expression at token index" << _current << " - returning null";
    return nullptr;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    LOG_DEBUG << "parsePrimary() called at token index" << _current;
    const bool isLiteral =
        match(Token::Type::Int) or
        match(Token::Type::Float) or
        match(Token::Type::String) or
        match(Token::Type::True) or
        match(Token::Type::False) or
        match(Token::Type::Null);
    if (isLiteral) {
        const auto& token = getToken();
        LOG_DEBUG << "Parsed literal expression: " << toString(token.getType());
        return std::make_unique<LiteralExpr>(getTokenAndAdvance());
    }

    if (match(Token::Type::Ident)) {
        const auto& token = getToken();
        LOG_DEBUG << "Parsed variable expression: " << token.getValue<std::string>();
        return std::make_unique<VariableExpr>(getTokenAndAdvance());
    }

    if (match(Token::Type::LParen)) {
        LOG_DEBUG << "Parsing grouped expression";
        auto expr = parseExpression();
        consume(Token::Type::RParen, "Expected ) after expression");
        return expr;
    }

    LOG_DEBUG << "No primary expression matched at current token";
    return nullptr;
}

// TODO possible refactor of binary expressions parsing
std::unique_ptr<Expr> Parser::parseEquality() {
    LOG_DEBUG << "parseEquality() called at token index" << _current;
    auto left = parseComparison();

    if (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType == Token::Type::Equal or tokenType == Token::Type::NotEqual) {
            advance();
            auto right = parseComparison();
            if (not right) {
                throwParserException(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            LOG_DEBUG << "Parsed equality binary expression successfully";
            return make_unique<BinaryExpr>(std::move(left), token, std::move(right));
        }
    }

    LOG_DEBUG << "No equality operator matched, returning left operand";
    return left;
}

std::unique_ptr<Expr> Parser::parseComparison() {
    LOG_DEBUG << "parseComparison() called at token index" << _current;
    auto left = parseTerm();

    if (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType == Token::Type::Greater or tokenType == Token::Type::Less) {
            advance();
            auto right = parseTerm();
            if (not right) {
                throwParserException(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            LOG_DEBUG << "Parsed comparison binary expression successfully";
            return std::make_unique<BinaryExpr>(std::move(left), token, std::move(right));
        }
    }

    LOG_DEBUG << "No comparison operator matched, returning left operand";
    return left;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    LOG_DEBUG << "parseTerm() called at token index" << _current;
    auto left = parsePrimary();

    if (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        // TODO after adding * and /: add parseFactor() function to resolve priorities
        if (tokenType == Token::Type::Plus or tokenType == Token::Type::Minus) {
            advance();
            auto right = parseTerm();
            if (not right) {
                throwParserException(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            LOG_DEBUG << "Parsed term binary expression successfully";
            return std::make_unique<BinaryExpr>(std::move(left), token, std::move(right));
        }
    }

    LOG_DEBUG << "No term operator matched, returning left operand";
    return left;
}
