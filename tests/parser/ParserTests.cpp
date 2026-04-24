#include <gtest/gtest.h>
#include <algorithm>
#include <initializer_list>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "utils/AstPrinter.hpp"

struct ParserTests : public ::testing::Test {
    static ParserResult parse(const std::string& source) {
        Lexer lexer{source, 0ull};
        auto lexerResult = lexer.tokenize();
        EXPECT_TRUE(lexerResult.errors.empty());
        
        Parser parser{std::move(lexerResult.tokens)};
        return parser.parse();
    }

    void parseOk(const std::string& source) {
        auto parserResult = parse(source);
        EXPECT_TRUE(parserResult.errors.empty());
        ast = std::move(parserResult.statements);
    }

    std::string printAst() {
        std::ostringstream out;
        AstPrinter{out}.print(ast);
        return out.str();
    }

    void expectPrintedInOrder(
        std::initializer_list<std::string_view> needles) {
        
        const auto output = printAst();
        std::size_t pos = 0;
        for (const auto needle : needles) {
            const auto found = output.find(needle, pos);
            ASSERT_NE(found, std::string::npos) << "Missing (after pos=" << pos << "): " << needle;
            pos = found + needle.size();
        }
    }

    void expectNumberOfStatements(size_t n) {
        EXPECT_EQ(ast.size(), n);
    }

    template <typename T>
    const T* expectStmt(size_t index) {
        if (index >= ast.size()) {
            ADD_FAILURE() << "Index " << index << " out of bounds (size: " << ast.size() << ")";
            throw std::out_of_range("AST index out of bounds");
        }
        const T* stmt = dynamic_cast<const T*>(ast[index].get());
        if (!stmt) {
            ADD_FAILURE() << "Failed to cast statement at index " << index;
            throw std::runtime_error("Bad AST cast");
        }
        return stmt;
    }

    template <typename T, typename Base>
    const T& expectCast(const Base& base) {
        const T* casted = dynamic_cast<const T*>(&base);
        if (!casted) {
            ADD_FAILURE() << "Failed to cast expression/statement";
            throw std::runtime_error("Bad AST cast");
        }
        return *casted;
    }

    const VarDefinitionStmt& expectVarDef(size_t index, const std::string& expectedName) {
        const auto* stmt = expectStmt<VarDefinitionStmt>(index);
        EXPECT_NE(stmt, nullptr);
        EXPECT_EQ(stmt->getName().getValue<std::string>(), expectedName);
        return *stmt;
    }

    const ReassignStmt& expectReassign(size_t index, const std::string& expectedName) {
        const auto* stmt = expectStmt<ReassignStmt>(index);
        EXPECT_NE(stmt, nullptr);
        EXPECT_EQ(stmt->getName().getValue<std::string>(), expectedName);
        return *stmt;
    }

    const FunctionStmt& expectFunction(size_t index, const std::string& expectedName) {
        const auto* stmt = expectStmt<FunctionStmt>(index);
        EXPECT_NE(stmt, nullptr);
        EXPECT_EQ(stmt->getName().getValue<std::string>(), expectedName);
        return *stmt;
    }

    const ReturnStmt& expectReturn(const BlockStmt& block, size_t index) {
        EXPECT_LT(index, block.getStatements().size());
        const auto* ret = dynamic_cast<const ReturnStmt*>(block.getStatements()[index].get());
        EXPECT_NE(ret, nullptr) << "Expected ReturnStmt at block index " << index;
        return *ret;
    }

    const LiteralExpr& expectLiteral(const Expr& expr, Token::Type expectedType) {
        const auto& literalExpr = expectCast<LiteralExpr>(expr);
        EXPECT_EQ(literalExpr.getLiteral().getType(), expectedType);
        return literalExpr;
    }

    template <typename V>
    void expectLiteralValue(const Expr& expr, Token::Type expectedType, const V& expectedValue) {
        const auto& literalExpr = expectLiteral(expr, expectedType);
        if constexpr (std::is_floating_point_v<V>) {
            EXPECT_FLOAT_EQ(literalExpr.getLiteral().getValue<V>(), expectedValue);
        }
        else {
            EXPECT_EQ(literalExpr.getLiteral().getValue<V>(), expectedValue);
        }
    }

    const BinaryExpr& expectBinaryExpr(const Expr& expr, Token::Type expectedOp) {
        const auto& binExpr = expectCast<BinaryExpr>(expr);
        EXPECT_EQ(binExpr.getOperator().getType(), expectedOp);
        return binExpr;
    }

    const LogicalExpr& expectLogicalExpr(const Expr& expr, Token::Type expectedOp) {
        const auto& logExpr = expectCast<LogicalExpr>(expr);
        EXPECT_EQ(logExpr.getOperator().getType(), expectedOp);
        return logExpr;
    }

    const UnaryExpr& expectUnaryExpr(const Expr& expr, Token::Type expectedOp) {
        const auto& unaryExpr = expectCast<UnaryExpr>(expr);
        EXPECT_EQ(unaryExpr.getOperator().getType(), expectedOp);
        return unaryExpr;
    }

    const VariableExpr& expectVariableExpr(const Expr& expr, const std::string& expectedName) {
        const auto& varExpr = expectCast<VariableExpr>(expr);
        EXPECT_EQ(varExpr.getName().getValue<std::string>(), expectedName);
        return varExpr;
    }

