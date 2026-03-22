#pragma once
#include <string>
#include "core/SourceLocation.hpp"

struct RuntimeError {
    enum class Type {
        Undefined,
        Value,
        Logic,
        Math,
        OutOfBounds,
    };

    Type type;
    SourceRange srcRange;
    std::string msg;
};
