#pragma once

#include <variant>
#include <string>
#include <functional>
#include <vector>
#include <memory>
#include <unordered_map>
#include <format>
#include "native_types/Vector.hpp"
#include "native_types/String.hpp"
#include "RuntimeErrors.hpp"

struct RuntimeValue;

using Null = std::monostate;
using Int = std::int32_t;
using Bool = bool;
using Float = double;
using Function = std::function<RuntimeValue(const std::vector<RuntimeValue>&)>;
using Module = std::shared_ptr<std::unordered_map<std::string, RuntimeValue>>;

using RuntimeValueBase = std::variant<
    Null,
    String,
    Int,
    Bool,
    Float,
    Vector,
    Function,
    Module>;
struct RuntimeValue : RuntimeValueBase {
    using RuntimeValueBase::variant;
};

namespace {

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

template <typename T>
constexpr std::string_view typeName() {
    using DecayedT = std::decay_t<T>;

    if constexpr (std::is_same_v<DecayedT, Null>) return "ghosted";
    else if constexpr (std::is_same_v<DecayedT, String>) return "yap";
    else if constexpr (std::is_same_v<DecayedT, Int>) return "solid";
    else if constexpr (std::is_same_v<DecayedT, Bool>) return "vibe";
    else if constexpr (std::is_same_v<DecayedT, Float>) return "change";
    else if constexpr (std::is_same_v<DecayedT, Vector>) return "lineup";
    else if constexpr (std::is_same_v<DecayedT, Function>) return "gig";
    else if constexpr (std::is_same_v<DecayedT, Module>) return "hub";
    else return "unknown";
}

inline std::string typeName(const RuntimeValueBase& value) {
    return std::visit([](const auto& v) {
        return std::string{typeName<decltype(v)>()};
    }, value);
}

}

inline bool operator==(const RuntimeValue& lhs, const RuntimeValue& rhs) {
    // TODO error handling
    if (lhs.index() != rhs.index()) {
        throw std::runtime_error{"Cannot compare values of different types"};
    }
    return std::visit([](const auto& l, const auto& r) -> bool {
        using T = std::decay_t<decltype(l)>;
        using U = std::decay_t<decltype(r)>;
        
        if constexpr (std::is_same_v<T, U>) {
            if constexpr (std::is_same_v<T, Function>) {
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
            if constexpr (std::is_same_v<T, Function>) {
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

std::string stringify(const RuntimeValue& value);

template<typename T>
bool is(const RuntimeValueBase& v) {
    return std::holds_alternative<T>(v);
}

template<typename T>
const T& as(const RuntimeValueBase& v, const SourceRange& srcRange) {
    if (not is<T>(v)) {
        throw RuntimeError{
            RuntimeError::Type::TypeMismatch,
            srcRange,
            std::format("Anticipated {} instead of {}", std::string{typeName<T>()}, typeName(v))};
    }
    return std::get<T>(v);
}

template<typename T>
const T& asUnsafe(const RuntimeValue& v) {
    if (not is<T>(v)) {
        throw std::runtime_error{std::format(
            "Anticipated {} instead of {}", std::string{typeName<T>()}, typeName(v))};
    }
    return std::get<T>(v);
}
