#pragma once

#include "runtime/RuntimeValue.hpp"

void expectArgsSizeRange(const std::vector<RuntimeValue>& args, size_t start, size_t end);
void expectArgsSize(const std::vector<RuntimeValue>& args, std::size_t expected);
void expectAtLeastArgs(const std::vector<RuntimeValue>& args, size_t min);
