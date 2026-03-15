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

TEST_F(ParserTestFixture, ParseVarDefinitionWithPlainTrue) {
    auto parserResult = parseSource("stash true about totally...");
    
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    auto* varStmt = dynamic_cast<VarDefinitionStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "true");
    
    const auto& initializer = dynamic_cast<const LiteralExpr&>(varStmt->getInitializer());
    const auto& literal = initializer.getLiteral();
    
    EXPECT_EQ(literal.getType(), Token::Type::True);
}

TEST_F(ParserTestFixture, PrintBoolVariable) {
    auto parserResult = parseSource(
        "stash true about totally...\n"
        "gossip.spill_tea(true)...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 2);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "true");
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(parserResult.statements[1].get());
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);

    const auto& varExpr = dynamic_cast<const VariableExpr&>(*args[0]);
    EXPECT_EQ(varExpr.getName().getValue<std::string>(), "true");
}

TEST_F(ParserTestFixture, ImportModule) {
    auto parserResult = parseSource("summon gossip...");

    ASSERT_TRUE(parserResult.errors.empty());
    ASSERT_EQ(parserResult.statements.size(), 1);

    const auto* importStmt = dynamic_cast<const ImportStmt*>(parserResult.statements[0].get());

    const auto& moduleToken = importStmt->getModuleName();
    EXPECT_EQ(moduleToken.getType(), Token::Type::Ident);
    EXPECT_EQ(moduleToken.getValue<std::string>(), "gossip");
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
    auto parserResult = parseSource("gossip.spill_tea(\"Hello\")...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(parserResult.statements[0].get());
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);

    const auto& expr = dynamic_cast<const LiteralExpr&>(*args[0]);
    
    EXPECT_EQ(expr.getLiteral().getType(), Token::Type::String);
    EXPECT_EQ(expr.getLiteral().getValue<std::string>(), "Hello");
}

TEST_F(ParserTestFixture, ParsePrintIntLiteral) {
    auto parserResult = parseSource("gossip.spill_tea(42)...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(parserResult.statements[0].get());
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);

    const auto& expr = dynamic_cast<const LiteralExpr&>(*args[0]);
    
    EXPECT_EQ(expr.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(expr.getLiteral().getValue<std::int32_t>(), 42);
}

TEST_F(ParserTestFixture, ParsePrintVariable) {
    auto source = R"(
        stash x about 10...
        gossip.spill_tea(x)...
    )";
    auto parserResult = parseSource(source);
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    
    ASSERT_EQ(parserResult.statements.size(), 2);
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(parserResult.statements[1].get());
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);

    const auto& varExpr = dynamic_cast<const VariableExpr&>(*args[0]);
    
    EXPECT_EQ(varExpr.getName().getValue<std::string>(), "x");
}

TEST_F(ParserTestFixture, ParseFunctionDeclarationAndCall) {
    auto source = R"(
        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            stash number about add(2, 3)...
            yeet ghosted...
        }
    )";
    auto parserResult = parseSource(source);
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 2);
    
    const auto* addFunc = dynamic_cast<const FunctionStmt*>(parserResult.statements[0].get());

    const auto& addBody = dynamic_cast<const BlockStmt&>(addFunc->getBody());
    ASSERT_EQ(addBody.getStatements().size(), 1);
    
    const auto* returnAdd = dynamic_cast<const ReturnStmt*>(addBody.getStatements()[0].get());
    const auto& addition = dynamic_cast<const BinaryExpr&>(returnAdd->getValue());
    EXPECT_EQ(addition.getOperator().getType(), Token::Type::Plus); 

    const auto* machoFunc = dynamic_cast<const FunctionStmt*>(parserResult.statements[1].get());
    EXPECT_EQ(machoFunc->getName().getValue<std::string>(), "macho");
    
    const auto& machoBody = dynamic_cast<const BlockStmt&>(machoFunc->getBody());
    ASSERT_EQ(machoBody.getStatements().size(), 2);
    
    const auto* stashStmt = dynamic_cast<const VarDefinitionStmt*>(machoBody.getStatements()[0].get());

    const auto& callExpr = dynamic_cast<const CallExpr&>(stashStmt->getInitializer());

    const auto& calleeExpr = callExpr.getCallee();
    const auto* varExpr = dynamic_cast<const VariableExpr*>(&calleeExpr);
    EXPECT_EQ(varExpr->getName().getValue<std::string>(), "add");
    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 2);
    
    const auto& arg1 = dynamic_cast<const LiteralExpr&>(*(args[0]));
    EXPECT_EQ(arg1.getLiteral().getValue<std::int32_t>(), 2);
    
    const auto* returnGhosted = dynamic_cast<const ReturnStmt*>(machoBody.getStatements()[1].get());
    const auto& ghostedValue = dynamic_cast<const LiteralExpr&>(returnGhosted->getValue());
    EXPECT_EQ(ghostedValue.getLiteral().getType(), Token::Type::Null);
}

