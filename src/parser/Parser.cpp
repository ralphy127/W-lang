#include "Parser.hpp"
#include "utils/Logging.hpp"
#include <cassert>
#include <format>
#include <algorithm>

Parser::Parser(std::vector<Token> tokens)
    : _tokens{std::move(tokens)} {}

ParserResult Parser::parse() {
    const auto tokensCount = static_cast<int>(_tokens.size());
    LOG_INFO << "Starting parse with " << tokensCount << " tokens";
    std::vector<std::unique_ptr<Stmt>> statements;
    std::vector<ParserError> errors;

    const auto stringifyAllTokens = [&tokens = _tokens, tokensCount]() -> std::string {
        std::string result{"Tokens to parse: \n"};
        for (int i{0}; i < tokensCount; ++i) {
            result += std::format("{} : {}\n", i, toString(tokens[i].getType()));
        }
        return result;
    };
    LOG_DEBUG << stringifyAllTokens();

    while (not parsedAll()) {
        try {
            if (auto statement = parseDefinition()) {
                statements.push_back(std::move(statement));
                LOG_DEBUG << "Parsed definition, total statements: " << statements.size();
            }
            else {
                LOG_WARN << "parseDefinition() returned null, synchronizing";
                errors.emplace_back(getToken(), "Mystery statement");
                synchronize();
            }
        }
        catch(ParserError error) {
            LOG_ERROR << std::format("Parse error at line {}, column {}: {}",
                error.badToken.getLine(), error.badToken.getColumn(), error.msg);
            errors.emplace_back(std::move(error));
            synchronize();
        }
        catch(const std::exception& e) {
            LOG_ERROR << "Unexpected exception: " << e.what();
            errors.emplace_back(getToken(), e.what());
            synchronize();
        }
    }

    LOG_INFO << std::format(
        "Parsing complete: {} statements, {} errors", statements.size(), errors.size());
    return {std::move(statements), std::move(errors)};
}

static bool isSafeToContinue(Token::Type tokenType) {
    switch (tokenType) {
        case Token::Type::Func:
        case Token::Type::Var:
        case Token::Type::If:
        case Token::Type::Elif:
        case Token::Type::Else:
        case Token::Type::Break:
        case Token::Type::Loop:
        case Token::Type::Repeat:
        case Token::Type::Import:
        case Token::Type::Ident:
        case Token::Type::Int:
        case Token::Type::Float:
        case Token::Type::String:
        case Token::Type::True:
        case Token::Type::False:
        case Token::Type::Null:
        case Token::Type::LParen:
        case Token::Type::LBracket:
        case Token::Type::Incr:
        case Token::Type::Return:
            return true;
        default:
            return false;
    }
}

void Parser::synchronize() {
    if (parsedAll()) {
        return;
    }

    advance();
    std::uint32_t blockDepth{0u};

    while (not parsedAll()) {
        const auto tokenType = getToken().getType();

        if (tokenType == Token::Type::LBrace) {
            ++blockDepth;
            advance();
            continue;
        }
        if (tokenType == Token::Type::RBrace) {
            if (blockDepth > 0u) {
                --blockDepth;
            }
            advance();
            continue;
        }
        if (blockDepth == 0u and isSafeToContinue(tokenType)) {
            return;
        }

        advance();
    }
}

const Token& Parser::getPreviousToken() const {
    assert(_current > 0ull);
    return _tokens[_current - 1ull];
}

const Token& Parser::getToken() const {
    assert(not parsedAll());
    return _tokens[_current];
}

const Token& Parser::getTokenAndAdvance() {
    advance();
    return getPreviousToken();
}

bool Parser::matchLookahead(Token::Type curr, Token::Type next) {
    return match(curr) and
        _current + 1ull < _tokens.size() and
        next == _tokens[_current + 1ull].getType();
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
        if (not parsedAll() and not match(type)) {
            _current = startPos;
            return false;
        }
        advance();
    }
    return true;
}

void Parser::throwParserError(const std::string& errorMessage) {
    throw ParserError{getToken(), errorMessage};
}

const Token& Parser::consume(
    Token::Type type,
    const std::string& errorMessage) {

    if (not match(type)) {
        throwParserError(errorMessage);
    }
    LOG_DEBUG << std::format("Consuming token: {} at token index: {}", toString(type), _current);
    return getTokenAndAdvance();
}

const Token& Parser::consumeIdent(Token::Type type, const std::string& errorMessage) {
    const auto& identToken = consume(type, errorMessage);
    if (not identToken.valueIs<std::string>()) {
        throwParserError("Not yapping when supposed to");
    }
    return identToken;
}

