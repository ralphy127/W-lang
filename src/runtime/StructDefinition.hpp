#pragma once

#include "runtime/RuntimeValue.hpp"
#include <string>
#include <vector>

struct StructDefinition {
    std::vector<std::string> fields;
    std::unordered_map<std::string, Function> methods;
};
