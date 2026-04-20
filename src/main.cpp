#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <functional>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"
#include "utils/Logging.hpp"
#include "errors/ErrorReporter.hpp"
#include "core/SourceManager.hpp"
#include "utils/AstPrinter.hpp"

struct RunOptions {
    bool dumpAst{false};
};

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

static int run(const std::string& filePath, SourceManager& srcManager, RunOptions options) {
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

    if (options.dumpAst) {
        AstPrinter{}.print(mainAst);
    }

    auto mainFolderPath = filePath.substr(0, filePath.find_last_of('/') + 1);
    if (mainFolderPath == filePath) mainFolderPath = "./";
    Interpreter interpreter{std::move(mainAst), std::move(resolver), std::move(mainFolderPath)};
    return interpreter.interpret();
}

int main(int argc, const char* argv[]) {
    ::logLevel = logLevelInfo;

    if (argc < 2) {
        std::cerr << "Usage: ./wlang <filepath>\n";
        return 1;
    }

    RunOptions options{};
    for (int i{2}; i < argc; ++i) {
        const char* flag = argv[i];
        if (strcmp(flag, "--debug") == 0) {
            ::logLevel = logLevelDebug;
        }
        else if (strcmp(flag, "--dump_ast") == 0) {
            options.dumpAst = true;
        }
        else {
            std::cerr << "Unknown flag: " << flag << std::endl;
            return 1;
        }
    }

    const auto filepath = argv[1];
    ErrorReporter errorReporter{};
    SourceManager sourceManager{};

    try {
        return run(std::move(filepath), sourceManager, options);
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
    catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }
}