SourceRange Parser::makeRange(const Token& start, const Token& end) {
    return {start.getFileId(), {start.getLine(), start.getColumn()}, {end.getLine(), end.getColumn()}};
}

SourceRange Parser::makeRange(const AstNode& start, const AstNode& end) {
    const auto& startRange = start.getSrcRange();
    return {startRange.fileId, startRange.start, end.getSrcRange().end};
}

SourceRange Parser::makeRange(const Token& start, const AstNode& end) {
    return {start.getFileId(), {start.getLine(), start.getColumn()}, end.getSrcRange().end};
}

std::unique_ptr<Stmt> Parser::parseStatement() {
    LOG_DEBUG << "parseStatement() called at token index: " << _current;
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
    if (matchAndAdvanceIfNeeded(Token::Type::Import)) {
        LOG_DEBUG << "Detected import statement";
        return parseImport();
    }
    if (matchAndAdvanceIfNeeded(Token::Type::Break)) {
        LOG_DEBUG << "Detected break statement";
        const auto& breakToken = getPreviousToken();
        const auto& semiToken = consume(Token::Type::BrSemi, "Expected '!!!' after 'rage_quit'");
        return std::make_unique<BreakStmt>(makeRange(breakToken, semiToken));
    }
    if (match(Token::Type::Return)) {
        LOG_DEBUG << "Detected Return statement";
        return parseReturn();
    }
    if (matchLookahead(Token::Type::Ident, Token::Type::Reassign)) {        
        return parseReassign();
    }

    const auto& firstToken = getToken();
    auto expr = parseExpression();
    if (not expr) {
        LOG_WARN << "Returning nullptr in parseStatement()";
        return nullptr;
    }

    const auto& semiToken = consume(Token::Type::Semi, "Expected '...' after expression");
    
    return std::make_unique<ExpressionStmt>(std::move(expr), makeRange(firstToken, semiToken));
}

std::unique_ptr<Stmt> Parser::parseDefinition() {
    LOG_DEBUG << "parseDefinition() called at token index: " << _current;
    if (matchAndAdvanceIfNeeded(Token::Type::Func)) {
        return parseFunctionDefinition();
    }
    if (matchAndAdvanceIfNeeded(Token::Type::Var)) {
        return parseVarDefinition();
    }

    return parseStatement();
}

std::unique_ptr<Stmt> Parser::parseFunctionDefinition() {
    LOG_DEBUG << "Parsing function definition starting at token index: " << _current;
    const auto& gigToken = getPreviousToken();
    const auto& nameToken = consumeIdent(Token::Type::Ident, "Expected function name after 'gig'");
    const auto& name = nameToken.getValue<std::string>();
    LOG_DEBUG << "Function name: " << name;

    consume(Token::Type::LParen, "Expected '(' after function name");

    std::vector<Token> parameters{};
    if (not match(Token::Type::RParen)) {
        LOG_DEBUG << "Parsing function parameters";
        do {
            parameters.push_back(consume(Token::Type::Ident, "Expected function parameter name"));
            LOG_DEBUG << "Added function parameter: " << parameters.back().getValue<std::string>();
        }
        while (matchAndAdvanceIfNeeded(Token::Type::Comma));
    }
    LOG_DEBUG << "Function has " << parameters.size() << " parameter(s)";

    consume(Token::Type::RParen, "Expected ')' after function parameters");

    auto body = parseBlock("gig");
    auto srcRange = makeRange(gigToken, *body);

    LOG_DEBUG << "Successfully parsed function definition for '" << name << "'";
    return std::make_unique<FunctionStmt>(
        nameToken, std::move(parameters), std::move(body), srcRange);
}

std::unique_ptr<Stmt> Parser::parseVarDefinition() {
    LOG_DEBUG << "Parsing variable definition starting at token index: " << _current;
    const auto& stashToken = getPreviousToken();
    const auto& nameToken = consumeIdent(
        Token::Type::Ident,
        "Expected variable name after 'stash'");

    std::unique_ptr<Expr> initializer{nullptr};
    
    if (matchAndAdvanceIfNeeded(Token::Type::Assign)) {
        initializer = parseExpression();
    }
    else {
        consume(Token::Type::Semi, "Missing 'about' in variable definition");
    }

    const auto& semiToken = consume(Token::Type::Semi, "Expected '...' after variable definition");
    LOG_DEBUG << std::format(
        "Successfully parsed variable definition for '{}'", nameToken.getValue<std::string>());
    return std::make_unique<VarDefinitionStmt>(
        nameToken, std::move(initializer), makeRange(stashToken, semiToken));
}

