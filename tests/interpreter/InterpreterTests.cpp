#include <gtest/gtest.h>
#include <iostream>
#include <sstream>
#include <string>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"
#include "runtime/RuntimeErrors.hpp"

struct InterpreterTests : ::testing::Test {
    AstResolver astSolver = [](const std::string&) {
        return std::vector<std::unique_ptr<Stmt>>{};
    };

    ParserResult parseSource(const std::string& source) {
        Lexer lexer{source, 0ull};
        auto lexerResult = lexer.tokenize();
        EXPECT_TRUE(lexerResult.errors.empty());
        
        Parser parser{std::move(lexerResult.tokens)};
        return parser.parse();
    }
    
    std::string executeAndCaptureOutput(const std::string& source) {
        auto statements = parseSource(source).statements;
        
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
        auto statements = parseSource(source).statements;

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
        auto statements = parseSource(source).statements;

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


TEST_F(InterpreterTests, MvpExecutesMachoFunction) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            gossip.spill_tea("F*ck me it works")...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "F*ck me it works\n");
}

TEST_F(InterpreterTests, MvpExecutesVariableStashAndScream) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash number about 10...
            gossip.spill_tea(number)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "10\n");
}

TEST_F(InterpreterTests, DefaultVarDefinition_InitializesToNullAndCanBeReassigned) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash x...
            gossip.spill_tea(x)...
            x might_be 10...
            gossip.spill_tea(x)...
        }
    )";

    expectOutput(source, "ghosted\n10\n");
}

TEST_F(InterpreterTests, OutputsCorrectBooleanValues) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash equal about 1 looks_like 1...
            gossip.spill_tea(equal)...
            stash notEqual about 1 looks_like 2...
            gossip.spill_tea(notEqual)...
            stash smaller about 1 tiny_ish 2...
            gossip.spill_tea(smaller)...
            stash notSmaller about 2 tiny_ish 1...
            gossip.spill_tea(notSmaller)...
            stash greater about 2 bigger_ish 1...
            gossip.spill_tea(greater)...
            stash notGreater about 1 bigger_ish 2...
            gossip.spill_tea(notGreater)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "totally\nnah\ntotally\nnah\ntotally\nnah\n");
}

TEST_F(InterpreterTests, EvaluatesLogicalOr) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash result1 about totally either nah...
            gossip.spill_tea(result1)...
            stash result2 about nah either nah...
            gossip.spill_tea(result2)...
            yeet ghosted...
        }
    )";

    expectOutput(source, "totally\nnah\n");
}

TEST_F(InterpreterTests, EvaluatesLogicalAnd) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash result1 about totally also totally...
            gossip.spill_tea(result1)...
            stash result2 about totally also nah...
            gossip.spill_tea(result2)...
            yeet ghosted...
        }
    )";

    expectOutput(source, "totally\nnah\n");
}

TEST_F(InterpreterTests, LogicalAndShortCircuitsRightSide) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash board about [1, 2, 3]...
            perhaps ((nah also (board.yoink(99) looks_like 1))) {
                gossip.spill_tea("should not print")...
            }
            gossip.spill_tea("ok")...
        }
    )";

    expectOutput(source, "ok\n");
}

TEST_F(InterpreterTests, LogicalOrShortCircuitsRightSide) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash board about [1, 2, 3]...
            perhaps ((totally either (board.yoink(99) looks_like 1))) {
                gossip.spill_tea("ok")...
            }
            screw_it {
                gossip.spill_tea("should not print")...
            }
        }
    )";

    expectOutput(source, "ok\n");
}

TEST_F(InterpreterTests, EvaluatesMixedLogicalInPerhaps) {
    auto source = R"(
        summon gossip...

        gig macho() {
            perhaps (totally also nah either totally) {
                gossip.spill_tea("mixed true")...
            }
            screw_it {
                gossip.spill_tea("mixed false")...
            }
            yeet ghosted...
        }
    )";

    expectOutput(source, "mixed true\n");
}

