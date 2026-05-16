#pragma once

#include <string>
#include <cstdint>

struct RuntimeValue;

using String = std::string;

enum class StringMethod : std::uint8_t {
    Unknown,
    ToInt,
    ToUpperCase,
    ToLowerCase,
    Length,
    IsEmpty,
    Trim,
    Substring,
    Concat,
    Contains
};

RuntimeValue callStringMethod(const String&, StringMethod);
