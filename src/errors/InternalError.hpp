#pragma once

#include <string>
#include <source_location>
#include <memory>

struct InternalError {
    std::string msg;
    std::source_location loc = std::source_location::current();
};

inline void ensureUnsafe(
    bool condition,
    const std::string& errorMsg,
    const std::source_location& loc = std::source_location::current()) {
    
    if (not condition) {
        throw InternalError{errorMsg, loc};
    }
}

template <typename T>
std::unique_ptr<T> unwrapUnsafe(
    std::unique_ptr<T> ptr, 
    const std::string& errorMsg = "Expected non-null",
    const std::source_location& loc = std::source_location::current()) {

    ensureUnsafe(ptr != nullptr, errorMsg, loc);
    return ptr; 
}
