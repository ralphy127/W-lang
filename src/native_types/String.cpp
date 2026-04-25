#include "String.hpp"
#include <vector>
#include "common.hpp"
#include "runtime/RuntimeValue.hpp"
#include "utils/Logging.hpp"

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

        throw NativeError{
            RuntimeError::Type::Logic,
            std::string{"Yap cannot do {}"} +  name};
}
