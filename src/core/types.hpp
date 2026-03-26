#pragma once

#include <cstdint>

using FileId = size_t;

struct SourcePosition {
    std::uint32_t line;
    std::uint32_t column;
};
