#pragma once

#include "runtime/RuntimeValue.hpp"

struct EvalProxy {
    RuntimeValue value;
    SourceRange range;

    operator RuntimeValue() && { 
        return std::move(value); 
    }

    operator RuntimeValue() const & = delete;

    template <typename T>
    const T& as() const {
        return ::as<T>(value, range); 
    }

    template <typename T>
    bool is() const {
        return ::is<T>(value); 
    }
};
