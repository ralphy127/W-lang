#include <gtest/gtest.h>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"

struct ParserTestFixture : public ::testing::Test {
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

TEST_F(ParserTestFixture, ParseVarDefinitionWithTrue) {
    auto parserResult = parseSource("stash truth about totally...");
    
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    auto* varStmt = dynamic_cast<VarDefinitionStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "truth");
    
    const auto& initializer = dynamic_cast<const LiteralExpr&>(varStmt->getInitializer());
    const auto& literal = initializer.getLiteral();
    
    EXPECT_EQ(literal.getType(), Token::Type::True);
}

TEST_F(ParserTestFixture, ParseVarDefinitionWithFalse) {
    auto parserResult = parseSource("stash truth about nah...");
    
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    auto* varStmt = dynamic_cast<VarDefinitionStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "truth");
    
    const auto& initializer = dynamic_cast<const LiteralExpr&>(varStmt->getInitializer());
    const auto& literal = initializer.getLiteral();
    
    EXPECT_EQ(literal.getType(), Token::Type::False);
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

TEST_F(ParserTestFixture, ParseSimpleIfStatement) {
    auto parserResult = parseSource("perhaps (totally) { yeet 1... }");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* ifStmt = dynamic_cast<const IfStmt*>(parserResult.statements[0].get());
    
    const auto& conditionExpr = dynamic_cast<const LiteralExpr&>(ifStmt->getCondition());
    EXPECT_EQ(conditionExpr.getLiteral().getType(), Token::Type::True);
    
    const auto& thenBlock = dynamic_cast<const BlockStmt&>(ifStmt->getThenBlock());
    const auto& thenStatements = thenBlock.getStatements();
    ASSERT_EQ(thenStatements.size(), 1) << "Then block should have exactly one statement";
    
    const auto* returnStmt = dynamic_cast<const ReturnStmt*>(thenStatements[0].get());
    
    const auto& returnValue = dynamic_cast<const LiteralExpr&>(returnStmt->getValue());
    EXPECT_EQ(returnValue.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(returnValue.getLiteral().getValue<std::int32_t>(), 1);
    
    EXPECT_TRUE(ifStmt->getElseIfClauses().empty()) << "Elif list should be empty";
}

TEST_F(ParserTestFixture, ParseFullIfElseChain) {
    auto source = R"(
        perhaps (1) {
            yeet 1...
        }
        or_whatever (2) {
            yeet 2...
        }
        or_whatever (3) {
            yeet 3...
        }
        screw_it {
            yeet 4...
        }
    )";
    auto parserResult = parseSource(source);
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* ifStmt = dynamic_cast<const IfStmt*>(parserResult.statements[0].get());
    
    const auto& mainCond = dynamic_cast<const LiteralExpr&>(ifStmt->getCondition());
    EXPECT_EQ(mainCond.getLiteral().getValue<std::int32_t>(), 1);
    
    const auto& thenBlock = dynamic_cast<const BlockStmt&>(ifStmt->getThenBlock());
    ASSERT_EQ(thenBlock.getStatements().size(), 1);
    
    const auto& elifs = ifStmt->getElseIfClauses();
    ASSERT_EQ(elifs.size(), 2) << "Should have parsed exactly two or_whatever branches";
    
    const auto& elif1Cond = dynamic_cast<const LiteralExpr&>(*elifs[0].condition);
    EXPECT_EQ(elif1Cond.getLiteral().getValue<std::int32_t>(), 2);
    
    const auto* elif1Body = dynamic_cast<const BlockStmt*>(elifs[0].body.get());
    ASSERT_EQ(elif1Body->getStatements().size(), 1);
    
    const auto& elif2Cond = dynamic_cast<const LiteralExpr&>(*elifs[1].condition);
    EXPECT_EQ(elif2Cond.getLiteral().getValue<std::int32_t>(), 3);
    
    const auto* elif2Body = dynamic_cast<const BlockStmt*>(elifs[1].body.get());
    ASSERT_EQ(elif2Body->getStatements().size(), 1);
    
    const auto& elseBlock = dynamic_cast<const BlockStmt&>(ifStmt->getElseBlock());
    ASSERT_EQ(elseBlock.getStatements().size(), 1);
    
    const auto* elseReturnStmt = dynamic_cast<const ReturnStmt*>(elseBlock.getStatements()[0].get());
    
    const auto& elseReturnValue = dynamic_cast<const LiteralExpr&>(elseReturnStmt->getValue());
    EXPECT_EQ(elseReturnValue.getLiteral().getValue<std::int32_t>(), 4);
}

