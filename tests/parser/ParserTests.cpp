#include <gtest/gtest.h>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

class ParserTestFixture : public ::testing::Test {
protected:
    ParserResult parseSource(const std::string& source) {
        Lexer lexer{source};
        auto lexerResult = lexer.tokenize();
        EXPECT_TRUE(lexerResult.errors.empty());
        
        Parser parser{std::move(lexerResult.tokens)};
        return parser.parse();
    }
};

TEST_F(ParserTestFixture, ParseFunctionWithReturnStatement) {
    auto parserResult = parseSource(
        "gig macho() {\n"
        "    yeet 0...\n"
        "}\n");

    ASSERT_EQ(parserResult.statements.size(), 1);
    
    auto* funcStmt = dynamic_cast<FunctionStmt*>(parserResult.statements[0].get());
    ASSERT_NE(funcStmt, nullptr);
    
    auto name = funcStmt->getName().getValue<std::string>();
    EXPECT_EQ(name.erase(name.find_first_of('\0')), "macho");
    EXPECT_EQ(funcStmt->getParameters().size(), 0);
    
    const auto& blockStmt = dynamic_cast<const BlockStmt&>(funcStmt->getBody());
    ASSERT_EQ(blockStmt.getStatements().size(), 1);
    
    auto* returnStmt = dynamic_cast<ReturnStmt*>(blockStmt.getStatements()[0].get());
    ASSERT_NE(returnStmt, nullptr);
    EXPECT_TRUE(returnStmt->hasValue());
    const auto& returnValue = dynamic_cast<const LiteralExpr&>(returnStmt->getValue());
    const auto& returnToken = returnValue.getLiteral();
    ASSERT_EQ(returnToken.getType(), Token::Type::Int);
    ASSERT_TRUE(returnToken.valueIs<std::int32_t>());
    EXPECT_EQ(returnToken.getValue<std::int32_t>(), 0);
}
