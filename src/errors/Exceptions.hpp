#pragma once

#include <string>
#include <vector>
#include <format>
#include <source_location>

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

struct InternalError {
    std::string msg;
    std::source_location loc = std::source_location::current();
};
