#include "String.hpp"
#include <vector>
#include <algorithm>
#include <cctype>
#include "common.hpp"
#include "runtime/RuntimeValue.hpp"
#include "utils/Logging.hpp"
#include "errors/InternalError.hpp"

namespace {
    template<typename UnaryOp>
    String convertString(const String& str, UnaryOp&& op) {
        auto newStr = String{str};

        std::transform(
            newStr.begin(),
            newStr.end(),
            newStr.begin(),
            op);

        LOG_DEBUG << std::format("Converted String: {} to {}", str, newStr);
        return newStr;
    }
}

RuntimeValue callStringMethod(const String& str, StringMethod method) {
    switch (method) {
        case StringMethod::ToInt:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::ToInt called on: " << str;
                    expectArgsSize(args, 0ull);    
                    
                    const auto integer = Int{std::stoi(str)};
                    LOG_DEBUG << std::format(
                        "Converted String: {} to Int: {}", str, stringify(integer));
                    return integer;
                },
                nullptr};
        case StringMethod::ToUpperCase:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::ToUpperCase called on: " << str;
                    expectArgsSize(args, 0ull);
                    return convertString(str, ::toupper);
                },
                nullptr};
        case StringMethod::ToLowerCase:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::ToLowerCase called on: " << str;
                    expectArgsSize(args, 0ull);
                    return convertString(str, ::tolower);
                },
                nullptr};
        case StringMethod::Length:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::Length called on: " << str;
                    expectArgsSize(args, 0ull);
                    return static_cast<Int>(str.size());
                },
                nullptr};
        case StringMethod::IsEmpty:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::IsEmpty called on: " << str;
                    expectArgsSize(args, 0ull);
                    const auto isEmpty = Bool{str.empty()};
                    LOG_DEBUG << "Returning IsEmpty: " << isEmpty;
                    return isEmpty;
                },
                nullptr};
        case StringMethod::Trim:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::Trim called on: " << str;
                    expectArgsSize(args, 0ull);
                    auto newStr = String{str};

                    auto start = std::find_if_not(
                        newStr.begin(),
                        newStr.end(),
                        [](const auto ch) { return std::isspace(ch); });
                    auto end = std::find_if_not(
                        newStr.rbegin(),
                        newStr.rend(),
                        [](const auto ch) { return std::isspace(ch); }).base();
                    
                    LOG_DEBUG << std::format("Converted String: {} to {}", str, newStr);
                    return start < end ? String{start, end} : String{};
                },
                nullptr};
        case StringMethod::Substring:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::Substring called on: " << str;
                    expectArgsSizeRange(args, 1ull, 2ull);

                    const auto pos =
                        static_cast<size_t>(as<Int>(args[0ull]));
                    const auto count =
                        args.size() == 2ull
                        ? static_cast<size_t>(as<Int>(args[1ull]))
                        : std::string::npos;
                    const auto newStr = String{str.substr(pos, count)};
                    
                    LOG_DEBUG << std::format("Converted String: {} to {}", str, newStr);
                    return newStr;
                },
                nullptr};
        case StringMethod::Concat:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::Concat called on: " << str;
                    expectAtLeastArgs(args, 1ull);
                    auto newStr = String{str};
                    for (const auto& arg : args) {
                        newStr.append(stringify(arg));
                    }

                    LOG_DEBUG << std::format("Converted String: {} to {}", str, newStr);
                    return newStr;
                },
                nullptr};
        case StringMethod::Contains:
            return Function{
                [str](const std::vector<RuntimeValue>& args, std::shared_ptr<Environment>)
                -> RuntimeValue {
                    LOG_DEBUG << "StringMethod::Contains called on: " << str;
                    expectArgsSize(args, 1ull);

                    const auto contains = str.contains(as<String>(args[0ull]));
                    LOG_DEBUG << "Returning Contains: " << contains;
                    return contains;
                },
                nullptr};
    default:
        throw InternalError{"Unhandled String method, interpreter should have caught this"};
    }
}
