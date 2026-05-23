#include "RuntimeValue.hpp"
#include "RuntimeValueType.hpp"

namespace {

std::string stringifyVector(const Vector& vector) {
    if (not vector) {
        return std::string{"[]"};
    }

    std::string result{"["};
    const auto vectorSize = vector->data.size();
    for (std::size_t index{0}; index < vectorSize; ++index) {
        if (index > 0) {
            result += ", ";
        }
        result += stringify(vector->data[index]);
    }
    result += "]";

    return result;
}

std::string stringifyFloat(Float f) {
    auto str = std::to_string(f);
    str.erase(str.find_last_not_of('0') + 1ull, std::string::npos); 
    return str;
}

}

std::string stringify(const RuntimeValue& value) {
    return std::visit(overloaded{
        [](Null) { return std::string{"ghosted"}; },
        [](const String& str) { return str; },
        [](Int i) { return std::to_string(i); },
        [](Bool boolean) { return boolean ? std::string{"totally"} : std::string{"nah"}; },
        [](Float f) { return stringifyFloat(f); },
        [](const Vector& vector) { return stringifyVector(vector); },
        [](const Function&) { return std::string{"gig"}; },
        [](const StructInstance& s) { return s.typeName; },
        [](const Module&) { return std::string{"hub"}; }
    }, value);
}

RuntimeValueType getType(const RuntimeValue& value) {
    return std::visit([](const auto& x) -> RuntimeValueType {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, Null>) return RuntimeValueType::Null;
        else if constexpr (std::is_same_v<T, String>) return RuntimeValueType::String;
        else if constexpr (std::is_same_v<T, Int>) return RuntimeValueType::Int;
        else if constexpr (std::is_same_v<T, Bool>) return RuntimeValueType::Bool;
        else if constexpr (std::is_same_v<T, Float>) return RuntimeValueType::Float;
        else if constexpr (std::is_same_v<T, Vector>) return RuntimeValueType::Vector;
        else if constexpr (std::is_same_v<T, Function>) return RuntimeValueType::Function;
        else if constexpr (std::is_same_v<T, StructInstance>) return RuntimeValueType::StructInstance;
        else if constexpr (std::is_same_v<T, Module>) return RuntimeValueType::Module;
        else return RuntimeValueType::Unset;
    }, static_cast<const RuntimeValueBase&>(value));
}
