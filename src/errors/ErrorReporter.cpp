#include "ErrorReporter.hpp"

#include <iostream>
#include <fstream>
#include <format>
#include <iomanip>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "utils/Logging.hpp"

namespace {

std::string createIndent(std::uint32_t column, const std::string& line) {
    std::string indent{""};
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
    std::uint32_t startLine,
    std::uint32_t startColumn,
    std::uint32_t endLine,
    std::uint32_t endColumn,
    const std::string& msg) {
    std::ifstream file{fileName};

    LOG_DEBUG << "Printing error: " << msg;
    std::cerr << std::format("{}:{}:{} - {}\n", fileName, startLine, startColumn, msg);
    if (not file.is_open()) {
        return;
    }

    std::string prevLine{};
    std::string currLine{};
    std::string nextLine{};

    bool hasCurr = static_cast<bool>(std::getline(file, currLine));

    for (std::uint32_t curr{1u}; curr <= endLine && hasCurr; ++curr) {
        const bool hasNext = static_cast<bool>(std::getline(file, nextLine));

        if (curr >= startLine and curr <= endLine) {
            constexpr int margin{6};

            if (curr == startLine && curr > 1) {
                printLine(curr - 1, prevLine);
            }
            
            printLine(curr, currLine);

            auto col = curr == startLine ? startColumn : 1u;
            if (curr != startLine) {
                while (col <= currLine.length() and
                       (currLine[col - 1] == ' ' or currLine[col - 1] == '\t')) {
                    col++;
                }
            }

            auto length = currLine.length();
            if (curr == endLine) {
                if (curr == startLine) {
                    length = endColumn > startColumn ? (endColumn - startColumn) : 1u;
                }
                else {
                    length = endColumn > col ? (endColumn - col) : 1u;
                }
            }
            else if (curr == startLine) {
                length =
                    currLine.length() >= startColumn ? (currLine.length() - startColumn + 1u) : 1u;
            }
            else {
                length = currLine.length() >= col ? (currLine.length() - col + 1) : 1u;
            }

            if (length == 0) {
                length = 1;
            }

            std::cerr << std::setw(margin) << "" << " | "
                      << createIndent(col, currLine)
                      << std::string(length, '^')
                      << "\n";

            if (curr == endLine && hasNext) {
                printLine(curr + 1, nextLine);
            }

            if (curr == endLine) {
                std::cerr << '\n';
                break;
            }
        }

        prevLine = currLine;
        currLine = nextLine;
        hasCurr = hasNext;
    }
}

std::string runtimeErrorTypeToString(RuntimeError::Type type) {
    switch (type) {
        case RuntimeError::Type::Undefined:
            return "Mystery";
        case RuntimeError::Type::Logic:
            return "BrainLag";
        case RuntimeError::Type::Math:
            return "MathOops";
        case RuntimeError::Type::OutOfBounds:
            return "OuttaBounds";
        case RuntimeError::Type::TypeMismatch:
            return "Nope";
    }
    return "Unknown";
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
            case LexerErrorType::EmptySource: msg = "You sure this does anything?"; break;
        }
        printError(
            crash.fileName,
            error.line,
            error.column,
            error.line,
            error.column + error.length,
            msg);
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
            error.badToken.getLine(),
            error.badToken.getColumn() + 1u,
            error.msg);
    }
    std::cerr << '\n';
}

void ErrorReporter::printRuntimeError(
    const RuntimeError& error,
    const SourceManager& sourceManager) {

    std::cerr << '\n';
    const auto typeStr = runtimeErrorTypeToString(error.type);
    const auto msg = std::format("[{}] {}", typeStr, error.msg);
    
    std::string filePath{"<unknown-file>"};
    try {
        filePath = sourceManager.getFilePath(error.srcRange.fileId);
    }
    catch (const std::out_of_range&) {
    }

    printError(
        filePath, 
        error.srcRange.start.line, 
        error.srcRange.start.column, 
        error.srcRange.end.line, 
        error.srcRange.end.column, 
        msg);
    std::cerr << '\n';
}

void ErrorReporter::printInternalError(const InternalError& error) {
    auto& loc = error.loc;
    auto msg = std::format(
        "Internal error, W-Lang just crashed.\n"
        "This is a bug, please report it.\n"
        "Reason  : {}\n"
        "C++ File: {}:{}\n"
        "Function: {}\n",
        error.msg, loc.file_name(), loc.line(), loc.function_name());
    std::cerr << msg;
}
