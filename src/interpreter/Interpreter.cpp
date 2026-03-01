#include "Interpreter.hpp"

#include <iostream>
#include "utils/Logging.hpp"

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmt>> statements)
    : _statements{std::move(statements)} {
}

void Interpreter::interpret() {
    LOG_DEBUG << std::format("Starting interpretation of {} statements", _statements.size());
    for (const auto& stmt : _statements) {
        stmt->accept(*this);
    }

    LOG_DEBUG << "Looking for entry point 'macho'";
    auto it = _functions.find("macho");
    if (it != _functions.end()) {
        LOG_DEBUG << "Executing 'macho' function";
        it->second.getBody().accept(*this);
    }
    else {
        LOG_ERROR << "Missing entry point 'gig macho()'";
        throw std::runtime_error("Missing entry point 'gig macho()'");
    }
    LOG_DEBUG << "Interpretation completed";
}

RuntimeValue Interpreter::visitVarDefinitionStmt(const VarDefinitionStmt& stmt) {
    LOG_DEBUG << "Visiting VarDefinitionStmt";
    return std::monostate{};
}

RuntimeValue Interpreter::visitAssignStmt(const AssignStmt& stmt) {
    LOG_DEBUG << "Visiting AssignStmt";
    return std::monostate{};
}

RuntimeValue Interpreter::visitBlockStmt(const BlockStmt& stmt) {
    LOG_DEBUG << "Visiting BlockStmt with " << stmt.getStatements().size() << " statements";
    for (const auto& innerStmt : stmt.getStatements()) {
        innerStmt->accept(*this);
    }
    return std::monostate{};
}

RuntimeValue Interpreter::visitIfStmt(const IfStmt& stmt) {
    LOG_DEBUG << "Visiting IfStmt";
    return std::monostate{};
}

RuntimeValue Interpreter::visitLoopStmt(const LoopStmt& stmt) {
    LOG_DEBUG << "Visiting LoopStmt";
    return std::monostate{};
}

RuntimeValue Interpreter::visitRepeatStmt(const RepeatStmt& stmt) {
    LOG_DEBUG << "Visiting RepeatStmt";
    return std::monostate{};
}

RuntimeValue Interpreter::visitPrintStmt(const PrintStmt& stmt) {
    LOG_DEBUG << "Visiting PrintStmt";
    const auto value = stmt.getExpression().accept(*this);

    auto str = std::holds_alternative<std::monostate>(value) ?
        std::string("ghosted") : std::get<std::string>(value);
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    LOG_DEBUG << "Printing: " << str;
    std::cout << str << "!!!" << std::endl;
    
    return std::monostate{};
}

RuntimeValue Interpreter::visitReturnStmt(const ReturnStmt& stmt) {
    LOG_DEBUG << "Visiting ReturnStmt";
    //TODO for now - just null
    return std::monostate{};
}

RuntimeValue Interpreter::visitBreakStmt(const BreakStmt& stmt) {
    LOG_DEBUG << "Visiting BreakStmt";
    return std::monostate{};
}

RuntimeValue Interpreter::visitFunctionStmt(const FunctionStmt& stmt) {
    LOG_DEBUG << "Visiting FunctionStmt";
    auto funcName = stmt.getName().getValue<std::string>();
    LOG_DEBUG << "Registering function: " << funcName;
    _functions.emplace(funcName, stmt);
    return std::monostate{};
}

RuntimeValue Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    LOG_DEBUG << "Visiting ExpressionStmt";
    return std::monostate{};
}

    
RuntimeValue Interpreter::visitLiteralExpr(const LiteralExpr& expr) {
    LOG_DEBUG << "Visiting LiteralExpr";
    if (expr.getLiteral().getType() == Token::Type::String) {
        auto value = expr.getLiteral().getValue<std::string>();
        LOG_DEBUG << "String literal: " << value;
        return value;
    }
    return std::monostate{};
}

RuntimeValue Interpreter::visitVariableExpr(const VariableExpr& expr) {
    LOG_DEBUG << "Visiting VariableExpr";
    return std::monostate{};
}

RuntimeValue Interpreter::visitBinaryExpr(const BinaryExpr& expr) {
    LOG_DEBUG << "Visiting BinaryExpr";
    return std::monostate{};
}

RuntimeValue Interpreter::visitUnaryExpr(const UnaryExpr& expr) {
    LOG_DEBUG << "Visiting UnaryExpr";
    return std::monostate{};
}

RuntimeValue Interpreter::visitCallExpr(const CallExpr& expr) {
    LOG_DEBUG << "Visiting CallExpr";
    return std::monostate{};
}
