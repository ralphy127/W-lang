#include "String.hpp"
#include <vector>
#include <format>
#include "runtime/RuntimeValue.hpp"
#include "utils/Logging.hpp"

namespace {
    // TODO can be extracted (Vector has similar)
    void expectArgsSize(const std::vector<RuntimeValue>& args, size_t expected) {
        if (args.size() != expected) {
            throw std::runtime_error{
                std::format("Expected {} args, got {}", expected, args.size())};
        }
    }
}

RuntimeValue callStringMethod(const String& string, const std::string& name) {
    if (name == "to_solid") {
        return Function{[string](const std::vector<RuntimeValue>& args) -> RuntimeValue {
            LOG_DEBUG << "String:to_solid called";
            expectArgsSize(args, 0ull);    
            
            auto integer = Int{std::stoi(string)};
            LOG_DEBUG << std::format("Converted String: {} to Int: {}", string, stringify(integer));
            return integer;
        }};
    }

    throw std::runtime_error{std::format("String does not have {} method", name)};
}
