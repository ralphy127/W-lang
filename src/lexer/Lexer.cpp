#include "Lexer.hpp"
#include <cctype>
#include <iostream>

Lexer::Lexer(std::string source)
    // TODO make a logger ffs
    : _source{std::move(source)}
{}

Token Lexer::getNextTokenAndAdvance() {
    // TODO check for safety of lookaheads
    // TODO refactor this sh
    skipWhitespaces();
    skipComments();

    Token token{Token::Type::Unknown, _line, _col};

    auto ch = getNextChar();

    bool simpleToken{true};

    switch (ch) {
        case '\0':
            token.setType(Token::Type::Eof);
            std::cout << "Tokenized EOF to Token::Type::Eof" << std::endl;
            break;
        case '(':
            advance(ch);
            token.setType(Token::Type::LParen);
            std::cout << "Tokenized '(' to Token::Type::LParen" << std::endl;
            break;
        case ')':
            advance(ch);
            token.setType(Token::Type::RParen);
            std::cout << "Tokenized ')' to Token::Type::RParen" << std::endl;
            break;
        case '{':
            advance(ch);
            token.setType(Token::Type::LBrace);
            std::cout << "Tokenized '{' to Token::Type::LBrace" << std::endl;
            break;
        case '}':
            advance(ch);
            token.setType(Token::Type::RBrace);
            std::cout << "Tokenized '}' to Token::Type::RBrace" << std::endl;
            break;
        case ':':
            advance(ch);
            token.setType(Token::Type::Colon);
            std::cout << "Tokenized ':' to Token::Type::Colon" << std::endl;
            break;
        case ',':
            advance(ch);
            token.setType(Token::Type::Comma);
            std::cout << "Tokenized ',' to Token::Type::Comma" << std::endl;
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
        std::cout << "Tokenized '...' to Token::Type::Semi" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("about") or matchAndAdvanceIfNeeded("might_be")) {
        token.setType(Token::Type::Assign);
        std::cout << "Tokenized 'about/might_be' to Token::Type::Assign" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("ghosted")) {
        token.setType(Token::Type::Null);
        std::cout << "Tokenized 'ghosted' to Token::Type::Null" << std::endl;
    }

    else if (matchAndAdvanceIfNeeded("gig")) {
        token.setType(Token::Type::Func);
        std::cout << "Tokenized 'gig' to Token::Type::Func" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("stash")) {
        token.setType(Token::Type::Var);
        std::cout << "Tokenized 'stash' to Token::Type::Var" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("scream")) {
        token.setType(Token::Type::Print);
        std::cout << "Tokenized 'scream' to Token::Type::Print" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("yeet")) {
        token.setType(Token::Type::Return);
        std::cout << "Tokenized 'yeet' to Token::Type::Return" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("perhaps")) {
        token.setType(Token::Type::If);
        std::cout << "Tokenized 'perhaps' to Token::Type::If" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("or_whatever")) {
        token.setType(Token::Type::Elif);
        std::cout << "Tokenized 'or_whatever' to Token::Type::Elif" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("screw_it")) {
        token.setType(Token::Type::Else);
        std::cout << "Tokenized 'screw_it' to Token::Type::Else" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("rage_quit")) {
        token.setType(Token::Type::Break);
        std::cout << "Tokenized 'rage_quit' to Token::Type::Break" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("!!!")) {
        token.setType(Token::Type::BrSemi);
        std::cout << "Tokenized '!!!' to Token::Type::BrSemi" << std::endl;
    }

    else if (matchAndAdvanceIfNeeded("bigger_ish")) {
        token.setType(Token::Type::Greater);
        std::cout << "Tokenized 'bigger_ish' to Token::Type::Greater" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("tiny_ish")) {
        token.setType(Token::Type::Less);
        std::cout << "Tokenized 'tiny_ish' to Token::Type::Less" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("looks_like")) {
        token.setType(Token::Type::Equal);
        std::cout << "Tokenized 'looks_like' to Token::Type::Equal" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("kinda_sus")) {
        token.setType(Token::Type::NotEqual);
        std::cout << "Tokenized 'kinda_sus' to Token::Type::NotEqual" << std::endl;
    }

    else if (matchAndAdvanceIfNeeded("totally")) {
        token.setType(Token::Type::True);
        std::cout << "Tokenized 'totally' to Token::Type::True" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("nah")) {
        token.setType(Token::Type::False);
        std::cout << "Tokenized 'nah' to Token::Type::False" << std::endl;
    }

    else if (matchAndAdvanceIfNeeded("pump_it")) {
        token.setType(Token::Type::Incr);
        std::cout << "Tokenized 'pump_it' to Token::Type::Incr" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("without")) {
        token.setType(Token::Type::Minus);
        std::cout << "Tokenized 'without' to Token::Type::Minus" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("with")) {
        token.setType(Token::Type::Plus);
        std::cout << "Tokenized 'with' to Token::Type::Plus" << std::endl;
    }

    else if (matchAndAdvanceIfNeeded("do_until_bored")) {
        token.setType(Token::Type::While);
        std::cout << "Tokenized 'do_until_bored' to Token::Type::While" << std::endl;
    }
    else if (matchAndAdvanceIfNeeded("spin_around")) {
        token.setType(Token::Type::Repeat);
        std::cout << "Tokenized 'spin_around' to Token::Type::Repeat" << std::endl;
    }

    // TODO check if minuses work properly
    else if (std::isdigit(ch) or ch == '-') {
        std::string buffer;
        bool isFloat{false};
        
        char digit = getNextChar();
        do {
            buffer += digit;
            advance(digit);
            digit = getNextChar();
        } while (std::isdigit(digit));

        if (digit == '.' and not tokenizedAll() and std::isdigit(_source[_pos+1])) {
            advance(digit);
            isFloat = true;
            buffer += digit;
            digit = getNextChar();
            while (std::isdigit(digit)) {
                advance(digit);
                buffer += digit;
                digit = getNextChar();
            }
        }

        if (isFloat) {
            token.setType(Token::Type::Float);
            token.setValue<double>(std::stod(buffer));
            std::cout << "Tokenized '" << buffer << "' to Token::Type::Float" << std::endl;
        }
        else {
            token.setType(Token::Type::Int);
            token.setValue(std::stoi(buffer));
            std::cout << "Tokenized '" << buffer << "' to Token::Type::Int" << std::endl;
        }
    }

    else if (ch == '\"') {
        advance(ch);
        std::string stringValue{};
        bool foundStringEnd{false};

        while (not tokenizedAll()) {
            ch = getNextChar();

            if (ch == '"') {
                advance(ch);
                foundStringEnd = true;
                break;
            }
            else if (ch == '\n') {
                std::cerr << "string not terminated 1" << std::endl;
                throw 1;
            }
            else {
                stringValue.push_back(ch);
                advance(ch);
            }
        }
        if (not foundStringEnd) {
            std::cerr << "string not terminated 2" << std::endl;
            throw 1;
        }
        std::cout << "Tokenized string: " << stringValue << '\n';
        token.setType(Token::Type::String);
        token.setValue<std::string>(std::move(stringValue));
    }

    else if (isalpha(ch) || ch == '_') {
        // TODO string has a bunch of /0 in the end, remove buffer or resize it in the end
        std::string tokenVal{};
        size_t bufferLen{128ul};
        tokenVal.resize(bufferLen);
        char c = getNextChar();
        for (size_t i{0ul}; i < bufferLen; ++i) {
            if (not std::isalnum(c) and c != '_') {
                break;
            }

            tokenVal[i] = c;
            advance(c);
            c = getNextChar();
        }

        std::cout << "Tokenized ident: " << tokenVal << std::endl;

        token.setType(Token::Type::Ident);
        token.setValue<std::string>(std::move(tokenVal));
    }

    std::cout << "Returning token at line " << token.getLine() << ", column "
              << token.getColumn() << ": " << std::to_underlying(token.getType()) << std::endl;

    if (token.getType() == Token::Type::Unknown) {
        std::cerr << "token type is unknown\n";
    }
    return token;
}

// TODO getNext is kinda current
char Lexer::getNextChar() const {
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

char Lexer::getNextCharAndAdvance() {
    auto ch = getNextChar();
    advance(ch);
    return ch;
}

void Lexer::skipWhitespaces() {
    for (;;) {
        char ch = getNextChar();
        if (not std::isspace(static_cast<unsigned char>(ch))) {
            break;
        }
        advance(ch);
    }
}

void Lexer::skipComments() {
    for (;;) {
        if (matchAndAdvanceIfNeeded("psst:")) {
            while (not tokenizedAll()) {
                if (getNextCharAndAdvance() == '\n') {
                    break;
                }
            }
            std::cout << "skipped one line comment" << std::endl;
            continue;
        }
        if (matchAndAdvanceIfNeeded("rant_stop")) {
            bool found = false;

            while (not tokenizedAll()) {
                if (_col == 1 and matchAndAdvanceIfNeeded("rant_start")) {
                    found = true;
                    break;
                }

                if (getNextCharAndAdvance() == '\0') {
                    break;
                }
            }

            if (not found) {
                std::cout << "unterminated block comment";
                throw 1;
            }
            std::cout << "skipped block comment" << std::endl;
            skipWhitespaces();
            continue;
        }

        break;
    }
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
