#include "common.hpp"

#include <format>

void expectArgsSize(const std::vector<RuntimeValue>& args, std::size_t expected) {
    if (args.size() != expected) {
        throw NativeError{
            // TODO RuntimeError::Type::Logic?
            // TODO different error str, improve String's as well
            RuntimeError::Type::OutOfBounds,
            std::format("Expected {} args, got {}", expected, args.size())
        };
    }
}