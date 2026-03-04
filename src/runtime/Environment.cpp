#include "Environment.hpp"

#include <cassert>
#include <format>

Environment::Environment(std::shared_ptr<Environment> outerScope)
    : _outerScope{std::move(outerScope)} {}

void Environment::defineVar(const std::string& name, RuntimeValue value) {
    if (_variables.contains(name)) {
        throw std::runtime_error{std::format("Variable {} already exists", name)};
    }
    _variables.emplace(name, std::move(value));
}

void Environment::reassignVar(const std::string& name, RuntimeValue newValue) {
    if (_variables.contains(name)) {
        _variables[name] = std::move(newValue);
        return;
    }
    if (_outerScope) {
        _outerScope->reassignVar(name, std::move(newValue));
        return;
    }
    throw std::runtime_error{std::format("Variable {} does not exist", name)};
}

RuntimeValue Environment::getVar(const std::string& name) const {
    if (_variables.contains(name)) {
        return _variables.at(name);
    }
    if (_outerScope) {
        return _outerScope->getVar(name);
    }
    throw std::runtime_error{std::format("Variable {} does not exist", name)};
}