TEST_F(ParserTestFixture, ParseAssignStatement) {
    auto parserResult = parseSource("counter might_be 42...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* reassignStmt = dynamic_cast<const ReassignStmt*>(parserResult.statements[0].get());
    
    EXPECT_EQ(reassignStmt->getName().getValue<std::string>(), "counter");

    const auto& expr = dynamic_cast<const LiteralExpr&>(reassignStmt->getValue());
    EXPECT_EQ(expr.getLiteral().getType(), Token::Type::Int);
    EXPECT_EQ(expr.getLiteral().getValue<std::int32_t>(), 42);
}

TEST_F(ParserTestFixture, ParseIncAsUnaryExpressionStatement) {
    auto parserResult = parseSource("pump_it counter...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(parserResult.statements[0].get());
    
    const auto& unaryExpr = dynamic_cast<const UnaryExpr&>(exprStmt->getExpression());
    
    EXPECT_EQ(unaryExpr.getOperator().getType(), Token::Type::Incr);
    
    const auto& operand = dynamic_cast<const VariableExpr&>(unaryExpr.getRight());
    
    EXPECT_EQ(operand.getName().getValue<std::string>(), "counter");
}

TEST_F(ParserTestFixture, ParseIfWithBoolLiteral) {
    auto parserResult = parseSource("perhaps (totally) { yeet ghosted... }");

    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);

    const auto* ifStmt = dynamic_cast<const IfStmt*>(parserResult.statements[0].get());

    const auto& conditionExpr = dynamic_cast<const LiteralExpr&>(ifStmt->getCondition());
    EXPECT_EQ(conditionExpr.getLiteral().getType(), Token::Type::True);

    const auto& thenBlock = dynamic_cast<const BlockStmt&>(ifStmt->getThenBlock());
    const auto& thenStatements = thenBlock.getStatements();
    ASSERT_EQ(thenStatements.size(), 1);

    const auto* returnStmt = dynamic_cast<const ReturnStmt*>(thenStatements[0].get());
    const auto& returnValue = dynamic_cast<const LiteralExpr&>(returnStmt->getValue());
    EXPECT_EQ(returnValue.getLiteral().getType(), Token::Type::Null);
}

TEST_F(ParserTestFixture, ParseVoidFunction) {
    auto source = R"(
        gig print(x) {
            gossip.spill_tea(x)...
        })";
    
    auto parserResult = parseSource(source);
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* funcStmt = dynamic_cast<const FunctionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(funcStmt->getName().getValue<std::string>(), "print");
    
    const auto& params = funcStmt->getParameters();
    ASSERT_EQ(params.size(), 1);
    EXPECT_EQ(params[0].getValue<std::string>(), "x");
    
    const auto& body = dynamic_cast<const BlockStmt&>(funcStmt->getBody());
    const auto& statements = body.getStatements();
    ASSERT_EQ(statements.size(), 1);
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(statements[0].get());
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);

    const auto& printExpr = dynamic_cast<const VariableExpr&>(*args[0]);
    EXPECT_EQ(printExpr.getName().getValue<std::string>(), "x");
}

TEST_F(ParserTestFixture, ParseVoidFunctionCall) {
    auto source = R"(
        gig print(x) {
            gossip.spill_tea(x)...
        }

        gig macho() {
            stash string about "hello"...
            print(string)...
        }
    )";
    
    auto parserResult = parseSource(source);
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 2);
    
    const auto* printFunc = dynamic_cast<const FunctionStmt*>(parserResult.statements[0].get());
    EXPECT_EQ(printFunc->getName().getValue<std::string>(), "print");
    
    const auto* machoFunc = dynamic_cast<const FunctionStmt*>(parserResult.statements[1].get());
    EXPECT_EQ(machoFunc->getName().getValue<std::string>(), "macho");
    
    const auto& machoBody = dynamic_cast<const BlockStmt&>(machoFunc->getBody());
    const auto& machoStatements = machoBody.getStatements();
    ASSERT_EQ(machoStatements.size(), 2);
    
    const auto* varStmt = dynamic_cast<const VarDefinitionStmt*>(machoStatements[0].get());
    EXPECT_EQ(varStmt->getName().getValue<std::string>(), "string");
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(machoStatements[1].get());
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& calleeExpr = callExpr.getCallee();
    const auto* varExpr = dynamic_cast<const VariableExpr*>(&calleeExpr);
    EXPECT_EQ(varExpr->getName().getValue<std::string>(), "print");
    
    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);
    
    const auto& arg = dynamic_cast<const VariableExpr&>(*args[0]);
    EXPECT_EQ(arg.getName().getValue<std::string>(), "string");
}