    void expectVarDefBinOpLits(const std::string& varName, Token::Type opType, Token::Type litType, std::int32_t leftVal, std::int32_t rightVal) {
        const auto& varStmt = expectVarDef(0, varName);
        const auto& binExpr = expectBinaryExpr(varStmt.getInitializer(), opType);
        expectLiteralValue<std::int32_t>(binExpr.getLeft(), litType, leftVal);
        expectLiteralValue<std::int32_t>(binExpr.getRight(), litType, rightVal);
    }

    void expectVarDefLogOpLits(const std::string& varName, Token::Type opType, Token::Type leftType, Token::Type rightType) {
        const auto& varStmt = expectVarDef(0, varName);
        const auto& logExpr = expectLogicalExpr(varStmt.getInitializer(), opType);
        expectLiteral(logExpr.getLeft(), leftType);
        expectLiteral(logExpr.getRight(), rightType);
    }

    static std::string joinErrorMessages(const std::vector<ParserError>& errors) {
        std::ostringstream out;
        for (const auto& err : errors) {
            out << "- (" << err.badToken.getLine() << ":" << err.badToken.getColumn() << ") "
                << err.msg << "\n";
        }
        return out.str();
    }

    static void expectHasErrorContaining(const ParserResult& result, std::string_view needle) {
        ASSERT_FALSE(result.errors.empty())
            << "Expected at least one parser error.";

        const bool found = std::any_of(result.errors.begin(), result.errors.end(), [&](const ParserError& err) {
            return err.msg.find(needle) != std::string::npos;
        });

        ASSERT_TRUE(found)
            << "Missing error containing: " << needle << "\n"
            << "Errors were:\n" << joinErrorMessages(result.errors);
    }

    static void expectNoErrorContaining(const ParserResult& result, std::string_view needle) {
        const bool found = std::any_of(result.errors.begin(), result.errors.end(), [&](const ParserError& err) {
            return err.msg.find(needle) != std::string::npos;
        });

        ASSERT_FALSE(found)
            << "Unexpected error containing: " << needle << "\n"
            << "Errors were:\n" << joinErrorMessages(result.errors);
    }

    static void expectErrorMessagesEq(
        const ParserResult& result,
        std::initializer_list<std::string_view> expectedMessages) {

        ASSERT_EQ(result.errors.size(), expectedMessages.size())
            << "Errors were:\n" << joinErrorMessages(result.errors);

        std::size_t idx = 0;
        for (const auto expected : expectedMessages) {
            EXPECT_EQ(result.errors[idx].msg, expected)
                << "Mismatch at error index " << idx << "\n"
                << "Errors were:\n" << joinErrorMessages(result.errors);
            ++idx;
        }
    }

    std::vector<std::unique_ptr<Stmt>> ast{};
};

TEST_F(ParserTests, Snapshot_FunctionWithReturnIntStatement) {
    parseOk(
        "gig macho() {\n"
        "    yeet 0...\n"
        "}\n");
    expectNumberOfStatements(1ull);
    
    expectPrintedInOrder({
        "FunctionStmt Ident (macho)",
        "params:",
        "body:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (0)",
    });
}

TEST_F(ParserTests, Snapshot_FunctionWithImplicitNullReturnStatement) {
    parseOk(
        "gig macho() {\n"
        "    yeet...\n"
        "}\n");
    expectNumberOfStatements(1ull);
    
    expectPrintedInOrder({
        "FunctionStmt Ident (macho)",
        "params:",
        "body:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Null",
    });
}

TEST_F(ParserTests, Structural_VarDefinitionWithInt) {
    parseOk("stash integer about 1...");
    expectNumberOfStatements(1ull);
    
    const auto& varStmt = expectVarDef(0, "integer");
    expectLiteralValue<std::int32_t>(varStmt.getInitializer(), Token::Type::Int, 1);
}

TEST_F(ParserTests, Structural_VarDefinitionWithFloat) {
    parseOk("stash float about 1.0...");
    expectNumberOfStatements(1ull);

    const auto& varStmt = expectVarDef(0, "float");
    expectLiteralValue<double>(varStmt.getInitializer(), Token::Type::Float, 1.0);
}

TEST_F(ParserTests, Structural_VarDefinitionWithTrue) {
    parseOk("stash truth about totally...");
    expectNumberOfStatements(1ull);
    
    const auto& varStmt = expectVarDef(0, "truth");
    expectLiteral(varStmt.getInitializer(), Token::Type::True);
}

TEST_F(ParserTests, Structural_VarDefinitionWithFalse) {
    parseOk("stash truth about nah...");
    expectNumberOfStatements(1ull);    

    const auto& varStmt = expectVarDef(0, "truth");
    expectLiteral(varStmt.getInitializer(), Token::Type::False);
}

TEST_F(ParserTests, Structural_VarDefinitionWithoutInitializer_UsesDefaultValue) {
    parseOk("stash null...\n");
    expectNumberOfStatements(1ull);

    const auto& varStmt = expectVarDef(0, "null");
    expectLiteral(varStmt.getInitializer(), Token::Type::Null);
}

TEST_F(ParserTests, Structural_VarDefinitionWithEqualityExpression) {
    parseOk("stash comparison about 10 looks_like 10...");
    expectNumberOfStatements(1ull);
    expectVarDefBinOpLits("comparison", Token::Type::Equal, Token::Type::Int, 10, 10);
}

