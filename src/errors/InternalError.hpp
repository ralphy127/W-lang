#pragma once

#include <string>
#include <source_location>
#include <memory>

struct InternalError {
    std::string msg;
    std::source_location loc = std::source_location::current();
};

inline void ensure(
    bool condition,
    const std::string& errorMsg,
    const std::source_location& loc = std::source_location::current()) {
    
    if (not condition) {
        throw InternalError{errorMsg, loc};
    }
}

template <typename T>
std::unique_ptr<T> unwrap(
    std::unique_ptr<T> ptr, 
    const std::string& errorMsg = "Expected non-null",
    const std::source_location& loc = std::source_location::current()) {

    ensure(ptr != nullptr, errorMsg, loc);
    return ptr; 
}

template <typename T>
std::shared_ptr<T> unwrap(
    std::shared_ptr<T> ptr, 
    const std::string& errorMsg = "Expected non-null",
    const std::source_location& loc = std::source_location::current()) {

    ensure(ptr != nullptr, errorMsg, loc);
    return ptr; 
}
