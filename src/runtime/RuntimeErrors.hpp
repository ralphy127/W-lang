#pragma once
#include <string>
#include "core/SourceRange.hpp"

struct RuntimeError {
    enum class Type {
        Undefined,
        Logic,
        Math,
        OutOfBounds,
        TypeMismatch,
    };

    Type type;
    SourceRange srcRange;
    std::string msg;
};

struct NativeError : std::runtime_error {
    RuntimeError::Type type;

    NativeError(RuntimeError::Type type, const std::string& msg)
        : std::runtime_error(msg), type(type) {}
};
