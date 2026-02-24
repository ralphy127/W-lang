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
    if (match(Token::Type::Return)) {
        LOG_DEBUG << "Detected Return statement";
        return parseReturnStatement();
    }

    LOG_DEBUG << "No statement matched in parseStatement()";
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parseDefinition() {
    LOG_DEBUG << "parseDefinition() called at token index " << _current;
    if (matchAndAdvanceIfNeeded(Token::Type::Func)) {
        return parseFunctionDefinition();
    }
    if (matchAndAdvanceIfNeeded(Token::Type::Var)) {
        return parseVarDefinition();
    }
    LOG_DEBUG << "No definition matched at current token index: " << _current;
    return nullptr;
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
    
    if (matchAndAdvanceIfNeeded(Token::Type::Assign)) {
        const auto& valueToken = getToken();
        if (not valueToken.isLiteral()) {
            throwParserException("Expected a literal after 'about'");
        }
        return std::make_unique<VarDefinitionStmt>(
            nameToken, std::make_unique<LiteralExpr>(valueToken));
    }

    if (matchAndAdvanceIfNeeded(Token::Type::Semi)) {
        return std::make_unique<VarDefinitionStmt>(nameToken, nullptr);
    }

    throwParserException("Variable definition parsing error");
    return nullptr;
}

std::unique_ptr<Stmt> Parser::parseBlock(const std::string& blockIdent) {
    LOG_DEBUG << std::format("Parsing block ({}) starting at token index", blockIdent, _current);
    consume(Token::Type::LBrace, "Expected '{' opening block");
    std::vector<std::unique_ptr<Stmt>> statements{};
    while (not match(Token::Type::RBrace) and not parsedAll()) {
        if (match(Token::Type::Return)) {
            statements.emplace_back(parseReturnStatement());
        }
    }
    consume(Token::Type::RBrace, "Expected '}' closing block");
    LOG_DEBUG << "Block parsed with " << statements.size() << " statement(s)";
    return std::make_unique<BlockStmt>(std::move(statements));
}

std::unique_ptr<Stmt> Parser::parseReturnStatement() {
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

std::unique_ptr<Expr> Parser::parseExpression() {
    LOG_DEBUG << "parseExpression() called at token index" << _current;
    if (auto expr = parsePrimary()) {
        LOG_DEBUG << "Expression parsed successfully";
        return std::move(expr);
    }

    LOG_WARN << "Expected an expression at token index" << _current << " - returning null";
    return nullptr;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    LOG_DEBUG << "parsePrimary() called at token index" << _current;
    const bool isLiteralOrLanguageConstant =
        match(Token::Type::Int) or
        match(Token::Type::Float) or
        match(Token::Type::String) or
        match(Token::Type::True) or
        match(Token::Type::False) or
        match(Token::Type::Null);
    if (isLiteralOrLanguageConstant) {
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
