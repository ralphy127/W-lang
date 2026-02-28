#include "Lexer.hpp"
#include <cctype>
#include <utils/Logging.hpp>

Lexer::Lexer(std::string source)
    : _source{std::move(source)}
{}

LexerResult Lexer::tokenize() {
    std::vector<Token> tokens{};
    std::vector<LexerError> errors{};

    while (not tokenizedAll()) {
        auto result = getTokenAndAdvance();

        if (result.has_value()) {
            Token token = std::move(result.value());
            
            if (token.getType() == Token::Type::Eof) {
                tokens.push_back(std::move(token));
                break;
            }
            
            tokens.push_back(std::move(token));
        }
        else {
            errors.push_back(result.error());
        }
    }

    return {std::move(tokens), std::move(errors)};
}

bool Lexer::tryTokenizeSingleChar(Token& token, char ch) {
    switch (ch) {
        case '\0':
            token.setType(Token::Type::Eof);
            LOG_DEBUG << "Tokenized EOF to Token::Type::Eof";
            return true;
        case '(':
            advance(ch);
            token.setType(Token::Type::LParen);
            LOG_DEBUG << "Tokenized '(' to Token::Type::LParen";
            return true;
        case ')':
            advance(ch);
            token.setType(Token::Type::RParen);
            LOG_DEBUG << "Tokenized ')' to Token::Type::RParen";
            return true;
        case '{':
            advance(ch);
            token.setType(Token::Type::LBrace);
            LOG_DEBUG << "Tokenized '{' to Token::Type::LBrace";
            return true;
        case '}':
            advance(ch);
            token.setType(Token::Type::RBrace);
            LOG_DEBUG << "Tokenized '}' to Token::Type::RBrace";
            return true;
        case ':':
            advance(ch);
            token.setType(Token::Type::Colon);
            LOG_DEBUG << "Tokenized ':' to Token::Type::Colon";
            return true;
        case ',':
            advance(ch);
            token.setType(Token::Type::Comma);
            LOG_DEBUG << "Tokenized ',' to Token::Type::Comma";
            return true;
        default:
            return false;
    }
}

