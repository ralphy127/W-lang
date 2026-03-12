#pragma once

#include <variant>
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>

struct RuntimeValue;

using Null = std::monostate;
using String = std::string;
using Int = std::int32_t;
using Bool = bool;
using Float = double;
// TODO ? change functions in interpretor to global Function type
using NativeFunction = std::function<RuntimeValue(const std::vector<RuntimeValue>&)>;
using Module = std::shared_ptr<std::unordered_map<std::string, RuntimeValue>>;

using RuntimeValueBase = std::variant<Null, String, Int, Bool, Float, NativeFunction, Module>;
struct RuntimeValue : RuntimeValueBase {
    using RuntimeValueBase::variant;
};

inline bool operator==(const RuntimeValue& lhs, const RuntimeValue& rhs) {
    // TODO error handling
    if (lhs.index() != rhs.index()) {
        throw std::runtime_error{"Cannot compare values of different types"};
    }
    return std::visit([](const auto& l, const auto& r) -> bool {
        using T = std::decay_t<decltype(l)>;
        using U = std::decay_t<decltype(r)>;
        
        if constexpr (std::is_same_v<T, U>) {
            if constexpr (std::is_same_v<T, NativeFunction>) {
                throw std::runtime_error{"Cannot compare functions"};
            }
            else {
                return l == r;
            }
        }
        return false;
    }, static_cast<const RuntimeValueBase&>(lhs), static_cast<const RuntimeValueBase&>(rhs));
}

inline bool operator<(const RuntimeValue& lhs, const RuntimeValue& rhs) {
    // TODO error handling
    if (lhs.index() != rhs.index()) {
        throw std::runtime_error{"Cannot compare values of different types"};
    }

    return std::visit([](const auto& l, const auto& r) -> bool {
        using T = std::decay_t<decltype(l)>;
        using U = std::decay_t<decltype(r)>;
        
        if constexpr (std::is_same_v<T, U>) {
            if constexpr (std::is_same_v<T, NativeFunction>) {
                throw std::runtime_error{"Cannot compare functions"};
            }
            else {
                return l < r;
            }
        }
        return false;
    }, static_cast<const RuntimeValueBase&>(lhs), static_cast<const RuntimeValueBase&>(rhs));
}

inline bool operator!=(const RuntimeValue& lhs, const RuntimeValue& rhs) { return !(lhs == rhs); }
inline bool operator>(const RuntimeValue& lhs, const RuntimeValue& rhs)  { return rhs < lhs; }
inline bool operator<=(const RuntimeValue& lhs, const RuntimeValue& rhs) { return !(rhs < lhs); }
inline bool operator>=(const RuntimeValue& lhs, const RuntimeValue& rhs) { return !(lhs < rhs); }

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::string stringify(const RuntimeValue& value);