TEST_F(ParserTests, Structural_VarDefinitionWithPlainTrue) {
    parseOk("stash true about totally...");
    expectNumberOfStatements(1ull);
    
    const auto& varStmt = expectVarDef(0, "true");
    expectLiteral(varStmt.getInitializer(), Token::Type::True);
}

TEST_F(ParserTests, Snapshot_PrintBoolVariable) {
    parseOk(
        "stash true about totally...\n"
        "gossip.spill_tea(true)...");
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (true)",
        "initializer:",
        "LiteralExpr True",
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExpr",
        "left:",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "VariableExpr Ident (true)",
    });
}

TEST_F(ParserTests, Structural_SummonAndVectorPrintWithVariableElement) {
    parseOk("stash list about [11, x, 33]...");
    expectNumberOfStatements(1ull);

    const auto& listVarStmt = expectVarDef(0, "list");
    const auto& vectorExpr = expectCast<VectorExpr>(listVarStmt.getInitializer());
    ASSERT_EQ(vectorExpr.getInitializers().size(), 3);

    expectLiteralValue<std::int32_t>(*vectorExpr.getInitializers()[0], Token::Type::Int, 11);
    expectVariableExpr(*vectorExpr.getInitializers()[1], "x");
    expectLiteralValue<std::int32_t>(*vectorExpr.getInitializers()[2], Token::Type::Int, 33);
}

TEST_F(ParserTests, Structural_ImportModule) {
    parseOk("summon gossip...");
    expectNumberOfStatements(1ull);

    const auto* importStmt = expectStmt<ImportStmt>(0);
    EXPECT_EQ(importStmt->getModuleName().getType(), Token::Type::Ident);
    EXPECT_EQ(importStmt->getModuleName().getValue<std::string>(), "gossip");
}

TEST_F(ParserTests, Structural_VarDefinitionWithNotEqualExpression) {
    parseOk("stash comparison about 10 kinda_sus 5...");
    expectNumberOfStatements(1ull);
    expectVarDefBinOpLits("comparison", Token::Type::NotEqual, Token::Type::Int, 10, 5);
}

TEST_F(ParserTests, Structural_VarDefinitionWithGreaterExpression) {
    parseOk("stash comparison about 20 bigger_ish 10...");
    expectNumberOfStatements(1ull);
    expectVarDefBinOpLits("comparison", Token::Type::Greater, Token::Type::Int, 20, 10);
}

TEST_F(ParserTests, Structural_VarDefinitionWithLessExpression) {
    parseOk("stash comparison about 5 tiny_ish 10...");
    expectNumberOfStatements(1ull);
    expectVarDefBinOpLits("comparison", Token::Type::Less, Token::Type::Int, 5, 10);
}

TEST_F(ParserTests, Structural_PrecedenceEqualityAndAddition) {
    parseOk("stash result about 2 with 2 looks_like 4...");
    expectNumberOfStatements(1ull);
    
    const auto& varStmt = expectVarDef(0, "result");
    
    const auto& equalityExpr = expectBinaryExpr(varStmt.getInitializer(), Token::Type::Equal);
    const auto& additionExpr = expectBinaryExpr(equalityExpr.getLeft(), Token::Type::Plus);
    
    expectLiteralValue<std::int32_t>(additionExpr.getLeft(), Token::Type::Int, 2);
    expectLiteralValue<std::int32_t>(additionExpr.getRight(), Token::Type::Int, 2);
    expectLiteralValue<std::int32_t>(equalityExpr.getRight(), Token::Type::Int, 4);
}

TEST_F(ParserTests, Structural_VarDefinitionWithAndExpression) {
    parseOk("stash result about totally also nah...");
    expectNumberOfStatements(1ull);
    expectVarDefLogOpLits("result", Token::Type::And, Token::Type::True, Token::Type::False);
}

TEST_F(ParserTests, Structural_VarDefinitionWithOrExpression) {
    parseOk("stash result about nah either totally...");
    expectNumberOfStatements(1ull);
    expectVarDefLogOpLits("result", Token::Type::Or, Token::Type::False, Token::Type::True);
}

TEST_F(ParserTests, Snapshot_PerhapsWithMixedAndOrCondition) {
    parseOk("perhaps (totally also nah either totally) { yeet 1... }");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "IfStmt",
        "condition:",
        "LogicalExpr Or",
        "left:",
        "LogicalExpr And",
        "left:",
        "LiteralExpr True",
        "right:",
        "LiteralExpr False",
        "right:",
        "LiteralExpr True",
        "then:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (1)",
    });
}

TEST_F(ParserTests, Snapshot_SimpleIfStatement) {
    parseOk("perhaps (totally) { yeet 1... }");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "IfStmt",
        "condition:",
        "LiteralExpr True",
        "then:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (1)",
    });
}

TEST_F(ParserTests, Snapshot_FullIfElseChain) {
    parseOk(R"(
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
    )");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "IfStmt",
        "condition:",
        "LiteralExpr Int (1)",
        "then:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (1)",
        "ElseIf",
        "condition:",
        "LiteralExpr Int (2)",
        "body:",
        "ReturnStmt",
        "LiteralExpr Int (2)",
        "ElseIf",
        "condition:",
        "LiteralExpr Int (3)",
        "body:",
        "ReturnStmt",
        "LiteralExpr Int (3)",
        "else:",
        "ReturnStmt",
        "LiteralExpr Int (4)",
    });
}

