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

static void run(const std::string& filePath) {
    AstResolver resolver = [](const std::string& filePath) -> std::vector<std::unique_ptr<Stmt>> {
        Lexer lexer{readFile(filePath)};
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
    Interpreter interpreter{std::move(mainAst), std::move(resolver)};
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

    try {
        run(std::move(filepath));
    }
    catch (const LexerCrash& crash) {
        errorReporter.printLexerErrors(crash);
    }
    catch (const ParserCrash& crash) {
        errorReporter.printParserErrors(crash);
    }
    catch (const RuntimeError& error) {
        // TODO! add some info in sourceRange about file or find other way to properly
        //! get file which something is in, (test after user imports) 
        errorReporter.printRuntimeError(error, filepath);
    }

    return 0;
}
