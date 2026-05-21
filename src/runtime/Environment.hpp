#pragma once

#include <unordered_map>
#include <memory>
#include "RuntimeValue.hpp"
#include "StructDefinition.hpp"

class Environment {
public:
    Environment() = default;
    Environment(std::shared_ptr<Environment> outerScope);

    void defineVar(const std::string& name, RuntimeValue);
    void reassignVar(const std::string& name, RuntimeValue);
    bool hasVar(const std::string& name) const { return _variables.contains(name); }
    RuntimeValue getVar(const std::string& name) const;

    void defineStruct(const std::string& name, StructDefinition);
    StructDefinition getStructDefinition(const std::string& name);

private:
    std::unordered_map<std::string, RuntimeValue> _variables{};
    std::unordered_map<std::string, StructDefinition> _structDefinitions{};
    std::shared_ptr<Environment> _outerScope{nullptr};
};
