#include "Environment.hpp"
#include <format>

Environment::Environment(std::shared_ptr<Environment> outerScope)
    : _outerScope{std::move(outerScope)} {}


// TODO better variable error msg

void Environment::defineVar(const std::string& name, RuntimeValue value) {
    if (hasVar(name)) {
        throw NativeError{
            RuntimeError::Type::Logic,
            std::format("Variable {} already exists", name)};
    }
    _variables.emplace(name, std::move(value));
}

void Environment::reassignVar(const std::string& name, RuntimeValue newValue) {
    if (hasVar(name)) {
        _variables[name] = std::move(newValue);
        return;
    }
    if (_outerScope) {
        _outerScope->reassignVar(name, std::move(newValue));
        return;
    }
    throw NativeError{
        RuntimeError::Type::Logic,
        std::format("Variable {} does not exist", name)};
}

RuntimeValue Environment::getVar(const std::string& name) const {
    if (hasVar(name)) {
        return _variables.at(name);
    }
    if (_outerScope) {
        return _outerScope->getVar(name);
    }
    throw NativeError{
        RuntimeError::Type::Logic,std::format("Variable {} does not exist", name)};
}

bool Environment::hasStructDefinition(const std::string& name) const {
    return _structDefinitions.contains(name);
}

void Environment::defineStruct(const std::string& name, StructDefinition structDefinition) {
    if (hasStructDefinition(name)) {
        throw NativeError{
            RuntimeError::Type::Logic,
            std::format("Crew {} is not happy you wanted to copy their name", name)};
    }
    _structDefinitions.emplace(name, structDefinition);
}

const StructDefinition& Environment::getStructDefinition(const std::string& name) const {
    if (hasStructDefinition(name)) {
        return _structDefinitions.at(name);
    }
    if (_outerScope) {
        return _outerScope->getStructDefinition(name);
    }
    throw NativeError{
        RuntimeError::Type::Logic, std::format("Crew {} is not recruiting right now", name)};
}
