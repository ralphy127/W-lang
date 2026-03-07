#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"
#include "utils/Logging.hpp"

static std::string readFile(const std::string& filepath) {
    const std::ifstream file(filepath);
    if (not file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    std::stringstream buffer{};
    buffer << file.rdbuf();
    return buffer.str();
}

static void run(std::string sourceCode) {
    Lexer lexer{std::move(sourceCode)};
    auto lexerResult = lexer.tokenize();

    if (not lexerResult.errors.empty()) {
        // TODO handle lexer errors
        for (const auto& error : lexerResult.errors) {
            std::cerr << error.line << ' ' << error.column << ' ' << std::to_underlying(error.type) << '\n';
        }
        std::cerr << "Found lexing errors";
        return;
    }


    Parser parser{std::move(lexerResult.tokens)};
    auto parserResult = parser.parse();
    if (not parserResult.errors.empty()) {
        // TODO handle parser errors
        std::cerr << "Found parsing errors";
        return;
    }

    Interpreter interpreter{std::move(parserResult.statements)};
    interpreter.interpret();
}

int main(int argc, const char* argv[]) {
    ::logLevel = logLevelInfo;

    if (argc < 2) {
        std::cerr << "Usage: ./wlang <filepath>\n";
        return -1;
    }
    if (argc == 3 and strcmp(argv[2], "debug") == 0) {
        ::logLevel = logLevelDebug;
    }

    const auto filepath = argv[1];

    try {
        auto sourceCode = readFile(filepath);
        run(std::move(sourceCode));
        
    }
    catch (const std::exception& e) {
        // TODO handle exceptions
        std::cerr << std::format("[FATAL ERROR] {}\n", e.what());
        return -1;
    }

    return 0;
}
