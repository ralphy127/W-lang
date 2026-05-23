#pragma once

#include <cstdint>

enum class RuntimeValueType : std::uint8_t {
    Unset = 0,
    Null,
    String,
    Int,
    Bool,
    Float,
    Vector,
    Function,
    StructInstance,
    Module,
};
