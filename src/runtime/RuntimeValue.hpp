#pragma once

#include <variant>
#include <string>

using RuntimeValue = std::variant<std::monostate, std::string, std::int32_t, bool, double>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string stringify(const RuntimeValue& value);