bool Lexer::tryTokenizeKeyword(Token& token) {
    if (matchAndAdvanceIfNeeded("...")) {
        token.setType(Token::Type::Semi);
        LOG_DEBUG << "Tokenized '...' to Token::Type::Semi";
        return true;
    }
    if (matchAndAdvanceIfNeeded("about") or matchAndAdvanceIfNeeded("might_be")) {
        token.setType(Token::Type::Assign);
        LOG_DEBUG << "Tokenized 'about/might_be' to Token::Type::Assign";
        return true;
    }
    if (matchAndAdvanceIfNeeded("ghosted")) {
        token.setType(Token::Type::Null);
        LOG_DEBUG << "Tokenized 'ghosted' to Token::Type::Null";
        return true;
    }

    if (matchAndAdvanceIfNeeded("gig")) {
        token.setType(Token::Type::Func);
        LOG_DEBUG << "Tokenized 'gig' to Token::Type::Func";
        return true;
    }
    if (matchAndAdvanceIfNeeded("stash")) {
        token.setType(Token::Type::Var);
        LOG_DEBUG << "Tokenized 'stash' to Token::Type::Var";
        return true;
    }
    if (matchAndAdvanceIfNeeded("scream")) {
        token.setType(Token::Type::Print);
        LOG_DEBUG << "Tokenized 'scream' to Token::Type::Print";
        return true;
    }
    if (matchAndAdvanceIfNeeded("yeet")) {
        token.setType(Token::Type::Return);
        LOG_DEBUG << "Tokenized 'yeet' to Token::Type::Return";
        return true;
    }
    if (matchAndAdvanceIfNeeded("perhaps")) {
        token.setType(Token::Type::If);
        LOG_DEBUG << "Tokenized 'perhaps' to Token::Type::If";
        return true;
    }
    if (matchAndAdvanceIfNeeded("or_whatever")) {
        token.setType(Token::Type::Elif);
        LOG_DEBUG << "Tokenized 'or_whatever' to Token::Type::Elif";
        return true;
    }
    if (matchAndAdvanceIfNeeded("screw_it")) {
        token.setType(Token::Type::Else);
        LOG_DEBUG << "Tokenized 'screw_it' to Token::Type::Else";
        return true;
    }
    if (matchAndAdvanceIfNeeded("rage_quit")) {
        token.setType(Token::Type::Break);
        LOG_DEBUG << "Tokenized 'rage_quit' to Token::Type::Break";
        return true;
    }
    if (matchAndAdvanceIfNeeded("!!!")) {
        token.setType(Token::Type::BrSemi);
        LOG_DEBUG << "Tokenized '!!!' to Token::Type::BrSemi";
        return true;
    }

    if (matchAndAdvanceIfNeeded("bigger_ish")) {
        token.setType(Token::Type::Greater);
        LOG_DEBUG << "Tokenized 'bigger_ish' to Token::Type::Greater";
        return true;
    }
    if (matchAndAdvanceIfNeeded("tiny_ish")) {
        token.setType(Token::Type::Less);
        LOG_DEBUG << "Tokenized 'tiny_ish' to Token::Type::Less";
        return true;
    }
    if (matchAndAdvanceIfNeeded("looks_like")) {
        token.setType(Token::Type::Equal);
        LOG_DEBUG << "Tokenized 'looks_like' to Token::Type::Equal";
        return true;
    }
    if (matchAndAdvanceIfNeeded("kinda_sus")) {
        token.setType(Token::Type::NotEqual);
        LOG_DEBUG << "Tokenized 'kinda_sus' to Token::Type::NotEqual";
        return true;
    }

    if (matchAndAdvanceIfNeeded("totally")) {
        token.setType(Token::Type::True);
        LOG_DEBUG << "Tokenized 'totally' to Token::Type::True";
        return true;
    }
    if (matchAndAdvanceIfNeeded("nah")) {
        token.setType(Token::Type::False);
        LOG_DEBUG << "Tokenized 'nah' to Token::Type::False";
        return true;
    }

    if (matchAndAdvanceIfNeeded("pump_it")) {
        token.setType(Token::Type::Incr);
        LOG_DEBUG << "Tokenized 'pump_it' to Token::Type::Incr";
        return true;
    }
    if (matchAndAdvanceIfNeeded("without")) {
        token.setType(Token::Type::Minus);
        LOG_DEBUG << "Tokenized 'without' to Token::Type::Minus";
        return true;
    }
    if (matchAndAdvanceIfNeeded("with")) {
        token.setType(Token::Type::Plus);
        LOG_DEBUG << "Tokenized 'with' to Token::Type::Plus";
        return true;
    }

    if (matchAndAdvanceIfNeeded("do_until_bored")) {
        token.setType(Token::Type::While);
        LOG_DEBUG << "Tokenized 'do_until_bored' to Token::Type::While";
        return true;
    }
    if (matchAndAdvanceIfNeeded("spin_around")) {
        token.setType(Token::Type::Repeat);
        LOG_DEBUG << "Tokenized 'spin_around' to Token::Type::Repeat";
        return true;
    }
    return false;
}

bool Lexer::tryTokenizeNumber(Token& token, char ch) {
    // TODO add negative numbers
    if (not std::isdigit(ch) and ch != '-') {
        return false;
    }

    std::string buffer;
    bool isFloat{false};
    
    char digit = getChar();
    do {
        buffer += digit;
        advance(digit);
        digit = getChar();
    } while (std::isdigit(digit));

    if (digit == '.' and not tokenizedAll() and std::isdigit(_source[_pos+1])) {
        advance(digit);
        isFloat = true;
        buffer += digit;
        digit = getChar();
        while (std::isdigit(digit)) {
            advance(digit);
            buffer += digit;
            digit = getChar();
        }
    }

    if (isFloat) {
        token.setType(Token::Type::Float);
        token.setValue<double>(std::stod(buffer));
        LOG_DEBUG << "Tokenized '" << buffer << "' to Token::Type::Float";
    }
    else {
        token.setType(Token::Type::Int);
        token.setValue(std::stoi(buffer));
        LOG_DEBUG << "Tokenized '" << buffer << "' to Token::Type::Int";
    }
    
    return true;
}

