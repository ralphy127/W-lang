#include "RuntimeValue.hpp"

namespace {
    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
}

std::string stringify(const RuntimeValue& value) {
    return std::visit(overloaded{
        [](std::monostate) { return std::string("ghosted"); },
        [](const std::string& str) { return str; },
        [](std::int32_t i) { return std::to_string(i); }
    }, value);
}