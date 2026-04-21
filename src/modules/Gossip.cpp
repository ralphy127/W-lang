#include "Gossip.hpp"
#include "runtime/Environment.hpp"
#include <iostream>

namespace modules {
namespace gossip {

RuntimeValue spill_tea(const std::vector<RuntimeValue>& args) {
    if (not args.empty()) {
        for (const auto& arg : args) {
            std::cout << stringify(arg);
        }
    }

    std::cout << '\n';
    return Null{};
}

RuntimeValue eavesdrop(const std::vector<RuntimeValue>& args) {
    if (not args.empty()) {
        for (const auto& arg : args) {
            std::cout << stringify(arg);
        }
    }

    std::string input{};
    std::getline(std::cin, input);
    return String{input};
}

}

Module createGossipModule() {
    auto moduleEnv = std::make_unique<Environment>();

    moduleEnv->defineVar("spill_tea", Function{gossip::spill_tea});
    moduleEnv->defineVar("eavesdrop", Function{gossip::eavesdrop});

    return Module{std::move(moduleEnv)};
}

}