TEST_F(ParserTests, Snapshot_DoUntilBoredStatement) {
    parseOk("do_until_bored { yeet 1... }");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "LoopStmt",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (1)",
    });
}

TEST_F(ParserTests, Snapshot_SpinAroundStatement) {
    parseOk("spin_around (5) { yeet 1... }");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "RepeatStmt",
        "count:",
        "LiteralExpr Int (5)",
        "body:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (1)",
    });
}

TEST_F(ParserTests, Snapshot_LoopWithRageQuit) {
    parseOk("do_until_bored { rage_quit!!! }");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "LoopStmt",
        "BlockStmt",
        "BreakStmt",
    });
}

TEST_F(ParserTests, Hybrid_ParsePrintStringLiteral) {
    parseOk("gossip.spill_tea(\"Hello\")...");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExp",
        "left:",
        "right Ident (spill_tea)",
        "args:",
        "LiteralExpr String (Hello)",
    });

    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(ast[0].get());

    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);
}

TEST_F(ParserTests, Hybrid_ParsePrintIntLiteral) {
    parseOk("gossip.spill_tea(42)...");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExpr",
        "left:",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "LiteralExpr Int (42)",
    });

    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(ast[0].get());

    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);
}

TEST_F(ParserTests, Hybrid_ParsePrintVariable) {
    parseOk(R"(
        stash x about 10...
        gossip.spill_tea(x)...
    )");
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (x)",
        "initializer:",
        "LiteralExpr Int (10)",
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExpr",
        "left:",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "VariableExpr Ident (x)",
    });

    const auto* exprStmt = dynamic_cast<const ExpressionStmt*>(ast[1].get());

    const auto& callExpr = dynamic_cast<const CallExpr&>(exprStmt->getExpression());
    const auto& dotExpr = dynamic_cast<const DotExpr&>(callExpr.getCallee());
    const auto& objectExpr = dynamic_cast<const VariableExpr&>(dotExpr.getLeft());
    EXPECT_EQ(objectExpr.getName().getValue<std::string>(), "gossip");
    EXPECT_EQ(dotExpr.getRight().getValue<std::string>(), "spill_tea");

    const auto& args = callExpr.getArgs();
    ASSERT_EQ(args.size(), 1);
}

TEST_F(ParserTests, Snapshot_FunctionDeclarationAndCall) {
    parseOk(R"(
        gig add(x, y) {
            yeet x with y...
        }

        gig macho() {
            stash number about add(2, 3)...
            yeet ghosted...
        }
    )");
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "FunctionStmt Ident (add)",
        "params: Ident (x), Ident (y)",
        "body:",
        "BlockStmt",
        "ReturnStmt",
        "BinaryExpr Plus",
        "VariableExpr Ident (x)",
        "VariableExpr Ident (y)",

        "FunctionStmt Ident (macho)",
        "BlockStmt",
        "VarDefinitionStmt Ident (number)",
        "CallExpr",
        "callee:",
        "VariableExpr Ident (add)",
        "args:",
        "LiteralExpr Int (2)",
        "LiteralExpr Int (3)",
        "ReturnStmt",
        "LiteralExpr Null",
    });
}

TEST_F(ParserTests, Structural_AssignStatement) {
    parseOk("counter might_be 42...");
    expectNumberOfStatements(1ull);
    
    const auto& reassignStmt = expectReassign(0, "counter");
    expectLiteralValue<std::int32_t>(reassignStmt.getValue(), Token::Type::Int, 42);
}

TEST_F(ParserTests, Structural_ParseIncAsUnaryExpressionStatement) {
    parseOk("pump_it counter...");
    expectNumberOfStatements(1ull);
    
    const auto* exprStmt = expectStmt<ExpressionStmt>(0);
    const auto& unaryExpr = expectUnaryExpr(exprStmt->getExpression(), Token::Type::Incr);
    expectVariableExpr(unaryExpr.getRight(), "counter");
}

TEST_F(ParserTests, Snapshot_IfWithBoolLiteral) {
    parseOk("perhaps (totally) { yeet ghosted... }");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "IfStmt",
        "condition:",
        "LiteralExpr True",
        "then:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Null",
    });
}

TEST_F(ParserTests, Snapshot_VoidFunction) {
    parseOk(R"(
        gig print(x) {
            gossip.spill_tea(x)...
        })");
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "FunctionStmt Ident (print)",
        "params: Ident (x)",
        "body:",
        "BlockStmt",
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExpr",
        "left:",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "VariableExpr Ident (x)",
    });
}

TEST_F(ParserTests, Snapshot_VoidFunctionCall) {
    parseOk(R"(
        gig print(x) {
            gossip.spill_tea(x)...
        }

        gig macho() {
            stash string about "hello"...
            print(string)...
        }
    )");
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "FunctionStmt Ident (print)",
        "params: Ident (x)",
        "body:",
        "BlockStmt",
        "CallExpr",
        "DotExpr",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "VariableExpr Ident (x)",

        "FunctionStmt Ident (macho)",
        "BlockStmt",
        "VarDefinitionStmt Ident (string)",
        "LiteralExpr String (hello)",
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "VariableExpr Ident (print)",
        "args:",
        "VariableExpr Ident (string)",
    });
}