std::unique_ptr<Stmt> Parser::parseBlock(std::string_view blockIdent) {
    LOG_DEBUG << std::format(
        "Parsing block ({}) starting at token index: {}", blockIdent, _current);
    const auto& lbraceToken = consume(Token::Type::LBrace, "Expected '{' opening block");

    std::vector<std::unique_ptr<Stmt>> statements{};
    while (not match(Token::Type::RBrace) and not parsedAll()) {
        statements.push_back(parseDefinition());
    }
    LOG_DEBUG << std::format("Parsed {} statement(s) in block", statements.size());

    const auto& rbraceToken = consume(Token::Type::RBrace, "Expected '}' closing block");

    LOG_DEBUG << "Block parsed with " << statements.size() << " statement(s)";
    return std::make_unique<BlockStmt>(std::move(statements), makeRange(lbraceToken, rbraceToken));
}

std::unique_ptr<Stmt> Parser::parseReturn() {
    const auto& returnToken = consume(Token::Type::Return, "Expected return token");
    LOG_DEBUG << "Parsing return statement at token index: " << _current;
    if (match(Token::Type::Semi)) {
        const auto& semiToken = getTokenAndAdvance();
        LOG_DEBUG << "Return statement with no value (implicit null)";
        return std::make_unique<ReturnStmt>(
            std::make_unique<LiteralExpr>(
                Token{Token::Type::Null, semiToken.getFileId(), semiToken.getLine(), semiToken.getColumn()},
                makeRange(semiToken, semiToken)),
            makeRange(returnToken, semiToken));
    }

    LOG_DEBUG << "Parsing return value expression";
    auto value = parseExpression();
    if (not value) {
        throwParserError("Expected an expression after 'yeet'");
    }

    const auto& semiToken = consume(Token::Type::Semi, "Expected '...' after return value");

    LOG_DEBUG << "Return statement parsed successfully";
    return std::make_unique<ReturnStmt>(std::move(value), makeRange(returnToken, semiToken));
}

std::unique_ptr<Stmt> Parser::parseIf() {
    LOG_DEBUG << "Parsing 'perhaps' statement";
    const auto& ifToken = getPreviousToken();
    consume(Token::Type::LParen, "Expected '(' after 'perhaps'");

    auto ifCondition = parseExpression();
    consume(Token::Type::RParen, "Expected ')' after 'perhaps' condition");

    auto ifBody = parseBlock("perhaps");

    std::vector<ElseIfClause> elIfClauses{};
    while (not parsedAll() and matchAndAdvanceIfNeeded(Token::Type::Elif)) {
        consume(Token::Type::LParen, "Expected '(' after 'or_whatever'");
        auto elifCondition = parseExpression();
        consume(Token::Type::RParen, "Expected ')' after 'or_whatever' condition");
        auto elifBody = parseBlock("or_whatever");

        elIfClauses.emplace_back(std::move(elifCondition), std::move(elifBody));
    }
    LOG_DEBUG << std::format("Parsed {} elif clause(s)", elIfClauses.size());

    std::unique_ptr<Stmt> elseClause{nullptr};
    if (not parsedAll() and matchAndAdvanceIfNeeded(Token::Type::Else)) {
        LOG_DEBUG << "Detected else clause";
        elseClause = parseBlock("screw_it");
    }

    const auto& lastToken = getPreviousToken();

    LOG_DEBUG << "If statement parsed successfully";
    return std::make_unique<IfStmt>(
        makeRange(ifToken, lastToken),
        std::move(ifCondition),
        std::move(ifBody),
        std::move(elIfClauses),
        std::move(elseClause)
    );
}

std::unique_ptr<Stmt> Parser::parseLoop() {
    LOG_DEBUG << "Parsing 'do_until_bored' statement";
    const auto& loopToken = getPreviousToken();
    auto body = parseBlock("do_until_bored");
    auto srcRange = makeRange(loopToken, *body);

    LOG_DEBUG << "Successfully parsed 'do_until_bored' statement";
    return std::make_unique<LoopStmt>(std::move(body), srcRange);
}

std::unique_ptr<Stmt> Parser::parseRepeat() {
    LOG_DEBUG << "Parsing 'repeat' statement";
    const auto& loopToken = getPreviousToken();
    consume(Token::Type::LParen, "Expected '(' after 'repeat'");
    auto countExpr = parseExpression();
    if (not countExpr) {
        throwParserError("Expected loop count expression inside '()'");
    }
    consume(Token::Type::RParen, "Expected ')' after loop count expression");

    auto body = parseBlock("spin_around");

    auto srcRange = makeRange(loopToken, *body);

    LOG_DEBUG << "Successfully parsed 'repeat' statement";
    return std::make_unique<RepeatStmt>(std::move(countExpr), std::move(body), srcRange);
}

