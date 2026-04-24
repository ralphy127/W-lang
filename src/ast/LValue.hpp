#pragma once

#include <string>
#include <memory>
#include <variant>

class Expr;

struct LValue {
    struct Variable {
        std::string name;
    };

    // TODO property

    std::variant<Variable> location;
};