TEST_F(ParserTests, Structure_ComplexNestedLogicalCondition) {
    parseOk(
        "perhaps (((x bigger_ish 0) also (x tiny_ish 4)) also\n"
        "         ((y bigger_ish 0) also (y tiny_ish 4)) also\n"
        "         (index looks_like \" \")) {\n"
        "    yeet 1...\n"
        "}\n"
    );
    expectNumberOfStatements(1ull);
    
    const auto* ifStmt = expectStmt<IfStmt>(0);
    EXPECT_TRUE(ifStmt->getElseIfClauses().empty());
    EXPECT_FALSE(ifStmt->getElseBlock().has_value());

    const auto& cond0 = expectLogicalExpr(ifStmt->getCondition(), Token::Type::And);

    const auto& indexEq = expectBinaryExpr(cond0.getRight(), Token::Type::Equal);
    expectVariableExpr(indexEq.getLeft(), "index");
    expectLiteralValue<std::string>(indexEq.getRight(), Token::Type::String, " ");

    const auto& cond1 = expectLogicalExpr(cond0.getLeft(), Token::Type::And);

    const auto& xGroup = expectLogicalExpr(cond1.getLeft(), Token::Type::And);
    const auto& xGt = expectBinaryExpr(xGroup.getLeft(), Token::Type::Greater);
    expectVariableExpr(xGt.getLeft(), "x");
    expectLiteralValue<std::int32_t>(xGt.getRight(), Token::Type::Int, 0);

    const auto& xLt = expectBinaryExpr(xGroup.getRight(), Token::Type::Less);
    expectVariableExpr(xLt.getLeft(), "x");
    expectLiteralValue<std::int32_t>(xLt.getRight(), Token::Type::Int, 4);

    const auto& yGroup = expectLogicalExpr(cond1.getRight(), Token::Type::And);
    const auto& yGt = expectBinaryExpr(yGroup.getLeft(), Token::Type::Greater);
    expectVariableExpr(yGt.getLeft(), "y");
    expectLiteralValue<std::int32_t>(yGt.getRight(), Token::Type::Int, 0);

    const auto& yLt = expectBinaryExpr(yGroup.getRight(), Token::Type::Less);
    expectVariableExpr(yLt.getLeft(), "y");
    expectLiteralValue<std::int32_t>(yLt.getRight(), Token::Type::Int, 4);

    const auto& thenBlock = expectCast<BlockStmt>(ifStmt->getThenBlock());
    ASSERT_EQ(thenBlock.getStatements().size(), 1);
    
    const auto& ret = expectReturn(thenBlock, 0);
    expectLiteralValue<std::int32_t>(ret.getValue()->get(), Token::Type::Int, 1);
}

TEST_F(ParserTests, Snapshot_IgnoresSingleLineComments) {
    parseOk(
        "psst: ignore this line\n"
        "stash x about 1..."
    );
    expectNumberOfStatements(1ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (x)",
        "initializer:",
        "LiteralExpr Int (1)",
    });
}

TEST_F(ParserTests, Snapshot_IgnoresBlockComments) {
    parseOk(
        "stash x about 1...\n"
        "rant_stop\n"
        "ignored text\n"
        "rant_start\n"
        "stash y about 2..."
    );
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (x)",
        "LiteralExpr Int (1)",
        "VarDefinitionStmt Ident (y)",
        "LiteralExpr Int (2)",
    });
}

TEST_F(ParserTests, Structural_ReturnWithPlusMinusNesting) {
    parseOk(
        "gig calculate_stuff(x, y) {\n"
        "    yeet 2 with 2 without 2...\n"
        "}\n"
    );
    expectNumberOfStatements(1ull);

    const auto& fn = expectFunction(0, "calculate_stuff");
    const auto& body = expectCast<BlockStmt>(fn.getBody());
    ASSERT_EQ(body.getStatements().size(), 1);

    const auto& ret = expectReturn(body, 0);
    const auto& minus = expectBinaryExpr(ret.getValue()->get(), Token::Type::Minus);
    const auto& plus = expectBinaryExpr(minus.getLeft(), Token::Type::Plus);

    expectLiteralValue<std::int32_t>(plus.getLeft(), Token::Type::Int, 2);
    expectLiteralValue<std::int32_t>(plus.getRight(), Token::Type::Int, 2);
    expectLiteralValue<std::int32_t>(minus.getRight(), Token::Type::Int, 2);
}

TEST_F(ParserTests, Structural_ExpressionStmtWithMultiply) {
    parseOk("2 times 3...");
    expectNumberOfStatements(1ull);

    const auto* exprStmt = expectStmt<ExpressionStmt>(0);
    const auto& mul = expectBinaryExpr(exprStmt->getExpression(), Token::Type::Multiply);
    expectLiteralValue<std::int32_t>(mul.getLeft(), Token::Type::Int, 2);
    expectLiteralValue<std::int32_t>(mul.getRight(), Token::Type::Int, 3);
}

TEST_F(ParserTests, Structural_ExpressionStmtWithDivide) {
    parseOk("8 over 4...");
    expectNumberOfStatements(1ull);

    const auto* exprStmt = expectStmt<ExpressionStmt>(0);
    const auto& div = expectBinaryExpr(exprStmt->getExpression(), Token::Type::Divide);
    expectLiteralValue<std::int32_t>(div.getLeft(), Token::Type::Int, 8);
    expectLiteralValue<std::int32_t>(div.getRight(), Token::Type::Int, 4);
}

