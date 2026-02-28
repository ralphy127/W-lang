#include "Token.hpp"

Token::Token(
    Type type,
    std::uint32_t line,
    std::uint32_t column,
    Value val)
    : _value{val}
    , _type{type}
    , _line{line}
    , _column{column} {}

bool Token::isLiteral() const {
    return _type == Type::Null or
           _type == Type::Int or
           _type == Type::Float or
           _type == Type::String or
           _type == Type::True or
           _type == Type::False;
}

bool Token::isOperator() const {
    return _type == Type::Plus or
           _type == Type::Minus or
           _type == Type::Equal or
           _type == Type::NotEqual or
           _type == Type::Incr or
           _type == Type::Greater or
           _type == Type::Less;
}

bool Token::isStatementStart() const {
    return _type == Type::Var or
           _type == Type::Print or
           _type == Type::If or
           _type == Type::Elif or
           _type == Type::Else or
           _type == Type::Loop or
           _type == Type::Repeat or
           _type == Type::Return ;
}

std::string toString(Token::Type type) {
    switch (type) {
        case Token::Type::Eof: return "Eof";
        case Token::Type::Func: return "Func";
        case Token::Type::Var: return "Var";
        case Token::Type::Print: return "Print";
        case Token::Type::Return: return "Return";
        case Token::Type::If: return "If";
        case Token::Type::Elif: return "Elif";
        case Token::Type::Else: return "Else";
        case Token::Type::Break: return "Break";
        case Token::Type::Loop: return "While";
        case Token::Type::Repeat: return "Repeat";
        case Token::Type::Greater: return "Greater";
        case Token::Type::Less: return "Less";
        case Token::Type::Equal: return "Equal";
        case Token::Type::NotEqual: return "NotEqual";
        case Token::Type::Ident: return "Ident";
        case Token::Type::Null: return "Null";
        case Token::Type::True: return "True";
        case Token::Type::False: return "False";
        case Token::Type::Int: return "Int";
        case Token::Type::Float: return "Float";
        case Token::Type::String: return "String";
        case Token::Type::LParen: return "LParen";
        case Token::Type::RParen: return "RParen";
        case Token::Type::LBrace: return "LBrace";
        case Token::Type::RBrace: return "RBrace";
        case Token::Type::Semi: return "Semi";
        case Token::Type::BrSemi: return "BrSemi";
        case Token::Type::Colon: return "Colon";
        case Token::Type::Comma: return "Comma";
        case Token::Type::Assign: return "Assign";
        case Token::Type::Reassign: return "Reassign";
        case Token::Type::Incr: return "Incr";
        case Token::Type::Plus: return "Plus";
        case Token::Type::Minus: return "Minus";
        default: throw std::logic_error{"Not implemented token type in toString"};
    }
}

std::string toSourceString(Token::Type type) {
    switch (type) {
        case Token::Type::Func: return "gig";
        case Token::Type::Var: return "stash";
        case Token::Type::Print: return "scream";
        case Token::Type::Return: return "yeet";
        case Token::Type::If: return "perhaps";
        case Token::Type::Elif: return "or_whatever";
        case Token::Type::Else: return "screw_it";
        case Token::Type::Break: return "rage_quit";
        case Token::Type::Loop: return "do_until_bored";
        case Token::Type::Repeat: return "spin_around";
        case Token::Type::Greater: return "bigger_ish";
        case Token::Type::Less: return "tiny_ish";
        case Token::Type::Equal: return "looks_like";
        case Token::Type::NotEqual: return "kinda_sus";
        case Token::Type::Null: return "ghosted";
        case Token::Type::True: return "totally";
        case Token::Type::False: return "nah";
        case Token::Type::LParen: return "(";
        case Token::Type::RParen: return ")";
        case Token::Type::LBrace: return "{";
        case Token::Type::RBrace: return "}";
        case Token::Type::Semi: return "...";
        case Token::Type::BrSemi: return "!!!";
        case Token::Type::Colon: return ":";
        case Token::Type::Comma: return ",";
        case Token::Type::Assign: return "about";
        case Token::Type::Reassign: return "might_be";
        case Token::Type::Incr: return "pump_it;";
        case Token::Type::Plus: return "with";
        case Token::Type::Minus: return "without";
        default: throw std::logic_error{"Not implemented token type in toSourceString"};
    }
}