TEST_F(InterpreterTests, EvaluatesHeavilyNestedLogicalInPerhaps) {
    // TODO fix line breaking
    auto source = R"(
        summon gossip...

        gig macho() {
            perhaps ((((totally also nah) either (totally also totally)) also ((nah either totally) either (totally also (totally either nah)))) either ((nah also totally) either (nah also (totally either nah)))) {
                gossip.spill_tea("nested true")...
            }
            screw_it {
                gossip.spill_tea("nested false")...
            }
            yeet ghosted...
        }
    )";

    expectOutput(source, "nested true\n");
}

TEST_F(InterpreterTests, SimpleIfWithTrueCondition) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            perhaps (totally) {
                gossip.spill_tea("if executed")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "if executed\n");
}

TEST_F(InterpreterTests, SimpleIfWithFalseCondition) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            perhaps (nah) {
                gossip.spill_tea("should not print")...
            }
            gossip.spill_tea("after if")...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "after if\n");
}

TEST_F(InterpreterTests, IfElseWithTrueCondition) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            perhaps (totally) {
                gossip.spill_tea("if branch")...
            }
            screw_it {
                gossip.spill_tea("else branch")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "if branch\n");
}

TEST_F(InterpreterTests, IfElseWithFalseCondition) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            perhaps (nah) {
                gossip.spill_tea("if branch")...
            }
            screw_it {
                gossip.spill_tea("else branch")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "else branch\n");
}

TEST_F(InterpreterTests, IfElseIfChainMatchesSecondCondition) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash value about 15...
            perhaps (value tiny_ish 10) {
                gossip.spill_tea("less than 10")...
            }
            or_whatever (value looks_like 15) {
                gossip.spill_tea("equals 15")...
            }
            screw_it {
                gossip.spill_tea("greater than 15")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "equals 15\n");
}

TEST_F(InterpreterTests, IfWithComparisonOperators) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash x about 10...
            perhaps (x bigger_ish 5) {
                gossip.spill_tea("greater than 5")...
            }
            perhaps (x tiny_ish 20) {
                gossip.spill_tea("less than 20")...
            }
            perhaps (x looks_like 10) {
                gossip.spill_tea("equals 10")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "greater than 5\nless than 20\nequals 10\n");
}

TEST_F(InterpreterTests, NestedIfStatements) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash x about 10...
            stash y about 20...
            perhaps (x tiny_ish 15) {
                perhaps (y bigger_ish 15) {
                    gossip.spill_tea("both conditions true")...
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "both conditions true\n");
}

TEST_F(InterpreterTests, IfWithVariableReassignment) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash counter about 5...
            perhaps (counter tiny_ish 10) {
                counter might_be 15...
                gossip.spill_tea(counter)...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "15\n");
}

TEST_F(InterpreterTests, CompleteIfStatementTest) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            perhaps (totally) {
                gossip.spill_tea("Test 1")...
            }
            
            perhaps (nah) {
                gossip.spill_tea("FAILED Test 2")...
            }
            
            perhaps (totally) {
                gossip.spill_tea("Test 3 if branch")...
            }
            screw_it {
                gossip.spill_tea("FAILED Test 3")...
            }
            
            perhaps (nah) {
                gossip.spill_tea("FAILED Test 4")...
            }
            screw_it {
                gossip.spill_tea("Test 4 else branch")...
            }
            
            stash value about 15...
            perhaps (value tiny_ish 10) {
                gossip.spill_tea("FAILED Test 5")...
            }
            or_whatever (value looks_like 15) {
                gossip.spill_tea("Test 5 - Value is exactly 15")...
            }
            screw_it {
                gossip.spill_tea("FAILED Test 5")...
            }
            
            yeet ghosted...
        }
    )";
    
    expectOutput(source, 
        "Test 1\nTest 3 if branch\n"
        "Test 4 else branch\nTest 5 - Value is exactly 15\n");
}

TEST_F(InterpreterTests, SpinAroundLoop) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash n about 5...
            spin_around (n) {
                gossip.spill_tea("Spinnin")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "Spinnin\nSpinnin\nSpinnin\nSpinnin\nSpinnin\n");
}

