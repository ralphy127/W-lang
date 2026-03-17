#include "RuntimeValue.hpp"

namespace {

std::string stringifyVector(const Vector& vector) {
    if (not vector) {
        return std::string{"[]"};
    }

    std::string result{"["};
    const auto vectorSize = vector->size();
    for (std::size_t index{0}; index < vectorSize; ++index) {
        if (index > 0) {
            result += ", ";
        }
        result += stringify((*vector)[index]);
    }
    result += "]";

    return result;
}

}

std::string stringify(const RuntimeValue& value) {
    return std::visit(overloaded{
        [](Null) { return std::string{"ghosted"}; },
        [](const String& str) { return str; },
        [](Int i) { return std::to_string(i); },
        [](Bool boolean) { return boolean ? std::string{"totally"} : std::string{"nah"}; },
        [](Float d) { return std::to_string(d); },
        [](const Vector& vector) { return stringifyVector(vector); },
        [](NativeFunction) { return std::string{"function"}; },
        [](Module) { return std::string{"module"}; }
    }, value);
}