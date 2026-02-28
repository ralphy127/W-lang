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

TEST_F(ParserTestFixture, ParseFunctionWithReturnIntStatement) {
    auto parserResult = parseSource(
        "gig macho() {\n"
        "    yeet 0...\n"
        "}\n");

    ASSERT_EQ(parserResult.statements.size(), 1);
    
    auto* funcStmt = dynamic_cast<FunctionStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(funcStmt->getName().getValue<std::string>(), "macho");
    EXPECT_EQ(funcStmt->getParameters().size(), 0);
    
    const auto& blockStmt = dynamic_cast<const BlockStmt&>(funcStmt->getBody());
    ASSERT_EQ(blockStmt.getStatements().size(), 1);
    
    auto* returnStmt = dynamic_cast<ReturnStmt*>(blockStmt.getStatements()[0].get());
    const auto& returnValue = dynamic_cast<const LiteralExpr&>(returnStmt->getValue());
    const auto& returnToken = returnValue.getLiteral();
    ASSERT_EQ(returnToken.getType(), Token::Type::Int);
    EXPECT_EQ(returnToken.getValue<std::int32_t>(), 0);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithInt) {
    auto parserResult = parseSource(
        "stash integer about 1...");
    
    ASSERT_EQ(parserResult.statements.size(), 1);

    auto* varStmt = dynamic_cast<VarDefinitionStmt*>(parserResult.statements[0].get());

    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "integer");

    const auto& initializer = dynamic_cast<const LiteralExpr&>(varStmt->getInitializer());
    const auto& literal = initializer.getLiteral();
    EXPECT_EQ(literal.getType(), Token::Type::Int);
    EXPECT_EQ(literal.getValue<std::int32_t>(), 1);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithFloat) {
    auto parserResult = parseSource("stash float about 1.0...");
    
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    auto* varStmt = dynamic_cast<VarDefinitionStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "float");
    
    const auto& initializer = dynamic_cast<const LiteralExpr&>(varStmt->getInitializer());
    const auto& literal = initializer.getLiteral();
    
    EXPECT_EQ(literal.getType(), Token::Type::Float);
    EXPECT_FLOAT_EQ(literal.getValue<double>(), 1.0);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithEqualityExpression) {
    auto parserResult = parseSource("stash comparison about 10 looks_like 10...");
    
    ASSERT_EQ(parserResult.errors.size(), 0);
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "comparison");
    
    const auto& initializer = dynamic_cast<const BinaryExpr&>(varStmt->getInitializer());
    EXPECT_EQ(initializer.getOperator().getType(), Token::Type::Equal);
    
    const auto& left = dynamic_cast<const LiteralExpr&>(initializer.getLeft());
    
    const auto& leftToken = left.getLiteral();
    EXPECT_EQ(leftToken.getType(), Token::Type::Int);
    EXPECT_EQ(leftToken.getValue<std::int32_t>(), 10);
    
    const auto& right = dynamic_cast<const LiteralExpr&>(initializer.getRight());
    
    const auto& rightToken = right.getLiteral();
    EXPECT_EQ(rightToken.getType(), Token::Type::Int);
    EXPECT_EQ(rightToken.getValue<std::int32_t>(), 10);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithNotEqualExpression) {
    auto parserResult = parseSource("stash comparison about 10 kinda_sus 5...");
    
    ASSERT_EQ(parserResult.errors.size(), 0);
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "comparison");
    
    const auto& initializer = dynamic_cast<const BinaryExpr&>(varStmt->getInitializer());
    EXPECT_EQ(initializer.getOperator().getType(), Token::Type::NotEqual);
    
    const auto& left = dynamic_cast<const LiteralExpr&>(initializer.getLeft());
    const auto& leftToken = left.getLiteral();
    EXPECT_EQ(leftToken.getType(), Token::Type::Int);
    EXPECT_EQ(leftToken.getValue<std::int32_t>(), 10);
    
    const auto& right = dynamic_cast<const LiteralExpr&>(initializer.getRight());
    const auto& rightToken = right.getLiteral();
    EXPECT_EQ(rightToken.getType(), Token::Type::Int);
    EXPECT_EQ(rightToken.getValue<std::int32_t>(), 5);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithGreaterExpression) {
    auto parserResult = parseSource("stash comparison about 20 bigger_ish 10...");
    
    ASSERT_EQ(parserResult.errors.size(), 0);
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "comparison");
    
    const auto& initializer = dynamic_cast<const BinaryExpr&>(varStmt->getInitializer());
    EXPECT_EQ(initializer.getOperator().getType(), Token::Type::Greater);
    
    const auto& left = dynamic_cast<const LiteralExpr&>(initializer.getLeft());
    const auto& leftToken = left.getLiteral();
    EXPECT_EQ(leftToken.getType(), Token::Type::Int);
    EXPECT_EQ(leftToken.getValue<std::int32_t>(), 20);
    
    const auto& right = dynamic_cast<const LiteralExpr&>(initializer.getRight());
    const auto& rightToken = right.getLiteral();
    EXPECT_EQ(rightToken.getType(), Token::Type::Int);
    EXPECT_EQ(rightToken.getValue<std::int32_t>(), 10);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithLessExpression) {
    auto parserResult = parseSource("stash comparison about 5 tiny_ish 10...");
    
    ASSERT_EQ(parserResult.errors.size(), 0);
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "comparison");
    
    const auto& initializer = dynamic_cast<const BinaryExpr&>(varStmt->getInitializer());
    EXPECT_EQ(initializer.getOperator().getType(), Token::Type::Less);
    
    const auto& left = dynamic_cast<const LiteralExpr&>(initializer.getLeft());
    const auto& leftToken = left.getLiteral();
    EXPECT_EQ(leftToken.getType(), Token::Type::Int);
    EXPECT_EQ(leftToken.getValue<std::int32_t>(), 5);
    
    const auto& right = dynamic_cast<const LiteralExpr&>(initializer.getRight());
    const auto& rightToken = right.getLiteral();
    EXPECT_EQ(rightToken.getType(), Token::Type::Int);
    EXPECT_EQ(rightToken.getValue<std::int32_t>(), 10);
}

TEST_F(ParserTestFixture, ParsePrecedenceEqualityAndAddition) {
    auto parserResult = parseSource("stash result about 2 with 2 looks_like 4...");
    
    ASSERT_EQ(parserResult.errors.size(), 0);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "result");
    
    const auto& equalityExpr = dynamic_cast<const BinaryExpr&>(varStmt->getInitializer());
    EXPECT_EQ(equalityExpr.getOperator().getType(), Token::Type::Equal); 
    
    const auto& additionExpr = dynamic_cast<const BinaryExpr&>(equalityExpr.getLeft());
    EXPECT_EQ(additionExpr.getOperator().getType(), Token::Type::Plus);
    
    const auto& addLeft = dynamic_cast<const LiteralExpr&>(additionExpr.getLeft());
    EXPECT_EQ(addLeft.getLiteral().getValue<std::int32_t>(), 2);
    
    const auto& addRight = dynamic_cast<const LiteralExpr&>(additionExpr.getRight());
    EXPECT_EQ(addRight.getLiteral().getValue<std::int32_t>(), 2);
    
    const auto& equalityRight = dynamic_cast<const LiteralExpr&>(equalityExpr.getRight());
    EXPECT_EQ(equalityRight.getLiteral().getValue<std::int32_t>(), 4);
}