TEST_F(InterpreterTests, DoUntilBoredWithRageQuit) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            do_until_bored {
                gossip.spill_tea("Doing until bored")...
                rage_quit!!!
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "Doing until bored\n");
}

TEST_F(InterpreterTests, LoopWithVariableReassignmentAndBreak) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash counter about 0...
            stash number about 0...
            do_until_bored {
                gossip.spill_tea(counter)...
                gossip.spill_tea(number)...
                counter might_be counter with 1...
                number might_be number without 1...
                perhaps (counter bigger_ish 9) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, 
        "0\n0\n"
        "1\n-1\n"
        "2\n-2\n"
        "3\n-3\n"
        "4\n-4\n"
        "5\n-5\n"
        "6\n-6\n"
        "7\n-7\n"
        "8\n-8\n"
        "9\n-9\n");
}

TEST_F(InterpreterTests, SpinAroundWithVariable) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash n about 5...
            spin_around (n) {
                gossip.spill_tea("Spinnin")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "Spinnin\nSpinnin\nSpinnin\nSpinnin\nSpinnin\n");
}

TEST_F(InterpreterTests, EndlessLoopWithImmediateBreak) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            do_until_bored {
                gossip.spill_tea("Doing until bored")...
                rage_quit!!!
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "Doing until bored\n");
}

TEST_F(InterpreterTests, SpinAroundWithZeroIterations) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash n about 0...
            spin_around (n) {
                gossip.spill_tea("should not print")...
            }
            gossip.spill_tea("after loop")...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "after loop\n");
}

TEST_F(InterpreterTests, SpinAroundWithLiteralCount) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            spin_around (3) {
                gossip.spill_tea("iteration")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "iteration\niteration\niteration\n");
}

TEST_F(InterpreterTests, NestedLoops) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            spin_around (2) {
                spin_around (3) {
                    gossip.spill_tea("nested")...
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "nested\nnested\nnested\nnested\nnested\nnested\n");
}

TEST_F(InterpreterTests, DoUntilBoredWithConditionalBreak) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash count about 0...
            do_until_bored {
                gossip.spill_tea(count)...
                count might_be count with 1...
                perhaps (count bigger_ish 4) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "0\n1\n2\n3\n4\n");
}

TEST_F(InterpreterTests, IfElseIfMultipleConditions) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash score about 85...
            perhaps (score bigger_ish 90) {
                gossip.spill_tea("A")...
            }
            or_whatever (score bigger_ish 80) {
                gossip.spill_tea("B")...
            }
            or_whatever (score bigger_ish 70) {
                gossip.spill_tea("C")...
            }
            screw_it {
                gossip.spill_tea("F")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "B\n");
}

TEST_F(InterpreterTests, IfElseIfFallsThrough) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash value about 5...
            perhaps (value bigger_ish 10) {
                gossip.spill_tea("greater than 10")...
            }
            or_whatever (value bigger_ish 8) {
                gossip.spill_tea("greater than 8")...
            }
            screw_it {
                gossip.spill_tea("other")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "other\n");
}

TEST_F(InterpreterTests, NestedIfInLoop) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash i about 0...
            spin_around (3) {
                perhaps (i looks_like 1) {
                    gossip.spill_tea("found one")...
                }
                i might_be i with 1...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "found one\n");
}

TEST_F(InterpreterTests, LoopWithMultipleBreaks) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash x about 0...
            do_until_bored {
                x might_be x with 1...
                perhaps (x looks_like 3) {
                    rage_quit!!!
                }
                gossip.spill_tea(x)...
                perhaps (x bigger_ish 5) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "1\n2\n");
}

TEST_F(InterpreterTests, VariableReassignmentWithArithmetic) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash value about 10...
            gossip.spill_tea(value)...
            value might_be value with 5...
            gossip.spill_tea(value)...
            value might_be value without 3...
            gossip.spill_tea(value)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "10\n15\n12\n");
}

