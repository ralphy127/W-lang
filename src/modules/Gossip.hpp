#pragma once

#include "runtime/RuntimeValue.hpp"

namespace modules {
namespace gossip {

RuntimeValue spill_tea(const std::vector<RuntimeValue>& args);
RuntimeValue eavesdrop(const std::vector<RuntimeValue>& args);

}

Module createGossipModule();

}
