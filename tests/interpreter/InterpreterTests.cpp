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
};

TEST_F(InterpreterTests, MvpExecutesMachoFunction) {
    auto source = R"(
        gig macho() {
            scream: "F*ck me it works"...
            yeet ghosted...
        }
    )";
    
    auto statements = parseSource(source).statements;
    
    std::stringstream buffer;
    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
    
    Interpreter interpreter{std::move(statements)};
    interpreter.interpret();
    
    std::cout.rdbuf(oldCout);
    
    EXPECT_EQ(buffer.str(), "F*CK ME IT WORKS!!!\n");
}