#pragma once

#include <string>
#include <variant>
#include <functional>

class Expr;

struct LValue {
    struct Variable {
        std::string name;
    };

    struct Property {
        std::reference_wrapper<const Expr> object;
        std::string name;
    };

    std::variant<Variable, Property> location;
};
