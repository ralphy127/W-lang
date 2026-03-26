#pragma once

#include "types.hpp"

struct SourceRange {
    FileId fileId;
    SourcePosition start;
    SourcePosition end;
};
