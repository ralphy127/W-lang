#pragma once

#include <vector>
#include <string>

struct RuntimeValue;

struct VectorImpl {
    std::vector<RuntimeValue> data{};
    size_t typeId{0ull};
};

using Vector = std::shared_ptr<VectorImpl>;

RuntimeValue callVectorMethod(const Vector&, const std::string& name);
