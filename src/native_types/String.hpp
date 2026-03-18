#pragma once

#include <string>

struct RuntimeValue;

using String = std::string;

RuntimeValue callStringMethod(const String&, const std::string& name);
