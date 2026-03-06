#pragma once

#include <variant>
#include <string>

using RuntimeValue = std::variant<std::monostate, std::string, std::int32_t, bool>;

std::string stringify(const RuntimeValue& value);
