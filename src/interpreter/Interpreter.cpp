#include "Interpreter.hpp"

#include <iostream>
#include "utils/Logging.hpp"

namespace {

class BreakStatementException {};
struct ReturnStatementException {
    RuntimeValue value;
};

template <typename Operation>
RuntimeValue applyMath(const RuntimeValue& left, const RuntimeValue& right, Operation&& operation) {
    return std::visit(overloaded{
        [&operation](std::int32_t l, std::int32_t r) -> RuntimeValue {
            return operation(l, r); },
        [&operation](double l, double r) -> RuntimeValue {
            return operation(l, r); },
        [&operation](std::int32_t l, double r) -> RuntimeValue {
            return operation(static_cast<double>(l), r); },
        [&operation](double l, std::int32_t r) -> RuntimeValue {
            return operation(l, static_cast<double>(r)); },
        [](auto&&, auto&&) -> RuntimeValue { 
            throw std::runtime_error("Math works only on numbers!"); 
        }
    }, left, right);
}

}

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmt>> statements)
    : _statements{std::move(statements)} {
}

void Interpreter::interpret() {
    LOG_DEBUG << std::format("Starting interpretation of {} statements", _statements.size());
    for (const auto& stmt : _statements) {
        stmt->accept(*this);
    }

    LOG_DEBUG << "Looking for entry point 'macho'";
    const auto it = _functions.find("macho");
    if (it == _functions.end()) {
        LOG_ERROR << "Missing entry point 'gig macho()'";
        throw std::runtime_error("Missing entry point 'gig macho()'");
    }

    LOG_DEBUG << "Executing 'macho' function";
    try {
        it->second.get().getBody().accept(*this);
    }
    catch (ReturnStatementException) {}
    catch (const std::exception& e) {
        // TODO error handling
        LOG_ERROR << "[FATAL] Caught unexpected error:" << e.what();
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

    return {};
}

RuntimeValue Interpreter::visitReassignStmt(const ReassignStmt& stmt) {
    LOG_DEBUG << "Visiting ReassignStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    auto newValue = stmt.getValue().accept(*this);
    _currentEnvironment->reassignVar(name, std::move(newValue));

    LOG_DEBUG << std::format("Reassigning variable {} to {} at scope depth {}",
        name, stringify(newValue), _scopeDepth);

    return {};
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
    return {};
}

RuntimeValue Interpreter::visitIfStmt(const IfStmt& stmt) {
    LOG_DEBUG << "Visiting IfStmt";
    const auto condition = stmt.getCondition().accept(*this);
    if (not std::holds_alternative<bool>(condition)) {
        throw std::runtime_error{"If statement's condition should be a boolean"};
    }
    if (std::get<bool>(condition)) {
        stmt.getThenBlock().accept(*this);
        return {};
    }
    for (const auto& elseIfClause : stmt.getElseIfClauses()) {
        const auto elseIfCondition = elseIfClause.condition->accept(*this);
        if (not std::holds_alternative<bool>(elseIfCondition)) {
            throw std::runtime_error{"Else if statement's condition should be a boolean"};
        }
        if (std::get<bool>(elseIfCondition)) {
            elseIfClause.body->accept(*this);
            return{};
        }
    }
    if (stmt.hasElseBlock()) {
        stmt.getElseBlock().accept(*this);
    }
    return {};
}

RuntimeValue Interpreter::visitLoopStmt(const LoopStmt& stmt) {
    LOG_DEBUG << "Visiting LoopStmt";
    const auto& body = stmt.getBody();
    while (true) {
        try {
            body.accept(*this);
        }
        catch (BreakStatementException) {
            LOG_DEBUG << "Caught break exception";
            break;
        }
    }
    return {};
}

RuntimeValue Interpreter::visitRepeatStmt(const RepeatStmt& stmt) {
    LOG_DEBUG << "Visiting RepeatStmt";
    // TODO casting to int / error handling
    const auto count = std::get<std::int32_t>(stmt.getCount().accept(*this));
    const auto& body = stmt.getBody();
    for (std::int32_t i{0}; i < count; ++i) {
        try {
            body.accept(*this);
        }
        catch (BreakStatementException) {
            LOG_DEBUG << "Caught break exception";
            break;
        }
    }
    return {};
}

RuntimeValue Interpreter::visitPrintStmt(const PrintStmt& stmt) {
    LOG_DEBUG << "Visiting PrintStmt";
    const auto value = stmt.getExpression().accept(*this);

    auto str = stringify(value);
    LOG_DEBUG << "Printing: " << str;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    std::cout << str << "!!!" << std::endl;
    
    return {};
}

RuntimeValue Interpreter::visitReturnStmt(const ReturnStmt& stmt) {
    LOG_DEBUG << "Visiting ReturnStmt";
    if (stmt.hasValue()) {
        LOG_DEBUG << "Returning a value";
        throw ReturnStatementException{stmt.getValue().accept(*this)};
    }
    
    LOG_DEBUG << "Returning default value";
    return {};
}

RuntimeValue Interpreter::visitBreakStmt(const BreakStmt& stmt) {
    LOG_DEBUG << "Visiting BreakStmt";
    throw BreakStatementException{};
    return {};
}

RuntimeValue Interpreter::visitFunctionStmt(const FunctionStmt& stmt) {
    LOG_DEBUG << "Visiting FunctionStmt";
    auto funcName = stmt.getName().getValue<std::string>();
    LOG_DEBUG << "Registering function: " << funcName;
    _functions.emplace(funcName, stmt);
    return {};
}

RuntimeValue Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    LOG_DEBUG << "Visiting ExpressionStmt";
    return {};
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
            return {};
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
        case Token::Type::Plus: {
            LOG_DEBUG << "Found plus operator";
            return applyMath(left, right, [](auto&& a, auto&& b) { return a + b; });
        }
        case Token::Type::Minus: {
            LOG_DEBUG << "Found minus operator";
            return applyMath(left, right, [](auto&& a, auto&& b) { return a - b; });
        }
        default:
            throw std::runtime_error{"Unknown operator in binary expression"};
    }
}

