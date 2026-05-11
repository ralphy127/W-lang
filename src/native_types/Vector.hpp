#pragma once

#include <vector>
#include <string>
#include <memory>
#include "runtime/RuntimeValueType.hpp"

struct RuntimeValue;

struct VectorImpl {
    std::vector<RuntimeValue> data{};
    RuntimeValueType type{RuntimeValueType::Unset};
};

using Vector = std::shared_ptr<VectorImpl>;

enum class VectorMethod : std::uint8_t {
    Unknown,
    Get,
    Set,
    PushBack,
    PopBack,
    IsEmpty,
    Size,
    Clear
};

std::string stringify(VectorMethod);

RuntimeValue callVectorMethod(const Vector&, VectorMethod);
