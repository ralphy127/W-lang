#include "utils/AstPrinter.hpp"

#include <iostream>
#include <ostream>
#include <string>
#include <format>
#include "ast/Expressions.hpp"
#include "ast/Statements.hpp"
#include "token/Token.hpp"

namespace {

std::string tokenValueToString(const Token& token) {
    if (token.valueIs<std::int32_t>()) {
        return std::to_string(token.getValue<std::int32_t>());
    }
    if (token.valueIs<double>()) {
        return std::to_string(token.getValue<double>());
    }
    if (token.valueIs<std::string>()) {
        return token.getValue<std::string>();
    }
    return "";
}

std::string tokenToString(const Token& token) {
    const auto typeStr = toString(token.getType());
    const auto valueStr = tokenValueToString(token);
    if (valueStr.empty()) {
        return typeStr;
    }

    return std::format("{} ({})", typeStr, valueStr);
}

}

AstPrinter::AstPrinter() 
    : _out{std::cout} {}

AstPrinter::AstPrinter(std::ostream& out)
    : _out{out} {}

void AstPrinter::print(const std::vector<std::unique_ptr<Stmt>>& stmts) {
    for (const auto& stmt : stmts) {
        stmt->accept(*this);
    }
}

RuntimeValue AstPrinter::visitVarDefinitionStmt(const VarDefinitionStmt& stmt) {
    printLine("VarDefinitionStmt " + tokenToString(stmt.getName()));
    printKey("initializer");
    ++_indent;
    stmt.getInitializer().accept(*this);
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitReassignStmt(const ReassignStmt& stmt) {
    printLine("ReassignStmt " + tokenToString(stmt.getName()));
    printKey("value");
    ++_indent;
    stmt.getValue().accept(*this);
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitBlockStmt(const BlockStmt& stmt) {
    printLine("BlockStmt");
    ++_indent;
    for (const auto& s : stmt.getStatements()) {
        s->accept(*this);
    }
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitIfStmt(const IfStmt& stmt) {
    printLine("IfStmt");
    printKey("condition");
    ++_indent;
    stmt.getCondition().accept(*this);
    --_indent;

    printKey("then");
    ++_indent;
    stmt.getThenBlock().accept(*this);
    --_indent;

    for (const auto& clause : stmt.getElseIfClauses()) {
        printLine("ElseIf");

        printKey("condition");
        _indent += 2;
        clause.condition->accept(*this);
        _indent -= 2;

        printKey("body");
        _indent += 2;
        clause.body->accept(*this);
        _indent -= 2;
    }

    if (auto elseBlock = stmt.getElseBlock()) {
        printKey("else");
        ++_indent;
        elseBlock->get().accept(*this);
        --_indent;
    }

    return Null{};
}

RuntimeValue AstPrinter::visitLoopStmt(const LoopStmt& stmt) {
    printLine("LoopStmt");
    ++_indent;
    stmt.getBody().accept(*this);
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitRepeatStmt(const RepeatStmt& stmt) {
    printLine("RepeatStmt");

    printKey("count");
    ++_indent;
    stmt.getCount().accept(*this);
    --_indent;

    printKey("body");
    ++_indent;
    stmt.getBody().accept(*this);
    --_indent;

    return Null{};
}

RuntimeValue AstPrinter::visitReturnStmt(const ReturnStmt& stmt) {
    printLine("ReturnStmt");
    if (auto returnValue = stmt.getValue()) {
        ++_indent;
        returnValue->get().accept(*this);
        --_indent;
    }
    return Null{};
}

RuntimeValue AstPrinter::visitBreakStmt(const BreakStmt&) {
    printLine("BreakStmt");
    return Null{};
}

RuntimeValue AstPrinter::visitFunctionStmt(const FunctionStmt& stmt) {
    printLine("FunctionStmt " + tokenToString(stmt.getName()));

    printIndent();
    _out << "params: ";
    const auto& params = stmt.getParameters();
    for (std::uint32_t i{0u}; i < params.size(); ++i) {
        if (i > 0) {
            _out << ", ";
        }
        _out << tokenToString(params[i]);
    }
    _out << "\n";

    printKey("body");
    ++_indent;
    stmt.getBody().accept(*this);
    --_indent;

    return Null{};
}

RuntimeValue AstPrinter::visitExpressionStmt(const ExpressionStmt& stmt) {
    printLine("ExpressionStmt");
    ++_indent;
    stmt.getExpression().accept(*this);
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitImportStmt(const ImportStmt& stmt) {
    printLine("ImportStmt " + tokenToString(stmt.getModuleName()));
    return Null{};
}

RuntimeValue AstPrinter::visitLiteralExpr(const LiteralExpr& expr) {
    printLine("LiteralExpr " + tokenToString(expr.getLiteral()));
    return Null{};
}

RuntimeValue AstPrinter::visitVariableExpr(const VariableExpr& expr) {
    printLine("VariableExpr " + tokenToString(expr.getName()));
    return Null{};
}

RuntimeValue AstPrinter::visitBinaryExpr(const BinaryExpr& expr) {
    printLine("BinaryExpr " + tokenToString(expr.getOperator()));

    printKey("left");
    ++_indent;
    expr.getLeft().accept(*this);
    --_indent;

    printKey("right");
    ++_indent;
    expr.getRight().accept(*this);
    --_indent;

    return Null{};
}

RuntimeValue AstPrinter::visitUnaryExpr(const UnaryExpr& expr) {
    printLine("UnaryExpr " + tokenToString(expr.getOperator()));
    ++_indent;
    expr.getRight().accept(*this);
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitCallExpr(const CallExpr& expr) {
    printLine("CallExpr");

    printKey("callee");
    ++_indent;
    expr.getCallee().accept(*this);
    --_indent;

    printKey("args");
    ++_indent;
    for (const auto& arg : expr.getArgs()) {
        arg->accept(*this);
    }
    --_indent;

    return Null{};
}

RuntimeValue AstPrinter::visitDotExpr(const DotExpr& expr) {
    printLine("DotExpr");

    printKey("left");
    ++_indent;
    expr.getLeft().accept(*this);
    --_indent;

    printKey("right " + tokenToString(expr.getRight()));

    return Null{};
}

RuntimeValue AstPrinter::visitVectorExpr(const VectorExpr& expr) {
    printLine("VectorExpr");
    ++_indent;
    for (const auto& init : expr.getInitializers()) {
        init->accept(*this);
    }
    --_indent;
    return Null{};
}

RuntimeValue AstPrinter::visitLogicalExpr(const LogicalExpr& expr) {
    printLine("LogicalExpr " + tokenToString(expr.getOperator()));

    printKey("left");
    ++_indent;
    expr.getLeft().accept(*this);
    --_indent;

    printKey("right");
    ++_indent;
    expr.getRight().accept(*this);
    --_indent;

    return Null{};
}

void AstPrinter::printIndent() {
    for (std::uint32_t i{0u}; i < _indent; ++i) {
        _out << "  ";
    }
}

void AstPrinter::printLine(std::string_view text) {
    printIndent();
    _out << text << "\n";
}

void AstPrinter::printKey(std::string_view key) {
    printIndent();
    _out << key << ":\n";
}