TEST_F(InterpreterTests, EvaluatesMultiplyTimes) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash result about 6 times 7...
            gossip.spill_tea(result)...
        }
    )";

    expectOutput(source, "42\n");
}

TEST_F(InterpreterTests, EvaluatesDivideOver) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash result about 8 over 4...
            gossip.spill_tea(result)...
        }
    )";

    expectOutput(source, "2\n");
}

TEST_F(InterpreterTests, EvaluatesMixedPlusMinusMultiplyDividePrecedence) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash x about 2...
            stash y about 2...
            stash result about 1 with x times 3 without 4 over y...
            gossip.spill_tea(result)...
        }
    )";

    expectOutput(source, "5\n");
}

TEST_F(InterpreterTests, EvaluatesParenthesizedMixedArithmetic) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash x about 2...
            stash y about 2...
            stash result about (1 with x) times (3 without 1) over y...
            gossip.spill_tea(result)...
        }
    )";

    expectOutput(source, "3\n");
}

TEST_F(InterpreterTests, OperatorKeywordsDoNotSplitIdentifiers) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash timesTwo about 1...
            stash overload about 2...
            stash withheld about 3...
            stash withoutX about 4...
            gossip.spill_tea(timesTwo)...
            gossip.spill_tea(overload)...
            gossip.spill_tea(withheld)...
            gossip.spill_tea(withoutX)...
        }
    )";

    expectOutput(source, "1\n2\n3\n4\n");
}

TEST_F(InterpreterTests, Precedence_MultiplyBindsTighterThanMinus) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash result about 0 without 5 times 2...
            gossip.spill_tea(result)...
        }
    )";

    expectOutput(source, "-10\n");
}

TEST_F(InterpreterTests, Associativity_DivideIsLeftAssociative) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash result about 8 over 4 over 2...
            gossip.spill_tea(result)...
        }
    )";

    expectOutput(source, "1\n");
}

TEST_F(InterpreterTests, ComparisonWithNegativeNumbers) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash negative about 0 without 5...
            gossip.spill_tea(negative)...
            perhaps (negative tiny_ish 0) {
                gossip.spill_tea("negative is less than zero")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "-5\nnegative is less than zero\n");
}

TEST_F(InterpreterTests, BooleanVariableInCondition) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash isReady about totally...
            perhaps (isReady looks_like totally) {
                gossip.spill_tea("ready")...
            }
            isReady might_be nah...
            perhaps (isReady looks_like nah) {
                gossip.spill_tea("not ready")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "ready\nnot ready\n");
}

TEST_F(InterpreterTests, SpinAroundWithVariableModification) {
    auto source = R"(
        summon gossip...
        
        gig macho() {
            stash counter about 0...
            spin_around (4) {
                counter might_be counter with 2...
                gossip.spill_tea(counter)...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "2\n4\n6\n8\n");
}

TEST_F(InterpreterTests, FunctionCallWithReturnValue) {
    auto source = R"(
        summon gossip...
        
        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            stash number about add(2, 3)...
            gossip.spill_tea(number)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "5\n");
}

TEST_F(InterpreterTests, FunctionCallWithNullArgumentPrintsGhosted) {
    auto source = R"(
        summon gossip...

        gig printArg(x) {
            gossip.spill_tea(x)...
        }

        gig macho() {
            printArg(ghosted)...
        }
    )";

    expectOutput(source, "ghosted\n");
}

TEST_F(InterpreterTests, Failure_PumpItOnNonVariableThrowsLogicRuntimeError) {
    auto source = R"(
        gig macho() {
            pump_it 1...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::Logic, "Can't pump_it into the void!");
}

TEST_F(InterpreterTests, Failure_IfConditionMustBeBool) {
    auto source = R"(
        gig macho() {
            perhaps (1) {
                yeet ghosted...
            }
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::TypeMismatch, "Anticipated vibe instead of solid");
}

