#pragma once

#include <string>
#include <vector>

struct LexerError;

struct LexerCrash {
    std::string fileName;
    std::vector<LexerError> errors;
};
