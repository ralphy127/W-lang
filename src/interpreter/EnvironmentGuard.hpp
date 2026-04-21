#pragma once
#include "Interpreter.hpp"

class EnvironmentGuard {
public:
    EnvironmentGuard(Interpreter& interpreter, std::shared_ptr<Environment> newEnv)
        : _interpreter(interpreter), 
          _previousEnv(interpreter.getCurrentEnvironment()) {
        
        _interpreter.setCurrentEnvironment(newEnv); 
        _interpreter.incrementScopeDepth(); 
    }

    ~EnvironmentGuard() {
        _interpreter.decrementScopeDepth();
        _interpreter.setCurrentEnvironment(_previousEnv);
    }

private:
    Interpreter& _interpreter;
    std::shared_ptr<Environment> _previousEnv;
};