RuntimeValue Interpreter::visitUnaryExpr(const UnaryExpr& expr) {
    LOG_DEBUG << "Visiting UnaryExpr";
    return {};
}

RuntimeValue Interpreter::visitCallExpr(const CallExpr& expr) {
    LOG_DEBUG << "Visiting CallExpr";
    const auto& name = expr.getName().getValue<std::string>();
    if (not _functions.contains(name)) {
        // TODO better error handling, but it requires some bigger decisions
        throw std::runtime_error{"Function does not exist"};
    }

    const auto& callArgs = expr.getArgs();
    const auto& funcStmt = _functions.at(name).get();
    const auto& parameters = funcStmt.getParameters();
    const auto parametersCount = parameters.size();
    if (callArgs.size() != parametersCount) {
        // TODO error handling again
        throw std::runtime_error{"Argument counts do not match"};
    }

    LOG_DEBUG << "Evaluating arguments";
    std::vector<RuntimeValue> evaluatedParameters(parametersCount);
    for (size_t i{0ull}; i < parametersCount; ++i) {
        evaluatedParameters[i] = callArgs[i]->accept(*this);
    }

    LOG_DEBUG << "Creating arguments' environment";
    auto callEnv = std::make_shared<Environment>(_globalEnvironment);
    for (size_t i{0ull}; i < parametersCount; ++i) {
        callEnv->defineVar(parameters[i].getValue<std::string>(), std::move(evaluatedParameters[i]));
    }

    LOG_DEBUG << "Jumping into arguments' environemnt";
    auto previousEnv = _currentEnvironment;
    _currentEnvironment = callEnv;
    try {
        funcStmt.getBody().accept(*this);
    }
    catch (ReturnStatementException ret) {
        LOG_DEBUG << "Caught return value";
        _currentEnvironment = previousEnv;
        return ret.value;
    }
    catch (...) {
        _currentEnvironment = previousEnv;
        throw;
    }

    // TODO make sure default (null) return works
    _currentEnvironment = _currentEnvironment = previousEnv;
    return {};
}
