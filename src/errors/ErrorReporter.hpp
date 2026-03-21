#pragma once

#include "Exceptions.hpp"

class ErrorReporter {
public:
    void printLexerErrors(const LexerCrash&);
};
