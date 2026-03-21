#include "ErrorReporter.hpp"

#include <iostream>
#include <fstream>
#include <format>
#include <iomanip>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

namespace {

std::string createIndent(std::uint32_t column, const std::string& line) {
    std::string indent = "";
    for (int i = 0; i < column - 1 && i < line.length(); ++i) {
        if (line[i] == '\t') {
            indent += '\t';
        }
        else {
            indent += ' ';
        }
    }
    return indent;
}

void printLine(std::size_t lineNumebr, const std::string& content) {
    constexpr int margin{6};
    std::cerr << std::format("{:>{}} | {}\n", lineNumebr, margin, content);
};

void printError(
    const std::string& fileName,
    std::uint32_t line,
    std::uint32_t column,
    std::uint32_t length,
    const std::string& msg) {
    std::ifstream file{fileName};

    std::cerr << std::format("{}:{}:{} error: {}\n", fileName, line, column, msg);
    if (not file.is_open()) return;

    std::string prevLine;
    std::string currLine;
    std::string nextLine;

    bool hasCurr = static_cast<bool>(std::getline(file, currLine));

    for (std::uint32_t curr{1u}; curr <= line && hasCurr; ++curr) {
        const bool hasNext = static_cast<bool>(std::getline(file, nextLine));

        if (curr == line) {
            constexpr int margin{6};

            if (curr > 1) {
                printLine(curr - 1, prevLine);
            }
            
            printLine(curr, currLine);

            std::cerr << std::setw(margin) << "" << " | "
                      << createIndent(column, currLine)
                      << std::string(length, '^')
                      << "\n";

            if (hasNext) {
                printLine(curr + 1, nextLine);
            }

            std::cerr << '\n';
            break;
        }

        prevLine = currLine;
        currLine = nextLine;
        hasCurr = hasNext;
    }
}

}

void ErrorReporter::printLexerErrors(const LexerCrash& crash) {
    std::cerr << '\n';
    for (const auto& error : crash.errors) {
        std::string msg;
        switch(error.type) {
            case LexerErrorType::UnterminatedString: msg = "Yap has no end quote!"; break;
            case LexerErrorType::UnterminatedBlockComment: msg = "Ranting never ends!"; break;
            case LexerErrorType::UnknownToken: msg = "What the heck is this character?"; break;
        }
        printError(crash.fileName, error.line, error.column, error.length, msg);
    }
    std::cerr << '\n';
}

void ErrorReporter::printParserErrors(const ParserCrash& crash) {
    std::cerr << '\n';
    for (const auto& error : crash.errors) {
        printError(
            crash.fileName,
            error.badToken.getLine(),
            error.badToken.getColumn(),
            1u,
            error.msg);
    }
    std::cerr << '\n';
}
