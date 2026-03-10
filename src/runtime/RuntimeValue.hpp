#pragma once

#include <variant>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

using Null = std::monostate;
using String = std::string;
using Int = std::int32_t;
using Bool = bool;
using Float = double;
// TODO ? change functions in interpretor to global Function type
using NativeFunction = std::function<RuntimeValue(std::vector<RuntimeValue>)>;
using Module = std::shared_ptr<std::unordered_map<std::string, NativeFunction>>;
using RuntimeValue = std::variant<Null, String, Int, Bool, Float, NativeFunction, Module>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string stringify(const RuntimeValue& value);
