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

    const auto& name = stmt.getName().getValue<std::string>();
    auto value = stmt.getInitializer().accept(*this);
    LOG_DEBUG << std::format("Defining variable {} with {} at scope depth {}",
        name, stringify(value), _scopeDepth);
    _currentEnvironment->defineVar(name, std::move(value));

    return std::monostate{};
}

RuntimeValue Interpreter::visitReassignStmt(const ReassignStmt& stmt) {
    LOG_DEBUG << "Visiting ReassignStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    auto newValue = stmt.getValue().accept(*this);
    _currentEnvironment->reassignVar(name, std::move(newValue));

    LOG_DEBUG << std::format("Reassigning variable {} to {} at scope depth {}",
        name, stringify(newValue), _scopeDepth);

    return std::monostate{};
}

RuntimeValue Interpreter::visitBlockStmt(const BlockStmt& stmt) {
    LOG_DEBUG << "Visiting BlockStmt with " << stmt.getStatements().size() << " statements";
    
    auto previousEnvironment = _currentEnvironment;
    _currentEnvironment = std::make_shared<Environment>(previousEnvironment);
    ++_scopeDepth;

    try {
        for (const auto& innerStmt : stmt.getStatements()) {
            innerStmt->accept(*this);
        }
    }
    catch (const std::exception& e) {
        _currentEnvironment = previousEnvironment;
        --_scopeDepth;
        throw e;
    }
    
    _currentEnvironment = previousEnvironment;
    --_scopeDepth;
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

    auto str = stringify(value);
    LOG_DEBUG << "Printing: " << str;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
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
    switch (expr.getLiteral().getType()) {
        case Token::Type::String: {
            auto value = expr.getLiteral().getValue<std::string>();
            LOG_DEBUG << "String literal: " << value;
            return value;
        }
        case Token::Type::Int: {
            auto value = expr.getLiteral().getValue<std::int32_t>();
            LOG_DEBUG << "Int literal: " << value;
            return value;
        }
        case Token::Type::True: {
            bool value{true};
            LOG_DEBUG << "Bool literal: true";
            return value;
        }
        case Token::Type::False: {
            bool value{false};
            LOG_DEBUG << "Bool literal: false";
            return value;
        }
        default:
            return std::monostate{};
    }
}

RuntimeValue Interpreter::visitVariableExpr(const VariableExpr& expr) {
    LOG_DEBUG << "Visiting VariableExpr";
    const auto& name = expr.getName().getValue<std::string>();
    LOG_DEBUG << std::format("Accessing variable {} at scope depth {}", name, _scopeDepth);
    auto value = _currentEnvironment->getVar(name);
    LOG_DEBUG << std::format("Retrieved value: {}", stringify(value));
    return value;
}

RuntimeValue Interpreter::visitBinaryExpr(const BinaryExpr& expr) {
    LOG_DEBUG << "Visiting BinaryExpr";
    const auto& left = expr.getLeft().accept(*this);
    const auto& op = expr.getOperator().getType();
    const auto& right = expr.getRight().accept(*this);

    switch (op) {
        case Token::Type::Equal:
            LOG_DEBUG << "Found equal operator";
            return left == right;
        case Token::Type::NotEqual:
            LOG_DEBUG << "Found not equal operator";
            return left != right;
        case Token::Type::Less:
            LOG_DEBUG << "Found less operator";
            return left < right;
        case Token::Type::Greater:
            LOG_DEBUG << "Found greater operator";
            return left > right;
        default:
            throw std::runtime_error{"Unknown operator in binary expression"};
    }
}

RuntimeValue Interpreter::visitUnaryExpr(const UnaryExpr& expr) {
    LOG_DEBUG << "Visiting UnaryExpr";
    return std::monostate{};
}

RuntimeValue Interpreter::visitCallExpr(const CallExpr& expr) {
    LOG_DEBUG << "Visiting CallExpr";
    return std::monostate{};
}
