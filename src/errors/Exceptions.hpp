#pragma once

#include <string>
#include <vector>
#include <format>

struct LexerError;
struct ParserError;

struct LexerCrash {
    std::string fileName;
    std::vector<LexerError> errors;
};

struct ParserCrash {
    std::string fileName;
    std::vector<ParserError> errors;
};