TEST_F(ParserTests, Structural_ReturnWithMixedPlusMinusMultiplyDividePrecedence) {
    parseOk(
        "gig calculate_stuff(x, y) {\n"
        "    yeet 1 with x times 3 without 4 over y...\n"
        "}\n"
    );
    expectNumberOfStatements(1ull);

    const auto& fn = expectFunction(0, "calculate_stuff");
    const auto& body = expectCast<BlockStmt>(fn.getBody());
    ASSERT_EQ(body.getStatements().size(), 1);

    const auto& ret = expectReturn(body, 0);

    const auto& minus = expectBinaryExpr(ret.getValue()->get(), Token::Type::Minus);
    const auto& plus = expectBinaryExpr(minus.getLeft(), Token::Type::Plus);
    const auto& mul = expectBinaryExpr(plus.getRight(), Token::Type::Multiply);
    const auto& div = expectBinaryExpr(minus.getRight(), Token::Type::Divide);

    expectLiteralValue<std::int32_t>(plus.getLeft(), Token::Type::Int, 1);
    expectVariableExpr(mul.getLeft(), "x");
    expectLiteralValue<std::int32_t>(mul.getRight(), Token::Type::Int, 3);
    expectLiteralValue<std::int32_t>(div.getLeft(), Token::Type::Int, 4);
    expectVariableExpr(div.getRight(), "y");
}

TEST_F(ParserTests, Snapshot_SpinAroundWithVariableCount) {
    parseOk(
        "stash n about 3...\n"
        "spin_around (n) { yeet 1... }"
    );
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (n)",
        "initializer:",
        "LiteralExpr Int (3)",
        "RepeatStmt",
        "count:",
        "VariableExpr Ident (n)",
        "body:",
        "BlockStmt",
        "ReturnStmt",
        "LiteralExpr Int (1)",
    });
}

TEST_F(ParserTests, Snapshot_DoUntilBoredWithPumpItAndInnerIfBreak) {
    parseOk(R"(
        stash counter about 0...
        do_until_bored {
            gossip.spill_tea(counter)...
            pump_it counter...
            perhaps (counter bigger_ish 3) {
                rage_quit!!!
            }
        }
    )");
    expectNumberOfStatements(2ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (counter)",
        "LiteralExpr Int (0)",
        "LoopStmt",
        "BlockStmt",
        "ExpressionStmt",
        "CallExpr",
        "DotExpr",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "VariableExpr Ident (counter)",
        "UnaryExpr Incr",
        "IfStmt",
        "BinaryExpr Greater",
        "LiteralExpr Int (3)",
        "BreakStmt",
    });
}

TEST_F(ParserTests, Snapshot_IfElseChainWithPrintCalls) {
    parseOk(R"(
        stash number about 10...
        stash isNumberTen about number looks_like 11...

        perhaps (isNumberTen looks_like totally) {
            gossip.spill_tea("TEN")...
        }
        or_whatever (isNumberTen looks_like nah) {
            gossip.spill_tea("NOT TEN")...
        }
        screw_it {
            gossip.spill_tea("WAT")...
        }
    )");
    expectNumberOfStatements(3ull);

    expectPrintedInOrder({
        "VarDefinitionStmt Ident (number)",
        "LiteralExpr Int (10)",
        "VarDefinitionStmt Ident (isNumberTen)",
        "BinaryExpr Equal",
        "VariableExpr Ident (number)",
        "LiteralExpr Int (11)",
        "IfStmt",
        "condition:",
        "BinaryExpr Equal",
        "VariableExpr Ident (isNumberTen)",
        "LiteralExpr True",
        "then:",
        "ExpressionStmt",
        "LiteralExpr String (TEN)",
        "ElseIf",
        "LiteralExpr False",
        "body:",
        "ExpressionStmt",
        "LiteralExpr String (NOT TEN)",
        "else:",
        "ExpressionStmt",
        "LiteralExpr String (WAT)",
    });
}

TEST_F(ParserTests, Structural_FloatComparisonsInIfElseChain) {
    parseOk(R"(
        stash floatingNumber about 11.0...
        perhaps (floatingNumber looks_like 10.0) {
            yeet ghosted...
        }
        or_whatever (floatingNumber kinda_sus 20.0) {
            yeet ghosted...
        }
        or_whatever (floatingNumber tiny_ish 5.0) {
            yeet ghosted...
        }
        screw_it {
            yeet ghosted...
        }
    )");
    expectNumberOfStatements(2ull);

    const auto* ifStmt = expectStmt<IfStmt>(1);
    ASSERT_EQ(ifStmt->getElseIfClauses().size(), 2);
    ASSERT_TRUE(ifStmt->getElseBlock().has_value());

    auto assertFloatComparison = [&](const Expr& expr, Token::Type op, double rhs) {
        const auto& bin = expectBinaryExpr(expr, op);
        expectVariableExpr(bin.getLeft(), "floatingNumber");
        expectLiteralValue<double>(bin.getRight(), Token::Type::Float, rhs);
    };

    assertFloatComparison(ifStmt->getCondition(), Token::Type::Equal, 10.0);
    assertFloatComparison(*ifStmt->getElseIfClauses()[0].condition, Token::Type::NotEqual, 20.0);
    assertFloatComparison(*ifStmt->getElseIfClauses()[1].condition, Token::Type::Less, 5.0);

    const auto& thenBlock = expectCast<BlockStmt>(ifStmt->getThenBlock());
    ASSERT_EQ(thenBlock.getStatements().size(), 1);
    
    const auto& thenRet = expectReturn(thenBlock, 0);
    expectLiteral(thenRet.getValue()->get(), Token::Type::Null);
}