TEST_F(ParserTestFixture, ParseDoUntilBoredStatement) {
    auto parserResult = parseSource("do_until_bored { yeet 1... }");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);

    const auto* loopStmt = dynamic_cast<const LoopStmt*>(parserResult.statements[0].get());
    
    const auto& bodyBlock = dynamic_cast<const BlockStmt&>(loopStmt->getBody());
    const auto& bodyStatements = bodyBlock.getStatements();
    ASSERT_EQ(bodyStatements.size(), 1) << "Loop body should have exactly one statement";
    
    const auto* returnStmt = dynamic_cast<const ReturnStmt*>(bodyStatements[0].get());
    
    const auto& returnValue = dynamic_cast<const LiteralExpr&>(returnStmt->getValue());
    EXPECT_EQ(returnValue.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(returnValue.getLiteral().getValue<std::int32_t>(), 1);
}

TEST_F(ParserTestFixture, ParseSpinAroundStatement) {
    auto parserResult = parseSource("spin_around (5) { yeet 1... }");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* repeatStmt = dynamic_cast<const RepeatStmt*>(parserResult.statements[0].get());
    
    const auto& countExpr = dynamic_cast<const LiteralExpr&>(repeatStmt->getCount());
    EXPECT_EQ(countExpr.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(countExpr.getLiteral().getValue<std::int32_t>(), 5);
    
    const auto& bodyBlock = dynamic_cast<const BlockStmt&>(repeatStmt->getBody());
    const auto& bodyStatements = bodyBlock.getStatements();
    ASSERT_EQ(bodyStatements.size(), 1) << "Loop body should have exactly one statement";
    
    const auto* returnStmt = dynamic_cast<const ReturnStmt*>(bodyStatements[0].get());
    
    const auto& returnValue = dynamic_cast<const LiteralExpr&>(returnStmt->getValue());
    EXPECT_EQ(returnValue.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(returnValue.getLiteral().getValue<std::int32_t>(), 1);
}

TEST_F(ParserTestFixture, ParseLoopWithRageQuit) {
    auto parserResult = parseSource("do_until_bored { rage_quit!!! }");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* loopStmt = dynamic_cast<const LoopStmt*>(parserResult.statements[0].get());
    
    const auto& bodyBlock = dynamic_cast<const BlockStmt&>(loopStmt->getBody());
    const auto& bodyStatements = bodyBlock.getStatements();
    ASSERT_EQ(bodyStatements.size(), 1) << "Loop body should have exactly one statement";
    
    const auto* breakStmt = dynamic_cast<const BreakStmt*>(bodyStatements[0].get());
}

TEST_F(ParserTestFixture, ParsePrintStringLiteral) {
    auto parserResult = parseSource("scream: \"Hello\"...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* printStmt = dynamic_cast<const PrintStmt*>(parserResult.statements[0].get());
    
    const auto& expr = dynamic_cast<const LiteralExpr&>(printStmt->getExpression());
    
    EXPECT_EQ(expr.getLiteral().getType(), Token::Type::String);
    EXPECT_EQ(expr.getLiteral().getValue<std::string>(), "Hello");
}

TEST_F(ParserTestFixture, ParsePrintIntLiteral) {
    auto parserResult = parseSource("scream: 42...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* printStmt = dynamic_cast<const PrintStmt*>(parserResult.statements[0].get());
    
    const auto& expr = dynamic_cast<const LiteralExpr&>(printStmt->getExpression());
    
    EXPECT_EQ(expr.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(expr.getLiteral().getValue<std::int32_t>(), 42);
}

TEST_F(ParserTestFixture, ParsePrintVariable) {
    auto source = R"(
        stash x about 10...
        scream: x...
    )";
    auto parserResult = parseSource(source);
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    
    ASSERT_EQ(parserResult.statements.size(), 2);
    
    const auto* printStmt = dynamic_cast<const PrintStmt*>(parserResult.statements[1].get());
    ASSERT_NE(printStmt, nullptr) << "Second statement is not a PrintStmt";
    
    const auto& varExpr = dynamic_cast<const VariableExpr&>(printStmt->getExpression());
    
    EXPECT_EQ(varExpr.getName().getValue<std::string>(), "x");
}
