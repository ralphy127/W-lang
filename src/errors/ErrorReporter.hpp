#pragma once

#include "Exceptions.hpp"
#include "core/SourceManager.hpp"
#include "runtime/RuntimeErrors.hpp"

class ErrorReporter {
public:
    void printLexerErrors(const LexerCrash&);
    void printParserErrors(const ParserCrash&);
    void printRuntimeError(const RuntimeError&, const SourceManager&);
    void printInternalError(const InternalError&);
};
