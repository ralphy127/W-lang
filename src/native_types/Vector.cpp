#include "Vector.hpp"
#include <format>
#include "runtime/RuntimeValue.hpp"
#include "utils/Logging.hpp"

RuntimeValue callVectorMethod(const Vector& vector, const std::string& name) {
    if (name == "yoink") {
        return NativeFunction{[vector](const std::vector<RuntimeValue>& args) -> RuntimeValue {
            LOG_DEBUG << "Vector:yoink called";
            if (args.size() != 1ull) {
                throw std::runtime_error{"Vector:yoink takes exactly 1 argument (index)"};
            }
            auto& arg = args[0];
            if (not std::holds_alternative<Int>(arg)) {
                // TODO ValueError ?
                throw std::runtime_error{"Vector:yoink takes an Int"};
            }
            
            auto vectorSize = vector->data.size();
            auto index = static_cast<size_t>(std::get<Int>(arg)) - 1ull;
            if (index >= vectorSize) {
                // TODO some custom OutOfBoundsError ?
                throw std::runtime_error{"Index out of bounds"};
            }
            
            auto value = vector->data.at(index);
            LOG_DEBUG << std::format("Retrieving value: {} at index{}", stringify(value), index);
            return value;
        }};
    }
    if (name == "patch") {
        return NativeFunction{[vector](const std::vector<RuntimeValue>& args) -> RuntimeValue {
            LOG_DEBUG << "Vector:patch called";
            // TODO extract common things
            if (args.empty()) {
                throw std::runtime_error{"Vector is empty"};
            }
            if (args.size() != 2ull) {
                throw std::runtime_error{"Vector:patch takes 2 arguments (index, value)"};
            }
            auto& arg = args[0];
            if (not std::holds_alternative<Int>(arg)) {
                // TODO ValueError ?
                throw std::runtime_error{"Vector:patch takes an Int as the first argument"};
            }
            
            auto vectorSize = vector->data.size();
            auto index = static_cast<size_t>(std::get<Int>(arg)) - 1ull;
            if (index >= vectorSize) {
                // TODO some custom OutOfBoundsError ?
                throw std::runtime_error{"Index out of bounds"};
            }

            auto& firstElement = vector->data.at(0ull);
            if (vector->typeId != firstElement.index()) {
                // TODO some ValueError
                throw std::runtime_error{"Wrong type"};
            }

            auto& value = args[1];
            auto& oldValue = vector->data.at(index);
            LOG_DEBUG << std::format("Changing value at index: {} from {} to {}",
                index, stringify(oldValue), stringify(value));
            oldValue = value;
            return Null{};
        }};
    }

    throw std::runtime_error{std::format("Vector does not have {} method", name)};
}