TEST_F(ParserTests, Failure_MysteryStatement_OnUnexpectedToken) {
    const auto result = parse(",");
    expectHasErrorContaining(result, "Mystery statement");
}

TEST_F(ParserTests, Failure_Break_RequiresBangSemi) {
    const auto result = parse("do_until_bored { rage_quit... }");
    expectHasErrorContaining(result, "Expected '!!!' after 'rage_quit'");
}

TEST_F(ParserTests, Failure_ExpressionStmt_RequiresEllipsis) {
    const auto result = parse("gossip.spill_tea(1) stash x about 1...");
    expectHasErrorContaining(result, "Expected '...' after expression");
}

TEST_F(ParserTests, Failure_Function_RequiresNameAfterGig) {
    const auto result = parse("gig (x) { yeet 1... }");
    expectHasErrorContaining(result, "Expected function name after 'gig'");
}

TEST_F(ParserTests, Failure_Function_RequiresLeftParenAfterName) {
    const auto result = parse("gig macho { yeet 1... }");
    expectHasErrorContaining(result, "Expected '(' after function name");
}

TEST_F(ParserTests, Failure_Function_RequiresRightParenAfterParameters) {
    const auto result = parse("gig macho(x y) { yeet 1... }");
    expectHasErrorContaining(result, "Expected ')' after function parameters");
}

TEST_F(ParserTests, Failure_Function_RequiresParameterNameAfterComma) {
    const auto result = parse("gig macho(x,) { yeet 1... }");
    expectHasErrorContaining(result, "Expected function parameter name");
}

TEST_F(ParserTests, Failure_Function_RequiresBlockAfterSignature) {
    const auto result = parse("gig macho() yeet 1...");
    expectHasErrorContaining(result, "Expected '{' opening block");
}

TEST_F(ParserTests, Failure_VarDefinition_RequiresNameAfterStash) {
    const auto result = parse("stash about 1...");
    expectHasErrorContaining(result, "Expected variable name after 'stash'");
}

TEST_F(ParserTests, Failure_VarDefinition_RequiresEllipsisAfterDefinition) {
    const auto result = parse("stash x about 1 stash y about 2...");
    expectHasErrorContaining(result, "Expected '...' after variable definition");
}

TEST_F(ParserTests, Failure_Return_RequiresExpressionWhenNotImplicitNull) {
    const auto result = parse("gig macho() { yeet } stash x about 1...");
    expectHasErrorContaining(result, "Expected an expression after 'yeet'");
}

TEST_F(ParserTests, Failure_Return_RequiresEllipsisAfterValue) {
    const auto result = parse("gig macho() { yeet 1 } stash x about 1...");
    expectHasErrorContaining(result, "Expected '...' after return value");
}

TEST_F(ParserTests, Failure_If_RequiresLeftParenAfterPerhaps) {
    const auto result = parse("perhaps totally) { yeet 1... }");
    expectHasErrorContaining(result, "Expected '(' after 'perhaps'");
}

TEST_F(ParserTests, Failure_If_RequiresRightParenAfterCondition) {
    const auto result = parse("perhaps (totally { yeet 1... }");
    expectHasErrorContaining(result, "Expected ')' after 'perhaps' condition");
}

TEST_F(ParserTests, Failure_Elif_RequiresLeftParenAfterOrWhatever) {
    const auto result = parse("perhaps (totally) { yeet 1... } or_whatever totally) { yeet 2... }");
    expectHasErrorContaining(result, "Expected '(' after 'or_whatever'");
}

TEST_F(ParserTests, Failure_Elif_RequiresRightParenAfterCondition) {
    const auto result = parse("perhaps (totally) { yeet 1... } or_whatever (nah { yeet 2... }");
    expectHasErrorContaining(result, "Expected ')' after 'or_whatever' condition");
}

TEST_F(ParserTests, Failure_Repeat_RequiresLeftParen) {
    const auto result = parse("spin_around 5) { yeet 1... }");
    expectHasErrorContaining(result, "Expected '(' after 'repeat'");
}

TEST_F(ParserTests, Failure_Repeat_RequiresCountExpressionInsideParens) {
    const auto result = parse("spin_around () { yeet 1... }");
    expectHasErrorContaining(result, "Expected loop count expression inside '()'");
}

TEST_F(ParserTests, Failure_Repeat_RequiresRightParenAfterCountExpression) {
    const auto result = parse("spin_around (5 { yeet 1... }");
    expectHasErrorContaining(result, "Expected ')' after loop count expression");
}

TEST_F(ParserTests, Failure_Import_RequiresModuleName) {
    const auto result = parse("summon ...");
    expectHasErrorContaining(result, "Expected module name after 'summon'");
}

TEST_F(ParserTests, Failure_Import_RequiresEllipsis) {
    const auto result = parse("summon gossip stash x about 1...");
    expectHasErrorContaining(result, "Expected '...' after module import");
}

TEST_F(ParserTests, Failure_Reassign_RequiresEllipsis) {
    const auto result = parse("counter might_be 1 stash x about 1...");
    expectHasErrorContaining(result, "Expected '...' after reassignment");
}

