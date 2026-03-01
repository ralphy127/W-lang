#pragma once

#include <variant>
#include <string>

using RuntimeValue = std::variant<std::monostate, std::string>;