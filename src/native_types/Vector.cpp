#include "Vector.hpp"
#include "common.hpp"
#include "runtime/RuntimeValue.hpp"
#include "utils/Logging.hpp"
#include "errors/InternalError.hpp"

namespace {
    size_t toIndex(const RuntimeValue& arg) {
        const auto& i = as<Int>(arg);

        if (i <= 0) {
            throw NativeError{
                RuntimeError::Type::OutOfBounds,
                "Lineup place must be bigger than 1 buddy"};
        }

        return static_cast<size_t>(i - 1);
    }

    void expectInBounds(const Vector& vector, size_t index) {
        if (index >= vector->data.size()) {
            throw NativeError{
                RuntimeError::Type::OutOfBounds, 
                std::format("Lineup got only {} places, you wanted {}",
                            vector->data.size(), index)
            };
        }
    }

    void expectNotEmpty(const Vector& vector) {
        if (vector->data.empty()) {
            throw NativeError{
                RuntimeError::Type::Logic, "Lineup is empty"};
        }
    }

    void expectTheSameTypes(const Vector& vector, const RuntimeValue& value) {
        if (vector->type != getType(value)) {
            throw NativeError{
                RuntimeError::Type::TypeMismatch, "Vibes don't match"};
        }
    }
}

std::string stringify(VectorMethod method) {
    switch (method) {
        case VectorMethod::Get: return "yoink";
        case VectorMethod::Set: return "patch";
        case VectorMethod::PushBack: return "shove";
        case VectorMethod::PopBack: return "kick";
        case VectorMethod::IsEmpty: return "vibe_check";
        case VectorMethod::Size: return "vibe_count";
        case VectorMethod::Clear: return "reset_the_vibe";
        default:
            throw NativeError{
                RuntimeError::Type::Logic,
                "Mysterious lineup stunt"};
    }
}

RuntimeValue callVectorMethod(const Vector& vector, VectorMethod method) {
    switch (method) {
        case VectorMethod::Get:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "VectorMethod::Get called";
                    expectArgsSize(args, 1ull);    
                    auto index = toIndex(args[0]);
                    expectInBounds(vector, index);
                    
                    auto value = vector->data.at(index);
                    LOG_DEBUG << std::format(
                        "Retrieving value: {} at index{}", stringify(value), index);
                    return value;
                },
                nullptr};
        case VectorMethod::Set:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "VectorMethod::Set called";
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
                },
                nullptr};
        case VectorMethod::PushBack:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "VectorMethod::Pushback called";
                    expectArgsSize(args, 1ull);
                    auto& value = args[0];
                    if (vector->data.empty() and vector->type == RuntimeValueType::Unset) {
                        vector->type = getType(value);
                    }
                    else {
                        expectTheSameTypes(vector, value);
                    }

                    LOG_DEBUG <<std::format(
                        "Adding {} to the end of the vector", stringify(value));
                    vector->data.push_back(value);
                    return Null{};
                },
                nullptr};
        case VectorMethod::PopBack:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "VectorMethod::PopBack called";
                    expectNotEmpty(vector);
                    expectArgsSize(args, 0ull);
                    auto valueToRemove = vector->data.back();

                    LOG_DEBUG << std::format(
                        "Popping {} from the end of the vector", stringify(valueToRemove));
                    vector->data.pop_back();
                    return valueToRemove;
                },
                nullptr};
        case VectorMethod::IsEmpty:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                    -> RuntimeValue {
                    LOG_DEBUG << "VectorMethod::IsEmpty called";
                    expectArgsSize(args, 0ull);

                    LOG_DEBUG << std::format("Checking if a vector is empty");
                    return Bool{vector->data.empty()};
                },
                nullptr};
        case VectorMethod::Size:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "VectorMethod::Size called";
                    expectArgsSize(args, 0ull);

                    LOG_DEBUG << std::format("Counting vector size");
                    return static_cast<Int>(vector->data.size());
                },
                nullptr};
        case VectorMethod::Clear:
            return Function{
                [vector](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "Vector:reset_the_vibe called";
                    expectArgsSize(args, 0ull);

                    LOG_DEBUG << std::format("Reseting vector");
                    vector->data.clear();
                    vector->type = RuntimeValueType::Unset;
                    return Null{};
                }, 
                nullptr};
        default:
            throw InternalError{"Unhandled vector method, interpreter should have caught this"};
    }
}
