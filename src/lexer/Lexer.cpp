#include "Lexer.hpp"
#include <cctype>
#include <utils/Logging.hpp>

Lexer::Lexer(std::string source)
    : _source{std::move(source)}
{}

std::expected<Token, LexerError> Lexer::getTokenAndAdvance() {
    // TODO check for safety of lookaheads
    // TODO refactor this sh
    skipWhitespaces();
    const auto skipResult = skipComments();
    if (not skipResult.has_value()) {
        return std::unexpected{skipResult.error()};
    }

    Token token{Token::Type::Unknown, _line, _col};

    auto ch = getChar();

    bool simpleToken{true};

    switch (ch) {
        case '\0':
            token.setType(Token::Type::Eof);
            LOG_DEBUG << "Tokenized EOF to Token::Type::Eof";
            break;
        case '(':
            advance(ch);
            token.setType(Token::Type::LParen);
            LOG_DEBUG << "Tokenized '(' to Token::Type::LParen";
            break;
        case ')':
            advance(ch);
            token.setType(Token::Type::RParen);
            LOG_DEBUG << "Tokenized ')' to Token::Type::RParen";
            break;
        case '{':
            advance(ch);
            token.setType(Token::Type::LBrace);
            LOG_DEBUG << "Tokenized '{' to Token::Type::LBrace";
            break;
        case '}':
            advance(ch);
            token.setType(Token::Type::RBrace);
            LOG_DEBUG << "Tokenized '}' to Token::Type::RBrace";
            break;
        case ':':
            advance(ch);
            token.setType(Token::Type::Colon);
            LOG_DEBUG << "Tokenized ':' to Token::Type::Colon";
            break;
        case ',':
            advance(ch);
            token.setType(Token::Type::Comma);
            LOG_DEBUG << "Tokenized ',' to Token::Type::Comma";
        default:
            simpleToken = false;
            break;
    }
    if (simpleToken) {
        ; // TODO skip the rest? idk
    }
    // TODO check if variables such as e.g. gigSomething is split into two tokens
    else if (matchAndAdvanceIfNeeded("...")) {
        token.setType(Token::Type::Semi);
        LOG_DEBUG << "Tokenized '...' to Token::Type::Semi";
    }
    else if (matchAndAdvanceIfNeeded("about") or matchAndAdvanceIfNeeded("might_be")) {
        token.setType(Token::Type::Assign);
        LOG_DEBUG << "Tokenized 'about/might_be' to Token::Type::Assign";
    }
    else if (matchAndAdvanceIfNeeded("ghosted")) {
        token.setType(Token::Type::Null);
        LOG_DEBUG << "Tokenized 'ghosted' to Token::Type::Null";
    }

    else if (matchAndAdvanceIfNeeded("gig")) {
        token.setType(Token::Type::Func);
        LOG_DEBUG << "Tokenized 'gig' to Token::Type::Func";
    }
    else if (matchAndAdvanceIfNeeded("stash")) {
        token.setType(Token::Type::Var);
        LOG_DEBUG << "Tokenized 'stash' to Token::Type::Var";
    }
    else if (matchAndAdvanceIfNeeded("scream")) {
        token.setType(Token::Type::Print);
        LOG_DEBUG << "Tokenized 'scream' to Token::Type::Print";
    }
    else if (matchAndAdvanceIfNeeded("yeet")) {
        token.setType(Token::Type::Return);
        LOG_DEBUG << "Tokenized 'yeet' to Token::Type::Return";
    }
    else if (matchAndAdvanceIfNeeded("perhaps")) {
        token.setType(Token::Type::If);
        LOG_DEBUG << "Tokenized 'perhaps' to Token::Type::If";
    }
    else if (matchAndAdvanceIfNeeded("or_whatever")) {
        token.setType(Token::Type::Elif);
        LOG_DEBUG << "Tokenized 'or_whatever' to Token::Type::Elif";
    }
    else if (matchAndAdvanceIfNeeded("screw_it")) {
        token.setType(Token::Type::Else);
        LOG_DEBUG << "Tokenized 'screw_it' to Token::Type::Else";
    }
    else if (matchAndAdvanceIfNeeded("rage_quit")) {
        token.setType(Token::Type::Break);
        LOG_DEBUG << "Tokenized 'rage_quit' to Token::Type::Break";
    }
    else if (matchAndAdvanceIfNeeded("!!!")) {
        token.setType(Token::Type::BrSemi);
        LOG_DEBUG << "Tokenized '!!!' to Token::Type::BrSemi";
    }

    else if (matchAndAdvanceIfNeeded("bigger_ish")) {
        token.setType(Token::Type::Greater);
        LOG_DEBUG << "Tokenized 'bigger_ish' to Token::Type::Greater";
    }
    else if (matchAndAdvanceIfNeeded("tiny_ish")) {
        token.setType(Token::Type::Less);
        LOG_DEBUG << "Tokenized 'tiny_ish' to Token::Type::Less";
    }
    else if (matchAndAdvanceIfNeeded("looks_like")) {
        token.setType(Token::Type::Equal);
        LOG_DEBUG << "Tokenized 'looks_like' to Token::Type::Equal";
    }
    else if (matchAndAdvanceIfNeeded("kinda_sus")) {
        token.setType(Token::Type::NotEqual);
        LOG_DEBUG << "Tokenized 'kinda_sus' to Token::Type::NotEqual";
    }

    else if (matchAndAdvanceIfNeeded("totally")) {
        token.setType(Token::Type::True);
        LOG_DEBUG << "Tokenized 'totally' to Token::Type::True";
    }
    else if (matchAndAdvanceIfNeeded("nah")) {
        token.setType(Token::Type::False);
        LOG_DEBUG << "Tokenized 'nah' to Token::Type::False";
    }

    else if (matchAndAdvanceIfNeeded("pump_it")) {
        token.setType(Token::Type::Incr);
        LOG_DEBUG << "Tokenized 'pump_it' to Token::Type::Incr";
    }
    else if (matchAndAdvanceIfNeeded("without")) {
        token.setType(Token::Type::Minus);
        LOG_DEBUG << "Tokenized 'without' to Token::Type::Minus";
    }
    else if (matchAndAdvanceIfNeeded("with")) {
        token.setType(Token::Type::Plus);
        LOG_DEBUG << "Tokenized 'with' to Token::Type::Plus";
    }

    else if (matchAndAdvanceIfNeeded("do_until_bored")) {
        token.setType(Token::Type::While);
        LOG_DEBUG << "Tokenized 'do_until_bored' to Token::Type::While";
    }
    else if (matchAndAdvanceIfNeeded("spin_around")) {
        token.setType(Token::Type::Repeat);
        LOG_DEBUG << "Tokenized 'spin_around' to Token::Type::Repeat";
    }

    // TODO check if minuses work properly
    else if (std::isdigit(ch) or ch == '-') {
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
    }

    else if (ch == '\"') {
        advance(ch);
        std::string stringValue{};
        bool foundStringEnd{false};

        while (not tokenizedAll()) {
            ch = getChar();

            if (ch == '"') {
                advance(ch);
                foundStringEnd = true;
                break;
            }
            else if (ch == '\n') {
                LOG_ERROR << "string not terminated before end of line";
                return std::unexpected{
                    LexerError{_line, _col, LexerErrorType::UnterminatedString}};
            }
            else {
                stringValue.push_back(ch);
                advance(ch);
            }
        }
        if (not foundStringEnd) {
            LOG_ERROR << "string not terminated";
            return std::unexpected{
                LexerError{_line, _col, LexerErrorType::UnterminatedString}};
        }
        LOG_DEBUG << "Tokenized string: " << stringValue;
        token.setType(Token::Type::String);
        token.setValue<std::string>(std::move(stringValue));
    }

    else if (isalpha(ch) || ch == '_') {
        // TODO string has a bunch of /0 in the end, remove buffer or resize it in the end
        std::string tokenVal{};
        size_t bufferLen{128ul};
        tokenVal.resize(bufferLen);
        char c = getChar();
        for (size_t i{0ul}; i < bufferLen; ++i) {
            if (not std::isalnum(c) and c != '_') {
                break;
            }

            tokenVal[i] = c;
            advance(c);
            c = getChar();
        }

        LOG_DEBUG << "Tokenized ident: " << tokenVal;

        token.setType(Token::Type::Ident);
        token.setValue<std::string>(std::move(tokenVal));
    }

    LOG_DEBUG << "Returning token at line " << token.getLine() << ", column "
              << token.getColumn() << ": " << std::to_underlying(token.getType());

    if (token.getType() == Token::Type::Unknown) {
        LOG_ERROR << "token type is unknown";
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
                return std::unexpected{
                    LexerError{_line, _col, LexerErrorType::UnterminatedBlockComment}};
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