TEST_F(ParserTestFixture, ParseModuleVoidFunctionCall) {
    auto parserResult = parseSource("gossip.spill_tea(\"Hello\")...");
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser returned errors!";
    ASSERT_EQ(parserResult.statements.size(), 1);
    
    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(parserResult.statements[0].get());
    
    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");
    
    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);
    
    const auto& arg = dynamic_cast<const LiteralExpr&>(*args[0]);
    EXPECT_EQ(arg.getLiteral().getType(), Token::Type::String);
    EXPECT_EQ(arg.getLiteral().getValue<std::string>(), "Hello");
}



TEST_F(ParserTestFixture, ParseEntirePrototypeMess) {
    auto parserResult = parseSource(
        "psst: very useful thingy\n"
        "gig calculate_stuff (x, y) {\n"
        "    yeet 2 with 2 without 2...\n"
        "}\n"
        "\n"
        "psst: This is the start of the mess\n"
        "gig macho() {\n"
        "    stash number about 10...\n"
        "    stash isNumberTen about number looks_like 11...\n"
        "\n" // line 10
        "    perhaps (isNumberTen looks_like totally) {\n"
        "        gossip.spill_tea(\"The number is is ten\")...\n"
        "    }\n"
        "    or_whatever (isNumberTen looks_like nah) {\n"
        "        gossip.spill_tea(\"The number is not ten\")...\n"
        "    }\n"
        "    screw_it {\n"
        "        gossip.spill_tea(\"How the fck did I get here\")...\n"
        "    }\n"
        "\n" // line 20
        "    stash floatingNumber about 11.0...\n"
        "    perhaps (floatingNumber looks_like 10.0) {\n"
        "        gossip.spill_tea(\"The floatingNumber is ten\")...\n"
        "    }\n"
        "    or_whatever (floatingNumber kinda_sus 20.0) {\n"
        "        gossip.spill_tea(\"The floatingNumber is not 20\")...\n"
        "    }\n"
        "    or_whatever (floatingNumber tiny_ish 5.0) {\n"
        "        gossip.spill_tea(\"The floatingNumber is smaller than 5\")...\n"
        "    }\n" // line 30
        "    screw_it {\n"
        "        gossip.spill_tea(\"This language is so weird\")...\n"
        "    }\n"
        "\n"
        "    stash counter about 0...\n"
        "    do_until_bored {\n"
        "        gossip.spill_tea(counter)...\n"
        "        pump_it counter...\n"
        "\n"
        "        perhaps (counter bigger_ish 3) {\n" // line 40
        "            rage_quit!!!\n"
        "        }\n"
        "    }\n"
        "\n"
        "    stash n about calculate_stuff(10, 20)...\n"
        "    spin_around (n) {\n"
        "        gossip.spill_tea(\"Spinnin\")...\n"
        "    }\n"
        "\n"
        "    yeet ghosted...\n" // line 50
        "}\n"
        "\n"
        "rant_stop\n"
        "    Output:\n"
        "    \n"
        "    THE NUMBER IS NOT TEN!!!\n"
        "    THE FLOATINGNUMBER IS NOT 20!!!\n"
        "    0!!!\n"
        "    1!!!\n"
        "    2!!!\n" // line 60
        "    3!!!\n"
        "    4!!!\n"
        "    SPINNIN!!!\n"
        "    SPINNIN!!!\n"
        "\n"
        "    Syntax:\n"
        "        Every statement must end with an ellipsis (...) to indicate hesitation.\n"
        "    Comments:\n"
        "        Single-line comments use 'psst:',\n"
        "        block comments are between 'rant_stop' and 'rant_start'.\n" // line 70
        "    Variables:\n"
        "        Declared using 'stash [name] about [value]' (dynamic typing).\n"
        "    Assignment:\n"
        "        Variable updates use 'might_be' instead of 'about'.\n"
        "    Types:\n"
        "        Booleans are 'totally' (true), 'nah' (false)\n"
        "        null is 'ghosted'.\n"
        "    Functions:\n"
        "        Defined using the 'gig' keyword and return values using 'yeet'.\n"
        "    Output:\n"
        "        'gossip.spill_tea' prints arguments to console in UPPERCASE with appended '!!!'.\n"
        "    Conditionals:\n"
        "        Logic flow uses 'perhaps' (if), 'or_whatever' (else if), and 'screw_it' (else).\n"
        "    Loops:\n"
        "        Iteration implemented via 'do_until_bored' (while) and 'spin_around' (for).\n"
        "    Flow Control:\n"
        "        Loops are terminated aggressively using the 'rage_quit!!!' command.\n"
        "    Equality:\n"
        "        Comparisons use 'looks_like' (==) and 'kinda_sus' (!=).\n"
        "    Relational:\n"
        "        Size comparisons use 'bigger_ish' (>) and 'tiny_ish' (<).\n"
        "    Math:\n"
        "        Arithmetic uses 'with' (+) and 'without' (-)\n"
        "        The 'pump_it' operator is used for incrementing values.\n"
        "rant_start\n"
    );
    
    ASSERT_EQ(parserResult.errors.size(), 0) << "Parser errors found in the prototype!";
    ASSERT_EQ(parserResult.statements.size(), 2) << "Expected exactly 2 global statements (functions)";

    // Line 2: gig calculate_stuff (x, y) { ... }
    const auto& calcFunc = dynamic_cast<const FunctionStmt&>(*parserResult.statements[0]);
    const auto& calcBody = dynamic_cast<const BlockStmt&>(calcFunc.getBody());
    const auto& yeetMath = dynamic_cast<const ReturnStmt&>(*calcBody.getStatements()[0]);
    const auto& mathExpr = dynamic_cast<const BinaryExpr&>(yeetMath.getValue());
    EXPECT_EQ(mathExpr.getOperator().getType(), Token::Type::Minus);

    const auto& leftMath = dynamic_cast<const BinaryExpr&>(mathExpr.getLeft());
    EXPECT_EQ(leftMath.getOperator().getType(), Token::Type::Plus);

    // Line 7: gig macho() { ... }
    const auto& machoFunc = dynamic_cast<const FunctionStmt&>(*parserResult.statements[1]);
    const auto& machoStatements = dynamic_cast<const BlockStmt&>(machoFunc.getBody()).getStatements();
    
    ASSERT_EQ(machoStatements.size(), 10);

    // Line 8: stash number about 10...
    const auto& stmt0 = dynamic_cast<const VarDefinitionStmt&>(*machoStatements[0]);
    EXPECT_EQ(stmt0.getName().getValue<std::string>(), "number");

    // Line 9: stash isNumberTen about number looks_like 11...
    const auto& stmt1 = dynamic_cast<const VarDefinitionStmt&>(*machoStatements[1]);
    const auto& looksLikeExpr = dynamic_cast<const BinaryExpr&>(stmt1.getInitializer());
    EXPECT_EQ(looksLikeExpr.getOperator().getType(), Token::Type::Equal);

    // Line 11: perhaps (isNumberTen looks_like totally) { ... }
    const auto& stmt2 = dynamic_cast<const IfStmt&>(*machoStatements[2]);
    const auto& ifCond = dynamic_cast<const BinaryExpr&>(stmt2.getCondition());
    const auto& trueToken = dynamic_cast<const LiteralExpr&>(ifCond.getRight());
    EXPECT_EQ(trueToken.getLiteral().getType(), Token::Type::True);
    
    // Line 14: or_whatever (isNumberTen looks_like nah) { ... }
    EXPECT_EQ(stmt2.getElseIfClauses().size(), 1);
    
    // Line 17: screw_it { ... }
    const auto& elseBlock = dynamic_cast<const BlockStmt&>(stmt2.getElseBlock());
    EXPECT_EQ(elseBlock.getStatements().size(), 1);

    // Line 21: stash floatingNumber about 11.0...
    const auto& stmt3 = dynamic_cast<const VarDefinitionStmt&>(*machoStatements[3]);
    EXPECT_EQ(stmt3.getName().getValue<std::string>(), "floatingNumber");

    // Line 22: perhaps (floatingNumber looks_like 10.0) { ... }
    const auto& stmt4 = dynamic_cast<const IfStmt&>(*machoStatements[4]);
    // Line 25 & 28: 2x or_whatever
    EXPECT_EQ(stmt4.getElseIfClauses().size(), 2);
    // Line 31: screw_it
    EXPECT_EQ(dynamic_cast<const BlockStmt&>(stmt4.getElseBlock()).getStatements().size(), 1);

    // Line 35: stash counter about 0...
    const auto& stmt5 = dynamic_cast<const VarDefinitionStmt&>(*machoStatements[5]);
    EXPECT_EQ(stmt5.getName().getValue<std::string>(), "counter");

    // Line 36: do_until_bored { ... }
    const auto& stmt6 = dynamic_cast<const LoopStmt&>(*machoStatements[6]);
    const auto& doUntilBody = dynamic_cast<const BlockStmt&>(stmt6.getBody()).getStatements();
    ASSERT_EQ(doUntilBody.size(), 3);
    
    // Line 37: gossip.spill_tea(counter)...
    const auto& loopCallStmt = dynamic_cast<const ExpressionStmt&>(*doUntilBody[0]);
    const auto& loopCallExpr = dynamic_cast<const CallExpr&>(loopCallStmt.getExpression());
    const auto& loopDotExpr = dynamic_cast<const DotExpr&>(loopCallExpr.getCallee());
    const auto& loopObjectExpr = dynamic_cast<const VariableExpr&>(loopDotExpr.getLeft());
    EXPECT_EQ(loopObjectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(loopDotExpr.getRight().getValue<std::string>(), "spill_tea");
    ASSERT_EQ(loopCallExpr.getArgs().size(), 1);
    const auto& loopPrintArg = dynamic_cast<const VariableExpr&>(*loopCallExpr.getArgs()[0]);
    EXPECT_EQ(loopPrintArg.getName().getValue<std::string>(), "counter");
    
    // Line 38: pump_it counter...
    const auto& loopPumpIt = dynamic_cast<const ExpressionStmt&>(*doUntilBody[1]);
    const auto& pumpItUnary = dynamic_cast<const UnaryExpr&>(loopPumpIt.getExpression());
    EXPECT_EQ(pumpItUnary.getOperator().getType(), Token::Type::Incr);
    
    // Line 40: perhaps (counter bigger_ish 3) { rage_quit!!! }
    const auto& loopIf = dynamic_cast<const IfStmt&>(*doUntilBody[2]);
    const auto& loopIfBody = dynamic_cast<const BlockStmt&>(loopIf.getThenBlock()).getStatements();
    const auto& rageQuit = dynamic_cast<const BreakStmt&>(*loopIfBody[0]);

    // Line 45: stash n about calculate_stuff(10, 20)...
    const auto& stmt7 = dynamic_cast<const VarDefinitionStmt&>(*machoStatements[7]);
    const auto& callExpr = dynamic_cast<const CallExpr&>(stmt7.getInitializer());
    const auto& calleeExpr = callExpr.getCallee();
    const auto* varExpr = dynamic_cast<const VariableExpr*>(&calleeExpr);
    EXPECT_EQ(varExpr->getName().getValue<std::string>(), "calculate_stuff");
    EXPECT_EQ(callExpr.getArgs().size(), 2);

    // Line 46: spin_around (n) { ... }
    const auto& stmt8 = dynamic_cast<const RepeatStmt&>(*machoStatements[8]);
    const auto& repeatCount = dynamic_cast<const VariableExpr&>(stmt8.getCount());
    EXPECT_EQ(repeatCount.getName().getValue<std::string>(), "n");
    
    // Line 47: gossip.spill_tea("Spinnin")...
    const auto& repeatBody = dynamic_cast<const BlockStmt&>(stmt8.getBody()).getStatements();
    const auto& repeatCallStmt = dynamic_cast<const ExpressionStmt&>(*repeatBody[0]);
    const auto& repeatCallExpr = dynamic_cast<const CallExpr&>(repeatCallStmt.getExpression());
    const auto& repeatDotExpr = dynamic_cast<const DotExpr&>(repeatCallExpr.getCallee());
    const auto& repeatObjectExpr = dynamic_cast<const VariableExpr&>(repeatDotExpr.getLeft());
    EXPECT_EQ(repeatObjectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(repeatDotExpr.getRight().getValue<std::string>(), "spill_tea");
    ASSERT_EQ(repeatCallExpr.getArgs().size(), 1);
    const auto& repeatArg = dynamic_cast<const LiteralExpr&>(*repeatCallExpr.getArgs()[0]);
    EXPECT_EQ(repeatArg.getLiteral().getType(), Token::Type::String);
    EXPECT_EQ(repeatArg.getLiteral().getValue<std::string>(), "Spinnin");

    // Line 50: yeet ghosted...
    const auto& stmt9 = dynamic_cast<const ReturnStmt&>(*machoStatements[9]);
    const auto& yeetValue = dynamic_cast<const LiteralExpr&>(stmt9.getValue());
    EXPECT_EQ(yeetValue.getLiteral().getType(), Token::Type::Null);
}