TEST_F(InterpreterTests, Failure_FunctionArgumentCountMismatch) {
    auto source = R"(
        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            add(1)...
        }
    )";

    expectRuntimeErrorMsgContains(source, RuntimeError::Type::OutOfBounds, "Argument count don't vibe");
}

TEST_F(InterpreterTests, Failure_CallingNonFunctionThrowsRuntimeError) {
    auto source = R"(
        gig macho() {
            stash x about 1...
            x()...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::TypeMismatch, "Anticipated gig instead of solid");
}

TEST_F(InterpreterTests, Failure_DotIntoNonDotThrowsRuntimeError) {
    auto source = R"(
        gig macho() {
            stash x about 1...
            x.anything()...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::TypeMismatch, "Can't dot into that");
}

TEST_F(InterpreterTests, Failure_TypeDoesNotMatchTriggersCppException) {
    auto source = R"(
        gig macho() {
            spin_around ("not_an_int") {
                yeet ghosted...
            }
        }

    )";

    expectRuntimeError(source, RuntimeError::Type::TypeMismatch, "Anticipated solid instead of yap");
}

TEST_F(InterpreterTests, FunctionCallWithNullArgumentWorksButMathFails) {
    auto source = R"(
        gig doSth(x) {
            yeet 2 with x...
        }

        gig macho() {
            doSth(ghosted)...
        }
    )";

    expectRuntimeError(source, RuntimeError::Type::Math, "Math is only mathing on numbers");
}

TEST_F(InterpreterTests, MultipleFunctionCalls) {
    auto source = R"(
        summon gossip...

        gig multiply(a, b) {
            yeet a with a with b...
        }

        gig macho() {
            stash result1 about multiply(3, 2)...
            gossip.spill_tea(result1)...
            stash result2 about multiply(4, 5)...
            gossip.spill_tea(result2)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "8\n13\n");
}

TEST_F(InterpreterTests, FunctionWithSubtraction) {
    auto source = R"(
        summon gossip...

        gig subtract(x, y) {
            yeet x without y...
        }

        gig macho() {
            stash result about subtract(10, 3)...
            gossip.spill_tea(result)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "7\n");
}

TEST_F(InterpreterTests, NestedFunctionCalls) {
    auto source = R"(
        summon gossip...

        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            stash result about add(add(1, 2), add(3, 4))...
            gossip.spill_tea(result)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "10\n");
}

TEST_F(InterpreterTests, FunctionCallInLoop) {
    auto source = R"(
        summon gossip...

        gig double(x) {
            yeet x with x...
        }

        gig macho() {
            stash i about 1...
            spin_around (3) {
                stash doubled about double(i)...
                gossip.spill_tea(doubled)...
                i might_be i with 1...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "2\n4\n6\n");
}

TEST_F(InterpreterTests, FunctionReturningBoolean) {
    auto source = R"(
        summon gossip...

        gig isGreater(x, y) {
            yeet x bigger_ish y...
        }

        gig macho() {
            stash result about isGreater(5, 3)...
            gossip.spill_tea(result)...
            stash result2 about isGreater(2, 8)...
            gossip.spill_tea(result2)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "totally\nnah\n");
}

TEST_F(InterpreterTests, FunctionCallInCondition) {
    auto source = R"(
        summon gossip...

        gig sum(a, b) {
            yeet a with b...
        }

        gig macho() {
            stash total about sum(2, 3)...
            perhaps (total looks_like 5) {
                gossip.spill_tea("correct sum")...
            }
            screw_it {
                gossip.spill_tea("wrong sum")...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "correct sum\n");
}

TEST_F(InterpreterTests, PumpItIncrementOperator) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash number about 0...
            gossip.spill_tea(number)...
            pump_it number...
            gossip.spill_tea(number)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "0\n1\n");
}

TEST_F(InterpreterTests, DoUntilBoredLoopWithPumpItAndRageQuit) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash count about 0...
            do_until_bored {
                gossip.spill_tea(count)...
                pump_it count...
                perhaps (count bigger_ish 9) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n");
}

