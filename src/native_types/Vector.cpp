#include "Vector.hpp"
#include <format>
#include "runtime/RuntimeValue.hpp"
#include "utils/Logging.hpp"

namespace {
    size_t toIndex(const RuntimeValue& arg) {
        const auto& i = tryAs<Int>(arg);

        if (i <= 0) {
            throw std::runtime_error{"Index must be >= 1"};
        }

        return static_cast<size_t>(i - 1);
    }

    // TODO error handling - global problem

    void expectArgsSize(const std::vector<RuntimeValue>& args, size_t expected) {
        if (args.size() != expected) {
            throw std::runtime_error{
                std::format("Expected {} args, got {}", expected, args.size())};
        }
    }

    void expectInBounds(const Vector& vector, size_t index) {
        if (index >= vector->data.size()) {
            throw std::runtime_error{
                std::format("Index {} out of bounds (size = {})",
                            index, vector->data.size())
            };
        }
    }

    void expectNotEmpty(const Vector& vector) {
        if (vector->data.empty()) {
            throw std::runtime_error{"Vector is empty"};
        }
    }

    void expectTheSameTypes(const Vector& vector, const RuntimeValue& value) {
        // TODO could use some enum instead of relying on variant's index
        if (vector->typeId != value.index()) {
            // TODO some ValueError
            throw std::runtime_error{"Wrong type"};
        }
    }
}

RuntimeValue callVectorMethod(const Vector& vector, const std::string& name) {
    // TODO change strings to enums + switch stmt
    if (name == "yoink") {
        return NativeFunction{[vector](const std::vector<RuntimeValue>& args) -> RuntimeValue {
            LOG_DEBUG << "Vector:yoink called";
            expectArgsSize(args, 1ull);    
            auto index = toIndex(args[0]);
            expectInBounds(vector, index);
            
            auto value = vector->data.at(index);
            LOG_DEBUG << std::format("Retrieving value: {} at index{}", stringify(value), index);
            return value;
        }};
    }
    if (name == "patch") {
        return NativeFunction{[vector](const std::vector<RuntimeValue>& args) -> RuntimeValue {
            LOG_DEBUG << "Vector:patch called";
            expectNotEmpty(vector);
            expectArgsSize(args, 2ull);
            auto index = toIndex(args[0]);
            expectInBounds(vector, index);
            auto& value = args[1];
            expectTheSameTypes(vector, value);

            auto& oldValue = vector->data.at(index);
            LOG_DEBUG << std::format("Changing value at index: {} from {} to {}",
                index, stringify(oldValue), stringify(value));
            oldValue = value;
            return Null{};
        }};
    }

    throw std::runtime_error{std::format("Vector does not have {} method", name)};
}