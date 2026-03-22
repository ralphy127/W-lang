#pragma once

#include <cstdint>

struct SourceLocation {
    std::uint32_t line;
    std::uint32_t column;
};

struct SourceRange {
    SourceLocation start;
    SourceLocation end;
};