TEST_F(ParserTests, Failure_BinaryExpr_RequiresRightOperand_Term) {
    const auto result = parse("stash x about 1 with ...");
    expectHasErrorContaining(result, "Expected right operand after");
}

TEST_F(ParserTests, Failure_LogicalExpr_RequiresRightOperand) {
    const auto result = parse("stash x about totally also ...");
    expectHasErrorContaining(result, "Expected right operand after");
}

TEST_F(ParserTests, Failure_EqualityExpr_RequiresRightOperand) {
    const auto result = parse("stash x about 1 looks_like ...");
    expectHasErrorContaining(result, "Expected right operand after");
}

TEST_F(ParserTests, Failure_ComparisonExpr_RequiresRightOperand) {
    const auto result = parse("stash x about 1 bigger_ish ...");
    expectHasErrorContaining(result, "Expected right operand after");
}

TEST_F(ParserTests, Failure_DotExpr_RequiresPropertyNameAfterDot) {
    const auto result = parse("gossip.(1)...");
    expectHasErrorContaining(result, "Expected property name after '.'");
}

TEST_F(ParserTests, Failure_GroupedExpr_RequiresClosingParen) {
    const auto result = parse("stash x about (1 with 2...\nstash y about 3...");
    expectHasErrorContaining(result, "Expected ) after expression");
}

TEST_F(ParserTests, Failure_VectorExpr_RequiresClosingBracket) {
    const auto result = parse("stash xs about [1, 2...\nstash y about 3...");
    expectHasErrorContaining(result, "Expected ']' at the end of a vector");
}

TEST_F(ParserTests, Failure_CallExpr_RequiresClosingParen) {
    const auto result = parse("print(1, 2... stash x about 1...");
    expectHasErrorContaining(result, "Expected ')' after arguments");
}

TEST_F(ParserTests, Failure_ParserInvariant_ConsumeIdentRequiresStringValue) {
    std::vector<Token> tokens;
    tokens.emplace_back(Token::Type::Func, 0ull, 1u, 1u);
    tokens.emplace_back(Token::Type::Ident, 0ull, 1u, 5u, std::int32_t{123});
    tokens.emplace_back(Token::Type::LParen, 0ull, 1u, 6u);
    tokens.emplace_back(Token::Type::RParen, 0ull, 1u, 7u);
    tokens.emplace_back(Token::Type::LBrace, 0ull, 1u, 9u);
    tokens.emplace_back(Token::Type::RBrace, 0ull, 1u, 10u);

    Parser parser{std::move(tokens)};
    const auto result = parser.parse();
    expectHasErrorContaining(result, "Not yapping when supposed to");
}

TEST_F(ParserTests, Failure_ParserInvariant_IdentTokenRequiresStringValueInPrimary) {
    std::vector<Token> tokens;
    tokens.emplace_back(Token::Type::Ident, 0ull, 1u, 1u, std::int32_t{42});
    tokens.emplace_back(Token::Type::Semi, 0ull, 1u, 2u);

    Parser parser{std::move(tokens)};
    const auto result = parser.parse();
    expectHasErrorContaining(result, "This is the moment to start yapping");
}

TEST_F(ParserTests, Failure_MultipleErrors_TwoBadTerms) {
    const auto result = parse("stash x about 1 with ...\nstash y about 2 without ...");
    expectErrorMessagesEq(result, {
        "Expected right operand after 'with'",
        "Expected right operand after 'without'",
    });
}

TEST_F(ParserTests, Failure_MultipleErrors_MixedIfAndVar) {
    const auto result = parse(
        "perhaps totally) { yeet 1... }\n"
        "stash x 1..."
    );

    expectErrorMessagesEq(result, {
        "Expected '(' after 'perhaps'",
        "Expected '...' after variable definition",
    });
}

TEST_F(ParserTests, Failure_SynchronizeCanSkipNextError) {
    const auto result = parse("gossip.spill_tea(1) stash about 1...");
    ASSERT_EQ(result.errors.size(), 1u) << joinErrorMessages(result.errors);
    expectHasErrorContaining(result, "Expected '...' after expression");
    expectNoErrorContaining(result, "Expected variable name after 'stash'");
}

TEST_F(ParserTests, Failure_IncrOperatorThrowsWhenCalledWithRvalue) {
    const auto result = parse("pump_it 1...");
    expectHasErrorContaining(result, "Can't pump_it - no grip, no glory.");
}

TEST_F(ParserTests, Failure_IncrOperatorThrowsWhenCalledWithFunctionCall) {
    const auto result = parse(
        "gig print(x) { gossip.spill_tea(x)... }\n"
        "pump_it print(1)..."
    );
    expectHasErrorContaining(result, "Can't pump_it - no grip, no glory.");
}

TEST_F(ParserTests, Failure_IncrOperatorThrowsWhenCalledWithStringLiteral) {
    const auto result = parse("pump_it \"hello\"...");
    expectHasErrorContaining(result, "Can't pump_it - no grip, no glory.");
}

TEST_F(ParserTests, Failure_IncrOperatorThrowsWhenCalledWithDotExpression) {
    const auto result = parse(
        "summon gossip...\n"
        "pump_it gossip.spill_tea..."
    );
    expectHasErrorContaining(result, "Can't pump_it - no grip, no glory.");
}
