#include "common.hpp"

#include <format>

void expectArgsSize(const std::vector<RuntimeValue>& args, std::size_t expected) {
    if (args.size() != expected) {
        throw NativeError{
            RuntimeError::Type::OutOfBounds,
            std::format("Expected {} args, got {}", expected, args.size())
        };
    }
}