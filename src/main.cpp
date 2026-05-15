#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <exception>
#include <functional>
#include <format>
#include <filesystem>
#include <string_view>
#include <utility>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"
#include "utils/Logging.hpp"
#include "errors/ErrorReporter.hpp"
#include "core/SourceManager.hpp"
#include "utils/AstPrinter.hpp"

namespace {

struct RunOptions {
    bool dumpAst{false};
};

enum class ExitCode : int {
    AstDump = 0,
    Failure,
    Usage
};

std::string readFile(const std::string& filepath) {
    const std::ifstream file(filepath);
    if (not file.is_open()) {
        throw std::runtime_error{"Cannot open file: " + filepath};
    }
    
    std::stringstream buffer{};
    buffer << file.rdbuf();
    return buffer.str();
}

std::string getMainFolderPath(const std::filesystem::path& file) {
    auto parent = file.parent_path();
    if (parent.empty()) {
        parent = std::filesystem::path{"."};
    }
    return (parent / "").string();
}

bool checkValidFile(const std::filesystem::path& path){
    if (not std::filesystem::exists(path)) {
        std::cerr << std::format("This doesn't exist: {}\n", path.string());
        return false;
    }
    if (std::filesystem::is_directory(path)) {
        std::cerr << std::format(
        "Cannot run a directory: {}\n",
        path.string());
        return false;
    }
    if (not std::filesystem::is_regular_file(path)) {
        std::cerr << std::format(
            "This isn't a regular file: {}\n",
            path.string());
        return false;
    }

    return true;
}

AstResolver createAstResolver(SourceManager& srcManager) {
    return [&srcManager](const std::string& filePath) -> std::vector<std::unique_ptr<Stmt>> {
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
}

int run(
    const std::filesystem::path& filePath,
    SourceManager& srcManager,
    const RunOptions& options) {

    auto astResolver = createAstResolver(srcManager);

    auto mainAst = astResolver(filePath.string());

    if (options.dumpAst) {
        AstPrinter{}.print(mainAst);
        return std::to_underlying(ExitCode::AstDump);
    }

    Interpreter interpreter{
        std::move(mainAst),
        std::move(astResolver),
        getMainFolderPath(filePath)};
    return interpreter.interpret();
}

}

int main(int argc, const char* argv[]) {
    ::logLevel = logLevelInfo;

    if (argc < 2) {
        std::cerr << std::format(
            "Usage: {} <filepath> [--debug] [--dump-ast]\n", argv[0]);
        return std::to_underlying(ExitCode::Usage);
    }

    const std::filesystem::path inputPath{argv[1]};
    if (not checkValidFile(inputPath)) {
        return std::to_underlying(ExitCode::Usage);
    }

    RunOptions options{};
    for (int i{2}; i < argc; ++i) {
        const std::string_view flag{argv[i]};
        if (flag == "--debug") {
            ::logLevel = logLevelDebug;
        }
        else if (flag == "--dump-ast") {
            options.dumpAst = true;
        }
        else {
            std::cerr << std::format("Unknown flag: {}\n", flag);
            return std::to_underlying(ExitCode::Usage);
        }
    }

    ErrorReporter errorReporter{};
    SourceManager sourceManager{};

    try {
        return run(inputPath, sourceManager, options);
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
    catch(const InternalError& error) {
        errorReporter.printInternalError(error);
    }
    catch (const std::exception& e) {
        std::cerr << std::format("Trainwreck: {}\n", e.what());
    }
    return std::to_underlying(ExitCode::Failure);
}
