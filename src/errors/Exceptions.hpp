#pragma once

#include <string>
#include <vector>

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
