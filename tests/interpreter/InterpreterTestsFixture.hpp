#pragma once

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>
#include <string>

#include "interpreter/Interpreter.hpp"
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "runtime/RuntimeErrors.hpp"

struct InterpreterTests : ::testing::Test {
    AstResolver astSolver = [](const std::string&) {
        return std::vector<std::unique_ptr<Stmt>>{};
    };

    std::vector<std::unique_ptr<Stmt>> parseSource(const std::string& source) {
        Lexer lexer{source, 0ull};
        auto lexerResult = lexer.tokenize();
        EXPECT_TRUE(lexerResult.errors.empty());

        Parser parser{std::move(lexerResult.tokens)};
        auto parserResult = parser.parse();
        EXPECT_TRUE(parserResult.errors.empty());
        return std::move(parserResult.statements);
    }

    std::string executeAndCaptureOutput(const std::string& source) {
        auto statements = parseSource(source);

        std::stringstream buffer{};
        auto* oldCout = std::cout.rdbuf(buffer.rdbuf());

        Interpreter interpreter{std::move(statements), astSolver, ""};
        try {
            interpreter.interpret();
        }
        catch (...) {
            std::cout.rdbuf(oldCout);
            throw;
        }

        std::cout.rdbuf(oldCout);

        return buffer.str();
    }

    std::string executeAndCaptureFailure(const std::string& source) {
        auto statements = parseSource(source);

        std::stringstream buffer{};
        auto* oldCout = std::cout.rdbuf(buffer.rdbuf());

        Interpreter interpreter{std::move(statements), astSolver, ""};
        try {
            interpreter.interpret();
        }
        catch (const RuntimeError& err) {
            std::cout.rdbuf(oldCout);
            return err.msg;
        }
        catch (...) {
            std::cout.rdbuf(oldCout);
            throw;
        }

        std::cout.rdbuf(oldCout);
        ADD_FAILURE() << "Expected interpreter to fail, but it succeeded";
        return "";
    }

    RuntimeError executeAndCaptureRuntimeError(const std::string& source) {
        auto statements = parseSource(source);

        std::stringstream buffer{};
        auto* oldCout = std::cout.rdbuf(buffer.rdbuf());

        Interpreter interpreter{std::move(statements), astSolver, ""};
        try {
            interpreter.interpret();
        }
        catch (const RuntimeError& err) {
            std::cout.rdbuf(oldCout);
            return err;
        }
        catch (...) {
            std::cout.rdbuf(oldCout);
            throw;
        }

        std::cout.rdbuf(oldCout);
        ADD_FAILURE() << "Expected interpreter to throw RuntimeError, but it succeeded";
        return RuntimeError{RuntimeError::Type::Undefined, {0u, {0u, 0u}, {0u, 0u}}, ""};
    }

    void expectOutput(const std::string& source, const std::string& expectedOutput) {
        EXPECT_EQ(executeAndCaptureOutput(source), expectedOutput);
    }

    void expectFailure(const std::string& source, const std::string& expectedFailure) {
        EXPECT_EQ(executeAndCaptureFailure(source), expectedFailure);
    }

    void expectRuntimeErrorType(const std::string& source, RuntimeError::Type expectedType) {
        const auto err = executeAndCaptureRuntimeError(source);
        EXPECT_EQ(err.type, expectedType);
    }

    void expectRuntimeError(const std::string& source, RuntimeError::Type expectedType, const std::string& expectedMsg) {
        const auto err = executeAndCaptureRuntimeError(source);
        EXPECT_EQ(err.type, expectedType);
        EXPECT_EQ(err.msg, expectedMsg);
    }

    void expectRuntimeErrorMsgContains(const std::string& source, RuntimeError::Type expectedType, const std::string& needle) {
        const auto err = executeAndCaptureRuntimeError(source);
        EXPECT_EQ(err.type, expectedType);
        EXPECT_NE(err.msg.find(needle), std::string::npos) << "Full message: " << err.msg;
    }
};
