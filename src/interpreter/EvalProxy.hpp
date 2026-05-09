#pragma once

#include "runtime/RuntimeValue.hpp"

class EvalProxy {
public:
    EvalProxy(RuntimeValue value, SourceRange range)
        : _value{std::move(value)}
        , _range(std::move(range)) {}
    
    operator RuntimeValue() && { 
        return std::move(_value); 
    }

    operator RuntimeValue() const & = delete;

    template <typename T>
    const T& as() const {
        return ::as<T>(_value, _range); 
    }

    template <typename T>
    bool is() const {
        return ::is<T>(_value); 
    }

private:
    RuntimeValue _value;
    SourceRange _range;
};
