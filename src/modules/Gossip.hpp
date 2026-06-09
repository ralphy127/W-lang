#pragma once

#include "runtime/RuntimeValue.hpp"

namespace modules {
namespace gossip {

RuntimeValue spill_tea(
    const std::vector<RuntimeValue>& args,
    std::shared_ptr<Environment> = nullptr);

RuntimeValue eavesdrop(
    const std::vector<RuntimeValue>& args,
    std::shared_ptr<Environment> = nullptr);

}

Module createGossipModule();

}
