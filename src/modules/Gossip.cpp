#include "Gossip.hpp"
#include <iostream>

// TODO error handling

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
    auto moduleMap = std::make_shared<Module::element_type>();

    (*moduleMap)["spill_tea"] = gossip::spill_tea;
    (*moduleMap)["eavesdrop"] = gossip::eavesdrop;

    return moduleMap;
}

}
