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
        gig macho() {
            scream: "F*ck me it works"...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "F*CK ME IT WORKS!!!\n");
}

TEST_F(InterpreterTests, MvpExecutesVariableStashAndScream) {
    auto source = R"(
        gig macho() {
            stash number about 10...
            scream: number...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "10!!!\n");
}

TEST_F(InterpreterTests, OutputsCorrectBooleanValues) {
    auto source = R"(
        gig macho() {
            stash equal about 1 looks_like 1...
            scream: equal...
            stash notEqual about 1 looks_like 2...
            scream: notEqual...
            stash smaller about 1 tiny_ish 2...
            scream: smaller...
            stash notSmaller about 2 tiny_ish 1...
            scream: notSmaller...
            stash greater about 2 bigger_ish 1...
            scream: greater...
            stash notGreater about 1 bigger_ish 2...
            scream: notGreater...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "TOTALLY!!!\nNAH!!!\nTOTALLY!!!\nNAH!!!\nTOTALLY!!!\nNAH!!!\n");
}

TEST_F(InterpreterTests, SimpleIfWithTrueCondition) {
    auto source = R"(
        gig macho() {
            perhaps (totally) {
                scream: "if executed"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "IF EXECUTED!!!\n");
}

TEST_F(InterpreterTests, SimpleIfWithFalseCondition) {
    auto source = R"(
        gig macho() {
            perhaps (nah) {
                scream: "should not print"...
            }
            scream: "after if"...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "AFTER IF!!!\n");
}

TEST_F(InterpreterTests, IfElseWithTrueCondition) {
    auto source = R"(
        gig macho() {
            perhaps (totally) {
                scream: "if branch"...
            }
            screw_it {
                scream: "else branch"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "IF BRANCH!!!\n");
}

TEST_F(InterpreterTests, IfElseWithFalseCondition) {
    auto source = R"(
        gig macho() {
            perhaps (nah) {
                scream: "if branch"...
            }
            screw_it {
                scream: "else branch"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "ELSE BRANCH!!!\n");
}

TEST_F(InterpreterTests, IfElseIfChainMatchesSecondCondition) {
    auto source = R"(
        gig macho() {
            stash value about 15...
            perhaps (value tiny_ish 10) {
                scream: "less than 10"...
            }
            or_whatever (value looks_like 15) {
                scream: "equals 15"...
            }
            screw_it {
                scream: "greater than 15"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "EQUALS 15!!!\n");
}

TEST_F(InterpreterTests, IfWithComparisonOperators) {
    auto source = R"(
        gig macho() {
            stash x about 10...
            perhaps (x bigger_ish 5) {
                scream: "greater than 5"...
            }
            perhaps (x tiny_ish 20) {
                scream: "less than 20"...
            }
            perhaps (x looks_like 10) {
                scream: "equals 10"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "GREATER THAN 5!!!\nLESS THAN 20!!!\nEQUALS 10!!!\n");
}

TEST_F(InterpreterTests, NestedIfStatements) {
    auto source = R"(
        gig macho() {
            stash x about 10...
            stash y about 20...
            perhaps (x tiny_ish 15) {
                perhaps (y bigger_ish 15) {
                    scream: "both conditions true"...
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "BOTH CONDITIONS TRUE!!!\n");
}

TEST_F(InterpreterTests, IfWithVariableReassignment) {
    auto source = R"(
        gig macho() {
            stash counter about 5...
            perhaps (counter tiny_ish 10) {
                counter might_be 15...
                scream: counter...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "15!!!\n");
}

TEST_F(InterpreterTests, CompleteIfStatementTest) {
    auto source = R"(
        gig macho() {
            perhaps (totally) {
                scream: "Test 1"...
            }
            
            perhaps (nah) {
                scream: "FAILED Test 2"...
            }
            
            perhaps (totally) {
                scream: "Test 3 if branch"...
            }
            screw_it {
                scream: "FAILED Test 3"...
            }
            
            perhaps (nah) {
                scream: "FAILED Test 4"...
            }
            screw_it {
                scream: "Test 4 else branch"...
            }
            
            stash value about 15...
            perhaps (value tiny_ish 10) {
                scream: "FAILED Test 5"...
            }
            or_whatever (value looks_like 15) {
                scream: "Test 5 - Value is exactly 15"...
            }
            screw_it {
                scream: "FAILED Test 5"...
            }
            
            yeet ghosted...
        }
    )";
    
    expectOutput(source, 
        "TEST 1!!!\nTEST 3 IF BRANCH!!!\n"
        "TEST 4 ELSE BRANCH!!!\nTEST 5 - VALUE IS EXACTLY 15!!!\n");
}

TEST_F(InterpreterTests, SpinAroundLoop) {
    auto source = R"(
        gig macho() {
            stash n about 5...
            spin_around (n) {
                scream: "Spinnin"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "SPINNIN!!!\nSPINNIN!!!\nSPINNIN!!!\nSPINNIN!!!\nSPINNIN!!!\n");
}

TEST_F(InterpreterTests, DoUntilBoredWithRageQuit) {
    auto source = R"(
        gig macho() {
            do_until_bored {
                scream: "Doing until bored"...
                rage_quit!!!
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "DOING UNTIL BORED!!!\n");
}

TEST_F(InterpreterTests, LoopWithVariableReassignmentAndBreak) {
    auto source = R"(
        gig macho() {
            stash counter about 0...
            stash number about 0...
            do_until_bored {
                scream: counter...
                scream: number...
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
        "0!!!\n0!!!\n"
        "1!!!\n-1!!!\n"
        "2!!!\n-2!!!\n"
        "3!!!\n-3!!!\n"
        "4!!!\n-4!!!\n"
        "5!!!\n-5!!!\n"
        "6!!!\n-6!!!\n"
        "7!!!\n-7!!!\n"
        "8!!!\n-8!!!\n"
        "9!!!\n-9!!!\n");
}

TEST_F(InterpreterTests, SpinAroundWithVariable) {
    auto source = R"(
        gig macho() {
            stash n about 5...
            spin_around (n) {
                scream: "Spinnin"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "SPINNIN!!!\nSPINNIN!!!\nSPINNIN!!!\nSPINNIN!!!\nSPINNIN!!!\n");
}

TEST_F(InterpreterTests, EndlessLoopWithImmediateBreak) {
    auto source = R"(
        gig macho() {
            do_until_bored {
                scream: "Doing until bored"...
                rage_quit!!!
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "DOING UNTIL BORED!!!\n");
}

TEST_F(InterpreterTests, SpinAroundWithZeroIterations) {
    auto source = R"(
        gig macho() {
            stash n about 0...
            spin_around (n) {
                scream: "should not print"...
            }
            scream: "after loop"...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "AFTER LOOP!!!\n");
}

TEST_F(InterpreterTests, SpinAroundWithLiteralCount) {
    auto source = R"(
        gig macho() {
            spin_around (3) {
                scream: "iteration"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "ITERATION!!!\nITERATION!!!\nITERATION!!!\n");
}

TEST_F(InterpreterTests, NestedLoops) {
    auto source = R"(
        gig macho() {
            spin_around (2) {
                spin_around (3) {
                    scream: "nested"...
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "NESTED!!!\nNESTED!!!\nNESTED!!!\nNESTED!!!\nNESTED!!!\nNESTED!!!\n");
}

TEST_F(InterpreterTests, DoUntilBoredWithConditionalBreak) {
    auto source = R"(
        gig macho() {
            stash count about 0...
            do_until_bored {
                scream: count...
                count might_be count with 1...
                perhaps (count bigger_ish 4) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "0!!!\n1!!!\n2!!!\n3!!!\n4!!!\n");
}

TEST_F(InterpreterTests, IfElseIfMultipleConditions) {
    auto source = R"(
        gig macho() {
            stash score about 85...
            perhaps (score bigger_ish 90) {
                scream: "A"...
            }
            or_whatever (score bigger_ish 80) {
                scream: "B"...
            }
            or_whatever (score bigger_ish 70) {
                scream: "C"...
            }
            screw_it {
                scream: "F"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "B!!!\n");
}

TEST_F(InterpreterTests, IfElseIfFallsThrough) {
    auto source = R"(
        gig macho() {
            stash value about 5...
            perhaps (value bigger_ish 10) {
                scream: "greater than 10"...
            }
            or_whatever (value bigger_ish 8) {
                scream: "greater than 8"...
            }
            screw_it {
                scream: "other"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "OTHER!!!\n");
}

TEST_F(InterpreterTests, NestedIfInLoop) {
    auto source = R"(
        gig macho() {
            stash i about 0...
            spin_around (3) {
                perhaps (i looks_like 1) {
                    scream: "found one"...
                }
                i might_be i with 1...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "FOUND ONE!!!\n");
}

TEST_F(InterpreterTests, LoopWithMultipleBreaks) {
    auto source = R"(
        gig macho() {
            stash x about 0...
            do_until_bored {
                x might_be x with 1...
                perhaps (x looks_like 3) {
                    rage_quit!!!
                }
                scream: x...
                perhaps (x bigger_ish 5) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "1!!!\n2!!!\n");
}

TEST_F(InterpreterTests, VariableReassignmentWithArithmetic) {
    auto source = R"(
        gig macho() {
            stash value about 10...
            scream: value...
            value might_be value with 5...
            scream: value...
            value might_be value without 3...
            scream: value...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "10!!!\n15!!!\n12!!!\n");
}

TEST_F(InterpreterTests, ComparisonWithNegativeNumbers) {
    auto source = R"(
        gig macho() {
            stash negative about 0 without 5...
            scream: negative...
            perhaps (negative tiny_ish 0) {
                scream: "negative is less than zero"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "-5!!!\nNEGATIVE IS LESS THAN ZERO!!!\n");
}

TEST_F(InterpreterTests, BooleanVariableInCondition) {
    auto source = R"(
        gig macho() {
            stash isReady about totally...
            perhaps (isReady looks_like totally) {
                scream: "ready"...
            }
            isReady might_be nah...
            perhaps (isReady looks_like nah) {
                scream: "not ready"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "READY!!!\nNOT READY!!!\n");
}

TEST_F(InterpreterTests, SpinAroundWithVariableModification) {
    auto source = R"(
        gig macho() {
            stash counter about 0...
            spin_around (4) {
                counter might_be counter with 2...
                scream: counter...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "2!!!\n4!!!\n6!!!\n8!!!\n");
}

TEST_F(InterpreterTests, FunctionCallWithReturnValue) {
    auto source = R"(
        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            stash number about add(2, 3)...
            scream: number...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "5!!!\n");
}

TEST_F(InterpreterTests, MultipleFunctionCalls) {
    auto source = R"(
        gig multiply(a, b) {
            yeet a with a with b...
        }

        gig macho() {
            stash result1 about multiply(3, 2)...
            scream: result1...
            stash result2 about multiply(4, 5)...
            scream: result2...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "8!!!\n13!!!\n");
}

TEST_F(InterpreterTests, FunctionWithSubtraction) {
    auto source = R"(
        gig subtract(x, y) {
            yeet x without y...
        }

        gig macho() {
            stash result about subtract(10, 3)...
            scream: result...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "7!!!\n");
}

TEST_F(InterpreterTests, NestedFunctionCalls) {
    auto source = R"(
        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            stash result about add(add(1, 2), add(3, 4))...
            scream: result...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "10!!!\n");
}

TEST_F(InterpreterTests, FunctionCallInLoop) {
    auto source = R"(
        gig double(x) {
            yeet x with x...
        }

        gig macho() {
            stash i about 1...
            spin_around (3) {
                stash doubled about double(i)...
                scream: doubled...
                i might_be i with 1...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "2!!!\n4!!!\n6!!!\n");
}

TEST_F(InterpreterTests, FunctionReturningBoolean) {
    auto source = R"(
        gig isGreater(x, y) {
            yeet x bigger_ish y...
        }

        gig macho() {
            stash result about isGreater(5, 3)...
            scream: result...
            stash result2 about isGreater(2, 8)...
            scream: result2...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "TOTALLY!!!\nNAH!!!\n");
}

TEST_F(InterpreterTests, FunctionCallInCondition) {
    auto source = R"(
        gig sum(a, b) {
            yeet a with b...
        }

        gig macho() {
            stash total about sum(2, 3)...
            perhaps (total looks_like 5) {
                scream: "correct sum"...
            }
            screw_it {
                scream: "wrong sum"...
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "CORRECT SUM!!!\n");
}

TEST_F(InterpreterTests, PumpItIncrementOperator) {
    auto source = R"(
        gig macho() {
            stash number about 0...
            scream: number...
            pump_it number...
            scream: number...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "0!!!\n1!!!\n");
}

TEST_F(InterpreterTests, DoUntilBoredLoopWithPumpItAndRageQuit) {
    auto source = R"(
        gig macho() {
            stash count about 0...
            do_until_bored {
                scream: count...
                pump_it count...
                perhaps (count bigger_ish 9) {
                    rage_quit!!!
                }
            }
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "0!!!\n1!!!\n2!!!\n3!!!\n4!!!\n5!!!\n6!!!\n7!!!\n8!!!\n9!!!\n");
}

TEST_F(InterpreterTests, VoidFunctionCall) {
    auto source = R"(
        gig print(x) {
            scream: x...
        }

        gig macho() {
            stash string about "hello"...
            print(string)...
            yeet ghosted...
        }
    )";
    
    expectOutput(source, "HELLO!!!\n");
}


