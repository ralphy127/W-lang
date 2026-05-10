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

RuntimeValue callVectorMethod(const Vector&, const std::string& name);
