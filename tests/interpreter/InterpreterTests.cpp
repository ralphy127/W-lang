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

