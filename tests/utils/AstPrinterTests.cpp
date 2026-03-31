#include <gtest/gtest.h>

#include <initializer_list>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include "lexer/Lexer.hpp"
#include "parser/Parser.hpp"
#include "utils/AstPrinter.hpp"

struct AstPrinterTests : public ::testing::Test {
    static void expectInOrder(
        const std::string& output,
        std::initializer_list<std::string_view> needles) {

        std::size_t pos = 0;
        for (const auto needle : needles) {
            const auto found = output.find(needle, pos);
            ASSERT_NE(found, std::string::npos) << "Missing (after pos=" << pos << "): " << needle;
            pos = found + needle.size();
        }
    }

    static std::vector<std::unique_ptr<Stmt>> parseToAst(const std::string& source) {
        constexpr FileId dummyFileId{0ull};

        Lexer lexer{source, dummyFileId};
        auto lexerResult = lexer.tokenize();
        EXPECT_TRUE(lexerResult.errors.empty());
        if (!lexerResult.errors.empty()) {
            return {};
        }

        Parser parser{std::move(lexerResult.tokens)};
        auto parserResult = parser.parse();
        EXPECT_TRUE(parserResult.errors.empty());
        if (!parserResult.errors.empty()) {
            return {};
        }

        return std::move(parserResult.statements);
    }

    static std::string printSource(const std::string& source) {
        auto ast = parseToAst(source);

        std::ostringstream out;
        AstPrinter printer{out};
        printer.print(ast);
        return out.str();
    }
};

TEST_F(AstPrinterTests, EmptyAstDoesNotPrintAnything) {
    std::vector<std::unique_ptr<Stmt>> ast{};
    std::ostringstream out;
    AstPrinter printer{out};
    printer.print(ast);
    EXPECT_TRUE(out.str().empty());
}

TEST_F(AstPrinterTests, VisitImportStmt_PrintsModuleName) {
    expectInOrder(printSource("summon gossip..."), {
        "ImportStmt Ident (gossip)",
    });
}

TEST_F(AstPrinterTests, VisitVarDefinitionStmt_PrintsInitializer) {
    expectInOrder(printSource("stash x about 1..."), {
        "VarDefinitionStmt Ident (x)",
        "initializer:",
        "LiteralExpr Int (1)",
    });
}

TEST_F(AstPrinterTests, VisitReassignStmt_PrintsValue) {
    expectInOrder(printSource("counter might_be 42..."), {
        "ReassignStmt Ident (counter)",
        "value:",
        "LiteralExpr Int (42)",
    });
}

TEST_F(AstPrinterTests, VisitExpressionStmt_CallDot_FloatLiteral) {
    expectInOrder(printSource("gossip.spill_tea(42.42)..."), {
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExpr",
        "left:",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "LiteralExpr Float (42.42",
    });
}

TEST_F(AstPrinterTests, VisitExpressionStmt_CallDot_StringLiteral) {
    expectInOrder(printSource("gossip.spill_tea(\"Hello\")..."), {
        "ExpressionStmt",
        "CallExpr",
        "callee:",
        "DotExpr",
        "left:",
        "VariableExpr Ident (gossip)",
        "right Ident (spill_tea)",
        "args:",
        "LiteralExpr String (Hello)",
    });
}

TEST_F(AstPrinterTests, VisitUnaryExpr_Incr) {
    expectInOrder(printSource("pump_it counter..."), {
        "ExpressionStmt",
        "UnaryExpr Incr",
        "VariableExpr Ident (counter)",
    });
}

TEST_F(AstPrinterTests, VisitBinaryExpr_Plus) {
    expectInOrder(printSource("stash result about 1 with 2..."), {
        "VarDefinitionStmt Ident (result)",
        "initializer:",
        "BinaryExpr Plus",
        "left:",
        "LiteralExpr Int (1)",
        "right:",
        "LiteralExpr Int (2)",
    });
}

TEST_F(AstPrinterTests, VisitLogicalExpr_And) {
    expectInOrder(printSource("stash result about totally also nah..."), {
        "VarDefinitionStmt Ident (result)",
        "initializer:",
        "LogicalExpr And",
        "left:",
        "LiteralExpr True",
        "right:",
        "LiteralExpr False",
    });
}

TEST_F(AstPrinterTests, VisitLogicalExpr_Or) {
    expectInOrder(printSource("stash result about nah either totally..."), {
        "VarDefinitionStmt Ident (result)",
        "initializer:",
        "LogicalExpr Or",
        "left:",
        "LiteralExpr False",
        "right:",
        "LiteralExpr True",
    });
}

TEST_F(AstPrinterTests, VisitVectorExpr_PrintsElements) {
    expectInOrder(printSource("stash list about [11, x, 33]..."), {
        "VarDefinitionStmt Ident (list)",
        "initializer:",
        "VectorExpr",
        "LiteralExpr Int (11)",
        "VariableExpr Ident (x)",
        "LiteralExpr Int (33)",
    });
}

TEST_F(AstPrinterTests, VisitIfStmt_SimpleThenBlock) {
    expectInOrder(printSource("perhaps (totally) { stash x about 1... }"), {
        "IfStmt",
        "condition:",
        "LiteralExpr True",
        "then:",
        "BlockStmt",
        "VarDefinitionStmt Ident (x)",
    });
}

TEST_F(AstPrinterTests, VisitIfStmt_ElseIfAndElse) {
    const std::string source =
        "perhaps (1) { stash a about 1... }\n"
        "or_whatever (2) { stash b about 2... }\n"
        "screw_it { stash c about 3... }\n";

    expectInOrder(printSource(source), {
        "IfStmt",
        "LiteralExpr Int (1)",
        "VarDefinitionStmt Ident (a)",
        "ElseIf",
        "LiteralExpr Int (2)",
        "VarDefinitionStmt Ident (b)",
        "else:",
        "VarDefinitionStmt Ident (c)",
    });
}

TEST_F(AstPrinterTests, VisitLoopStmt_AndBreakStmt) {
    expectInOrder(printSource("do_until_bored { rage_quit!!! }"), {
        "LoopStmt",
        "BlockStmt",
        "BreakStmt",
    });
}

TEST_F(AstPrinterTests, VisitRepeatStmt_CountAndBody) {
    expectInOrder(printSource("spin_around (5) { pump_it counter... }"), {
        "RepeatStmt",
        "count:",
        "LiteralExpr Int (5)",
        "body:",
        "BlockStmt",
        "UnaryExpr Incr",
    });
}

TEST_F(AstPrinterTests, VisitFunctionStmt_ParamsAndReturnValue) {
    const std::string source = "gig foo(x, y) { yeet x with y... }";
    const auto out = printSource(source);

    ASSERT_NE(out.find("params: Ident (x), Ident (y)"), std::string::npos);
    expectInOrder(out, {
        "FunctionStmt Ident (foo)",
        "body:",
        "BlockStmt",
        "ReturnStmt",
        "BinaryExpr Plus",
        "VariableExpr Ident (x)",
        "VariableExpr Ident (y)",
    });
}

TEST_F(AstPrinterTests, VisitReturnStmt_ImplicitNull) {
    expectInOrder(printSource("gig foo() { yeet... }"), {
        "FunctionStmt Ident (foo)",
        "ReturnStmt",
        "LiteralExpr Null",
    });
}
