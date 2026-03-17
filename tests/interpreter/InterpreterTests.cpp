#include <gtest/gtest.h>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "interpreter/Interpreter.hpp"

struct InterpreterTests : ::testing::Test {
    ParserResult parseSource(const std::string& source) {
        Lexer lexer{source};
        auto lexerResult = lexer.tokenize();
        EXPECT_TRUE(lexerResult.errors.empty());
        
        Parser parser{std::move(lexerResult.tokens)};
        return parser.parse();
    }
    
    std::string executeAndCaptureOutput(const std::string& source) {
        auto statements = parseSource(source).statements;
        
        std::stringstream buffer{};
        auto* oldCout = std::cout.rdbuf(buffer.rdbuf());
        
        Interpreter interpreter{std::move(statements)};
        interpreter.interpret();
        
        std::cout.rdbuf(oldCout);
        
        return buffer.str();
    }
    
    void expectOutput(const std::string& source, const std::string& expectedOutput) {
        EXPECT_EQ(executeAndCaptureOutput(source), expectedOutput);
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
    
    expectOutput(source, "Hello\n");
}
