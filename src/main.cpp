#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"
#include "utils/Logging.hpp"
#include "errors/ErrorReporter.hpp"
#include "core/SourceManager.hpp"

static std::string readFile(const std::string& filepath) {
    const std::ifstream file(filepath);
    if (not file.is_open()) {
        // TODO handle this differently
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    std::stringstream buffer{};
    buffer << file.rdbuf();
    return buffer.str();
}

static void run(const std::string& filePath, SourceManager& srcManager) {
    AstResolver resolver = [&srcManager](const std::string& filePath) -> std::vector<std::unique_ptr<Stmt>> {
        const auto currentFileId = srcManager.registerFile(filePath);
        Lexer lexer{readFile(filePath), currentFileId};
        auto lexerResult = lexer.tokenize();
        if (not lexerResult.errors.empty()) {
            throw LexerCrash{filePath, std::move(lexerResult.errors)};
        }

        Parser parser{std::move(lexerResult.tokens)};
        auto parserResult = parser.parse();
        if (not parserResult.errors.empty()) {
            throw ParserCrash{filePath, std::move(parserResult.errors)};
        }

        return std::move(parserResult.statements);
    };

    auto mainAst = resolver(filePath);
    auto mainFolderPath = filePath.substr(0, filePath.find_last_of('/') + 1);
    if (mainFolderPath == filePath) mainFolderPath = "./";
    Interpreter interpreter{std::move(mainAst), std::move(resolver), std::move(mainFolderPath)};
    interpreter.interpret();
}

int main(int argc, const char* argv[]) {
    ::logLevel = logLevelInfo;

    if (argc < 2) {
        std::cerr << "Usage: ./wlang <filepath>\n";
        return -1;
    }
    if (argc == 3 and strcmp(argv[2], "--debug") == 0) {
        ::logLevel = logLevelDebug;
    }

    const auto filepath = argv[1];
    ErrorReporter errorReporter{};
    SourceManager sourceManager{};

    try {
        run(std::move(filepath), sourceManager);
    }
    catch (const LexerCrash& crash) {
        errorReporter.printLexerErrors(crash);
    }
    catch (const ParserCrash& crash) {
        errorReporter.printParserErrors(crash);
    }
    catch (const RuntimeError& error) {
        errorReporter.printRuntimeError(error, sourceManager);
    }

    return 0;
}