std::unique_ptr<Stmt> Parser::parseImport() {
    LOG_DEBUG << "Parsing 'summon' statement";
    const auto& importToken = getPreviousToken();
    const auto& moduleToken = consume(Token::Type::Ident, "Expected module name after 'summon'");
    const auto& semiToken = consume(Token::Type::Semi, "Expected '...' after module import");
    LOG_DEBUG << "Successfully parsed 'summon' statement for module '{}'"
              << moduleToken.getValue<std::string>();
    return std::make_unique<ImportStmt>(moduleToken, makeRange(importToken, semiToken));
}

std::unique_ptr<Stmt> Parser::parseReassign() {
    LOG_DEBUG << "parseReassign() called at token index: " << _current;
    const auto& nameToken = getTokenAndAdvance();
    advance();
            
    auto value = parseExpression();
    const auto& semiToken = consume(Token::Type::Semi, "Expected '...' after reassignment");
    return std::make_unique<ReassignStmt>(
        nameToken, std::move(value), makeRange(nameToken, semiToken));
}

std::unique_ptr<Expr> Parser::parseExpression() {
    LOG_DEBUG << "parseExpression() called at token index: " << _current;
    if (auto expr = parseOr()) {
        LOG_DEBUG << "Expression parsed successfully";
        return expr;
    }
    LOG_WARN << "Expected an expression at token index: " << _current << " - returning null";
    return nullptr;
}

std::unique_ptr<Expr> Parser::parseOr() {
    LOG_DEBUG << "parseOr() called at token index: " << _current;
    auto left = parseAnd();

    while (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType != Token::Type::Or) {
            break;
        }
        advance();
        auto right = parseAnd();
        if (not right) {
            throwParserError(
                std::format("Expected right operand after '{}'", toSourceString(tokenType)));
        }
        auto srcRange = makeRange(*left, *right);
        LOG_DEBUG << "Parsed logical or expression successfully";
        left = std::make_unique<LogicalExpr>(
            std::move(left),
            token,
            std::move(right),
            srcRange
        );
    }

    LOG_DEBUG << "No more or operators, returning expression";
    return left;
}

std::unique_ptr<Expr> Parser::parseAnd() {
    LOG_DEBUG << "parseAnd() called at token index: " << _current;
    auto left = parseEquality();

    while (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType != Token::Type::And) {
            break;
        }
        advance();
        auto right = parseEquality();
        if (not right) {
            throwParserError(
                std::format("Expected right operand after '{}'", toSourceString(tokenType)));
        }
        auto srcRange = makeRange(*left, *right);
        LOG_DEBUG << "Parsed logical and expression successfully";
        left = std::make_unique<LogicalExpr>(
            std::move(left),
            token,
            std::move(right),
            srcRange);
    }

    LOG_DEBUG << "No more and operators, returning expression";
    return left;
}

std::unique_ptr<Expr> Parser::parsePrimary() {
    LOG_DEBUG << "parsePrimary() called at token index: " << _current;
    if (const auto& token = getToken(); token.isLiteral()) {
        LOG_DEBUG << "Parsed literal expression: " << toString(token.getType());
        return std::make_unique<LiteralExpr>(getTokenAndAdvance(), makeRange(token, token));
    }

    if (match(Token::Type::Ident)) {
        const auto& nameToken = getTokenAndAdvance();
        if (not nameToken.valueIs<std::string>()) {
            throwParserError("This is the moment to start yapping");
        }

        std::unique_ptr<Expr> expr = std::make_unique<VariableExpr>(
            nameToken, makeRange(nameToken, nameToken));

        while (not parsedAll()) {
            if (matchAndAdvanceIfNeeded(Token::Type::Dot)) {
                const auto& propertyToken = consume(
                    Token::Type::Ident, "Expected property name after '.'");
                expr = std::make_unique<DotExpr>(
                    std::move(expr), propertyToken, makeRange(nameToken, propertyToken));
            }
            else if (matchAndAdvanceIfNeeded(Token::Type::LParen)) {
                expr = parseFunctionCall(std::move(expr)); 
            }
            else {
                break;
            }
        }

        LOG_DEBUG << "Parsed variable/call/dot expression";
        return expr;
    }

    if (matchAndAdvanceIfNeeded(Token::Type::LParen)) {
        LOG_DEBUG << "Parsing grouped expression";
        auto expr = parseExpression();
        consume(Token::Type::RParen, "Expected ) after expression");
        return expr;
    }

    if (match(Token::Type::LBracket)) {
        LOG_DEBUG << "Parsing vector";
        const auto& lbracketToken = getTokenAndAdvance();
        std::vector<std::unique_ptr<Expr>> elements{};
        if (not match(Token::Type::RBracket)) {
            do {
                elements.push_back(parseExpression());
            } while (matchAndAdvanceIfNeeded(Token::Type::Comma));
        }
        const auto& rbracketToken = consume(
            Token::Type::RBracket, "Expected ']' at the end of a vector");
        LOG_DEBUG << std::format("Parsed vector with {} elements", elements.size());
        return std::make_unique<VectorExpr>(
            std::move(elements),
            makeRange(lbracketToken, rbracketToken));
    }

    LOG_DEBUG << "No primary expression matched at current token";
    return nullptr;
}

