#include "RuntimeValue.hpp"

std::string stringify(const RuntimeValue& value) {
    return std::visit(overloaded{
        [](Null) { return std::string("ghosted"); },
        [](const String& str) { return str; },
        [](Int i) { return std::to_string(i); },
        [](Bool boolean) { return boolean ? std::string{"totally"} : std::string{"nah"}; },
        [](Float d) { return std::to_string(d); }
    }, value);
}