TEST_F(InterpreterTests, BreakInsideBlockDoesNotLeakBlockScope) {
    auto source = R"(
        summon gossip...

        gig macho() {
            do_until_bored {
                stash x about 123...
                rage_quit!!!
            }
            gossip.spill_tea(x)...
        }
    )";

    expectRuntimeErrorMsgContains(
        source,
        RuntimeError::Type::Undefined,
        "Variable x does not exist");
}

TEST_F(InterpreterTests, VoidFunctionCall) {
    auto source = R"(
        summon gossip...

        gig print(x) {
            gossip.spill_tea(x)...
        }

        gig macho() {
            stash string about "hello"...
            print(string)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "hello\n");
}

TEST_F(InterpreterTests, GossipModulePrint) {
    auto source = R"(
        summon gossip...

        gig macho() {
            gossip.spill_tea("Hello")...
        }
    )";
    
    expectOutput(source, "Hello\n");
}

// TODO move String tests somewhere else

TEST_F(InterpreterTests, StringToSolidConvertsStringToInt) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash string about "11"...
            gossip.spill_tea(string.to_solid())...
        }
    )";

    expectOutput(source, "11\n");
}

// TODO move Vector tests somewhere else

TEST_F(InterpreterTests, GossipPrintsVectorWithVariableElement) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash x about 22...
            stash list about [11, x, 33]...
            gossip.spill_tea(list)...
        }
    )";

    expectOutput(source, "[11, 22, 33]\n");
}

TEST_F(InterpreterTests, VectorPatchCorrectlySetsElements) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector)...
            vector.patch(1, 99)...
            gossip.spill_tea(vector)...
            vector.patch(3, 0)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source,
        "[11, 22, 33]\n"
        "[99, 22, 33]\n"
        "[99, 22, 0]\n");
}

TEST_F(InterpreterTests, VectorYoinkReturnsElementAtOneBasedIndex) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector.yoink(2))...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source,
        "22\n"
        "[11, 22, 33]\n");
}

TEST_F(InterpreterTests, VectorPatchUpdatesElement) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.patch(1, 99)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[99, 22, 33]\n");
}

TEST_F(InterpreterTests, VectorShoveAddsElementAtEnd) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.shove(44)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[11, 22, 33, 44]\n");
}

TEST_F(InterpreterTests, VectorKickRemovesAndReturnsLastElement) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector.kick())...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "33\n[11, 22]\n");
}

TEST_F(InterpreterTests, VectorVibeCheckReturnsWhetherEmpty) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11]...
            gossip.spill_tea(vector.vibe_check())...
            vector.reset_the_vibe()...
            gossip.spill_tea(vector.vibe_check())...
        }
    )";

    expectOutput(source, "nah\ntotally\n");
}

TEST_F(InterpreterTests, VectorVibeCountReturnsCurrentSize) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            gossip.spill_tea(vector.vibe_count())...
            vector.kick()...
            gossip.spill_tea(vector.vibe_count())...
        }
    )";

    expectOutput(source, "3\n2\n");
}

TEST_F(InterpreterTests, VectorResetTheVibeClearsAllElements) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.reset_the_vibe()...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[]\n");
}

TEST_F(InterpreterTests, VectorResetTheVibeResetsVectorType) {
    auto source = R"(
        summon gossip...

        gig macho() {
            stash vector about [11, 22, 33]...
            vector.reset_the_vibe()...
            psst: Int -> Float
            vector.shove(11.11)...
            gossip.spill_tea(vector)...
        }
    )";

    expectOutput(source, "[11.11]\n");
}

TEST_F(InterpreterTests, ScopedImportModulesTest) {
    // TODO explicitly check error
    auto source = R"(
        gig invalidPrint() {
            psst: summon gossip...
            gossip.spill_tea("Man I'm dead")...
        }

        gig macho() {
            summon gossip...

            gossip.spill_tea("Hello")...

            invalidPrint()...
        }
    )";
    EXPECT_ANY_THROW(expectOutput(source, "Hello\n"));
}