std::expected<void, LexerError> Lexer::tryTokenizeString(Token& token) {
    char ch = getChar();
    if (ch != '\"') {
        return {};
    }

    advance(ch);
    std::string stringValue{};
    bool foundStringEnd{false};

    while (not tokenizedAll()) {
        ch = getChar();

        if (ch == '\"') {
            advance(ch);
            foundStringEnd = true;
            break;
        }
        if (ch == '\n') {
            LOG_ERROR << "string not terminated before end of line";
            return std::unexpected{createError(LexerErrorType::UnterminatedString)};
        }
        stringValue.push_back(ch);
        advance(ch);
    }
    
    if (not foundStringEnd) {
        LOG_ERROR << "string not terminated";
        return std::unexpected{createError(LexerErrorType::UnterminatedString)};
    }
    
    LOG_DEBUG << "Tokenized string: " << stringValue;
    token.setType(Token::Type::String);
    token.setValue<std::string>(std::move(stringValue));
    
    return {};
}

void Lexer::tokenizeIdentifier(Token& token) {
    std::string tokenVal{};
    char c = getChar();
    
    while(not tokenizedAll()) {
        if (not std::isalnum(c) and c != '_') {
            break;
        }
        tokenVal.push_back(c);
        advance(c);
        c = getChar();
    }

    LOG_DEBUG << "Tokenized ident: " << tokenVal;
    token.setType(Token::Type::Ident);
    token.setValue<std::string>(std::move(tokenVal));
}

std::expected<Token, LexerError> Lexer::getTokenAndAdvance() {
    skipWhitespaces();
    const auto skipResult = skipComments();
    if (not skipResult.has_value()) {
        return std::unexpected{skipResult.error()};
    }

    Token token{Token::Type::Unknown, _line, _col};
    auto ch = getChar();

    if (tryTokenizeSingleChar(token, ch)) {
        return token;
    }
    if (tryTokenizeKeyword(token)) {
        return token;
    }
    if (tryTokenizeNumber(token, ch)) {
        return token;
    }

    auto stringResult = tryTokenizeString(token);
    if (not stringResult.has_value()) {
        return std::unexpected{stringResult.error()};
    }
    if (token.getType() != Token::Type::Unknown) {
        return token;
    }

    if (isalpha(ch) || ch == '_') {
        tokenizeIdentifier(token);
        return token;
    }

    LOG_DEBUG << "Returning token at line " << token.getLine() << ", column "
              << token.getColumn() << ": " << std::to_underlying(token.getType());

    if (token.getType() == Token::Type::Unknown) {
        return std::unexpected{createError(LexerErrorType::UnknownToken)};
    }

    return token;
}

char Lexer::getChar() const {
    return _source[_pos];
}

// TODO make sure there is no possibility of overflowing and eof is safe
void Lexer::advance(char ch) {
    ++_pos;
    if (ch == '\n') {
        _line++;
        _col = 1u;
    } else {
        _col++;
    }
}

char Lexer::getCharAndAdvance() {
    auto ch = getChar();
    advance(ch);
    return ch;
}

void Lexer::skipWhitespaces() {
    for (;;) {
        char ch = getChar();
        if (not std::isspace(static_cast<unsigned char>(ch))) {
            break;
        }
        advance(ch);
    }
}

std::expected<void, LexerError> Lexer::skipComments() {
    for (;;) {
        if (matchAndAdvanceIfNeeded("psst:")) {
            while (not tokenizedAll()) {
                if (getCharAndAdvance() == '\n') {
                    break;
                }
            }
            LOG_DEBUG << "skipped one line comment";
            continue;
        }
        if (matchAndAdvanceIfNeeded("rant_stop")) {
            bool found = false;

            while (not tokenizedAll()) {
                if (_col == 1 and matchAndAdvanceIfNeeded("rant_start")) {
                    found = true;
                    break;
                }

                if (getCharAndAdvance() == '\0') {
                    break;
                }
            }

            if (not found) {
                LOG_ERROR << "unterminated block comment";
                return std::unexpected{createError(LexerErrorType::UnterminatedBlockComment)};
            }
            LOG_DEBUG << "skipped block comment";
            skipWhitespaces();
            continue;
        }

        break;
    }

    return {};
}


bool Lexer::match(char expected) {
    return _source[_pos] == expected;
}

bool Lexer::matchAndAdvanceIfNeeded(std::string_view expected) {
    auto startPos = _pos;
    auto startLine = _line;
    auto startCol = _col;
    
    for (auto ch : expected) {
        if (not match(ch)) {
            _pos = startPos;
            _line = startLine;
            _col = startCol;
            return false;
        }
        advance(ch);
    }

    return true;
}