std::unique_ptr<Expr> Parser::parseEquality() {
    LOG_DEBUG << "parseEquality() called at token index: " << _current;
    auto left = parseComparison();

    if (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType == Token::Type::Equal or tokenType == Token::Type::NotEqual) {
            advance();
            auto right = parseComparison();
            if (not right) {
                throwParserError(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            auto srcRange = makeRange(*left, *right);
            LOG_DEBUG << "Parsed equality binary expression successfully";
            return make_unique<BinaryExpr>(std::move(left), token, std::move(right), srcRange);
        }
    }

    LOG_DEBUG << "No equality operator matched, returning left operand";
    return left;
}

std::unique_ptr<Expr> Parser::parseComparison() {
    LOG_DEBUG << "parseComparison() called at token index: " << _current;
    auto left = parseTerm();

    if (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType == Token::Type::Greater or tokenType == Token::Type::Less) {
            advance();
            auto right = parseTerm();
            if (not right) {
                throwParserError(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            auto srcRange = makeRange(*left, *right);
            LOG_DEBUG << "Parsed comparison binary expression successfully";
            return std::make_unique<BinaryExpr>(std::move(left), token, std::move(right), srcRange);
        }
    }

    LOG_DEBUG << "No comparison operator matched, returning left operand";
    return left;
}

std::unique_ptr<Expr> Parser::parseTerm() {
    LOG_DEBUG << "parseTerm() called at token index: " << _current;
    auto left = parseFactor();

    while (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType == Token::Type::Plus or tokenType == Token::Type::Minus) {
            advance();
            auto right = parseFactor();
            if (not right) {
                throwParserError(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            auto srcRange = makeRange(*left, *right);
            left = std::make_unique<BinaryExpr>(std::move(left), token, std::move(right), srcRange);
        }
        else {
            break;
        }
    }

    LOG_DEBUG << "No term operator matched, returning left operand";
    return left;
}

std::unique_ptr<Expr> Parser::parseFactor() {
    LOG_DEBUG << "parseFactor() called at token index: " << _current;
    auto left = parseUnary();

    while (not parsedAll()) {
        const auto& token = getToken();
        const auto tokenType = token.getType();
        if (tokenType == Token::Type::Multiply or tokenType == Token::Type::Divide) {
            advance();
            auto right = parseUnary();
            if (not right) {
                throwParserError(
                    std::format("Expected right operand after '{}'", toSourceString(tokenType)));
            }
            auto srcRange = makeRange(*left, *right);
            left = std::make_unique<BinaryExpr>(std::move(left), token, std::move(right), srcRange);
        }
        else {
            break;
        }
    }

    LOG_DEBUG << "No factor operator matched, returning left operand";
    return left;
}

std::unique_ptr<Expr> Parser::parseFunctionCall(std::unique_ptr<Expr> callee) {
    LOG_DEBUG << "Parsing function call arguments...";
    const auto& lParenToken = getPreviousToken();
    std::vector<std::unique_ptr<Expr>> arguments;
                
    if (not match(Token::Type::RParen)) {
        do {
            arguments.push_back(parseExpression());
        } while (matchAndAdvanceIfNeeded(Token::Type::Comma));
    }
    const auto& rParenToken = consume(Token::Type::RParen, "Expected ')' after arguments");
                
    LOG_DEBUG << std::format("Parsed function call with {} arguments", arguments.size());
    return std::make_unique<CallExpr>(
        std::move(callee), std::move(arguments), makeRange(lParenToken, rParenToken));
}

std::unique_ptr<Expr> Parser::parseUnary() {
    if (match(Token::Type::Incr)) {
        const auto& opToken = getTokenAndAdvance();
        auto right = parseUnary();
        auto srcRange = makeRange(opToken, *right);
        return std::make_unique<UnaryExpr>(opToken, std::move(right), srcRange);
    }

    return parsePrimary();
}
