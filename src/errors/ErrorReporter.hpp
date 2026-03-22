#pragma once

#include "Exceptions.hpp"
#include "runtime/RuntimeErrors.hpp"

class ErrorReporter {
public:
    void printLexerErrors(const LexerCrash&);
    void printParserErrors(const ParserCrash&);
    void printRuntimeError(const RuntimeError&, const std::string&);
};
