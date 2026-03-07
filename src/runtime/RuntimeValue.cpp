#include "RuntimeValue.hpp"

std::string stringify(const RuntimeValue& value) {
    return std::visit(overloaded{
        [](std::monostate) { return std::string("ghosted"); },
        [](const std::string& str) { return str; },
        [](std::int32_t i) { return std::to_string(i); },
        [](bool boolean) { return boolean ? std::string{"totally"} : std::string{"nah"}; },
        [](double d) { return std::to_string(d); }
    }, value);
}