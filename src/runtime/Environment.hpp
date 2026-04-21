#pragma once

#include <unordered_map>
#include <memory>
#include "RuntimeValue.hpp"

class Environment {
public:
    Environment() = default;
    Environment(std::shared_ptr<Environment> outerScope);

    void defineVar(const std::string& name, RuntimeValue value);
    void reassignVar(const std::string& name, RuntimeValue newValue);
    bool hasVar(const std::string& name) const { return _variables.contains(name); }
    RuntimeValue getVar(const std::string& name) const;
    std::unordered_map<std::string, RuntimeValue> stealAllVariables() { return std::move(_variables); }

private:
    std::unordered_map<std::string, RuntimeValue> _variables{};
    std::shared_ptr<Environment> _outerScope{nullptr};
};