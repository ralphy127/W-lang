#include "common.hpp"

#include <format>

void expectArgsSizeRange(const std::vector<RuntimeValue>& args, size_t start, size_t end) {
    if (args.size() < start) {
        throw NativeError{
            RuntimeError::Type::Logic,
            std::format(
                "Got empty places. This hustle demands {} places filled, "
                "but you only showed up with {}.",
                start,
                args.size())};
    }
    else if (args.size() > end) {
        throw NativeError{
            RuntimeError::Type::Logic,
            std::format(
                "Overbooked. This hustle demands {} places filled, but you crammed {} in.",
                end,
                args.size())};
    }
}

void expectArgsSize(const std::vector<RuntimeValue>& args, std::size_t expected) {
    expectArgsSizeRange(args, expected, expected);
}

void expectAtLeastArgs(const std::vector<RuntimeValue>& args, size_t min) {
    expectArgsSizeRange(args, min, static_cast<size_t>(-1));
}
