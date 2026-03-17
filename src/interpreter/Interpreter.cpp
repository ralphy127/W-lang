#include "Interpreter.hpp"

#include <iostream>
#include "utils/Logging.hpp"
#include "modules/Gossip.hpp"
#include "native_types/Vector.hpp"

namespace {

class BreakStatementException {};
struct ReturnStatementException {
    RuntimeValue value;
};

template <typename Op>
RuntimeValue applyMath(const RuntimeValue& left, const RuntimeValue& right, Op&& operation) {
    return std::visit(overloaded{
        [&operation](Int l, Int r) -> RuntimeValue {
            return Int{operation(l, r)}; },
        [&operation](Float l, Float r) -> RuntimeValue {
            return Float{operation(l, r)}; },
        [&operation](Int l, Float r) -> RuntimeValue {
            return Float{operation(static_cast<Float>(l), r)}; },
        [&operation](Float l, Int r) -> RuntimeValue {
            return Float{operation(l, static_cast<Float>(r))}; },
        [](auto&&, auto&&) -> RuntimeValue { 
            throw std::runtime_error("Math works only on numbers"); 
        }
    }, left, right);
}

}

Interpreter::Interpreter(std::vector<std::unique_ptr<Stmt>> statements)
    : _statements{std::move(statements)} {}

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
        LOG_ERROR << "[FATAL] Caught unexpected error: " << e.what();
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

    return Null{};
}

RuntimeValue Interpreter::visitReassignStmt(const ReassignStmt& stmt) {
    LOG_DEBUG << "Visiting ReassignStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    auto newValue = stmt.getValue().accept(*this);
    _currentEnvironment->reassignVar(name, std::move(newValue));

    LOG_DEBUG << std::format("Reassigning variable {} to {} at scope depth {}",
        name, stringify(newValue), _scopeDepth);

    return Null{};
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
        throw;
    }
    
    _currentEnvironment = previousEnvironment;
    --_scopeDepth;
    return Null{};
}

RuntimeValue Interpreter::visitIfStmt(const IfStmt& stmt) {
    LOG_DEBUG << "Visiting IfStmt";
    const auto condition = stmt.getCondition().accept(*this);
    if (not std::holds_alternative<bool>(condition)) {
        throw std::runtime_error{"If statement's condition should be a boolean"};
    }
    if (std::get<bool>(condition)) {
        stmt.getThenBlock().accept(*this);
        return Null{};
    }
    for (const auto& elseIfClause : stmt.getElseIfClauses()) {
        const auto elseIfCondition = elseIfClause.condition->accept(*this);
        if (not std::holds_alternative<bool>(elseIfCondition)) {
            throw std::runtime_error{"Else if statement's condition should be a boolean"};
        }
        if (std::get<bool>(elseIfCondition)) {
            elseIfClause.body->accept(*this);
            return Null{};
        }
    }
    if (stmt.hasElseBlock()) {
        stmt.getElseBlock().accept(*this);
    }
    return Null{};
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
    return Null{};
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
    return Null{};
}

RuntimeValue Interpreter::visitReturnStmt(const ReturnStmt& stmt) {
    LOG_DEBUG << "Visiting ReturnStmt";
    if (stmt.hasValue()) {
        LOG_DEBUG << "Returning a value";
        throw ReturnStatementException{stmt.getValue().accept(*this)};
    }
    
    LOG_DEBUG << "Returning default value";
    return Null{};
}

RuntimeValue Interpreter::visitBreakStmt(const BreakStmt& stmt) {
    LOG_DEBUG << "Visiting BreakStmt";
    throw BreakStatementException{};
}

RuntimeValue Interpreter::visitFunctionStmt(const FunctionStmt& stmt) {
    LOG_DEBUG << "Visiting FunctionStmt";
    auto funcName = stmt.getName().getValue<std::string>();
    LOG_DEBUG << "Registering function: " << funcName;
    _functions.emplace(funcName, stmt);
    return Null{};
}

RuntimeValue Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    LOG_DEBUG << "Visiting ExpressionStmt";
    return stmt.getExpression().accept(*this);
}

RuntimeValue Interpreter::visitImportStmt(const ImportStmt& stmt) {
    LOG_DEBUG << "Visiting ImportStmt";
    const auto& moduleName = stmt.getModuleName().getValue<std::string>();

    if (moduleName == "gossip") {
        LOG_DEBUG << "Defining module " << moduleName;
        _currentEnvironment->defineVar(moduleName, modules::createGossipModule());
        return Null{};
    }
    throw std::runtime_error{"Import of unknown module: " + moduleName};
}
    
RuntimeValue Interpreter::visitLiteralExpr(const LiteralExpr& expr) {
    LOG_DEBUG << "Visiting LiteralExpr";
    const auto& literal = expr.getLiteral();
    switch (literal.getType()) {
        case Token::Type::String: {
            auto value = literal.getValue<std::string>();
            LOG_DEBUG << "String literal: " << value;
            return String{std::move(value)};
        }
        case Token::Type::Int: {
            auto value = literal.getValue<std::int32_t>();
            LOG_DEBUG << "Int literal: " << value;
            return Int{value};
        }
        case Token::Type::Float: {
            auto value = literal.getValue<double>();
            LOG_DEBUG << "Float literal: " << value;
            return Float{value};
        }
        case Token::Type::True: {
            bool value{true};
            LOG_DEBUG << "Bool literal: true";
            return Bool{value};
        }
        case Token::Type::False: {
            bool value{false};
            LOG_DEBUG << "Bool literal: false";
            return Bool{value};
        }
        default:
            return Null{};
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
            return Bool{left == right};
        case Token::Type::NotEqual:
            LOG_DEBUG << "Found not equal operator";
            return Bool{left != right};
        case Token::Type::Less:
            LOG_DEBUG << "Found less operator";
            return Bool{left < right};
        case Token::Type::Greater:
            LOG_DEBUG << "Found greater operator";
            return Bool{left > right};
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
    if (expr.getOperator().getType() == Token::Type::Incr) {
        // TODO might make some problems with other types / block to int only
        if (const auto* varExpr = dynamic_cast<const VariableExpr*>(&expr.getRight())) {
            const auto& name = varExpr->getName().getValue<std::string>();
            auto oldValue = std::get<std::int32_t>(varExpr->accept(*this));
            LOG_DEBUG << std::format("Incrementing variable {}: {} --> {}", name, oldValue, oldValue + 1);
            _currentEnvironment->reassignVar(name, oldValue + 1);
            return Null{};
        }
        else {
            // TODO error handling
            throw std::runtime_error{"Right side of 'pump_it' operator must be a variable"};
        }
    }
    LOG_WARN << "Unknown unary operator";
    return Null{};
}

RuntimeValue Interpreter::handleUserDefinedFunctionCall(const VariableExpr& varExpr, const std::vector<std::unique_ptr<Expr>>& callArgs) {
    const auto& name = varExpr.getName().getValue<std::string>();
    if (not _functions.contains(name)) {
        throw std::runtime_error{"Function does not exist"};
    }

    const auto& funcStmt = _functions.at(name).get();
    const auto& parameters = funcStmt.getParameters();
    const auto parametersCount = parameters.size();
    if (callArgs.size() != parametersCount) {
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
        callEnv->defineVar(
            parameters[i].getValue<std::string>(),
            std::move(evaluatedParameters[i]));
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

    _currentEnvironment = previousEnv;
    return Null{};
}

RuntimeValue Interpreter::visitCallExpr(const CallExpr& expr) {
    LOG_DEBUG << "Visiting CallExpr";
    if (const auto* varExpr = dynamic_cast<const VariableExpr*>(&expr.getCallee())) {
        return handleUserDefinedFunctionCall(*varExpr, expr.getArgs());
    }

    auto calleeValue = expr.getCallee().accept(*this);
    if (std::holds_alternative<NativeFunction>(calleeValue)) {
        LOG_DEBUG << "Executing NativeFunction";
        
        const auto& nativeFunc = std::get<NativeFunction>(calleeValue);
        const auto& callArgs = expr.getArgs();
        std::vector<RuntimeValue> evaluatedArgs{};
        evaluatedArgs.reserve(callArgs.size());
        
        for (const auto& arg : callArgs) {
            evaluatedArgs.push_back(arg->accept(*this));
        }
        return nativeFunc(evaluatedArgs);
    }

    throw std::runtime_error{"Unknown instructions in visitCallExpr"};
}

RuntimeValue Interpreter::handleModuleCall(const Module& mod, const std::string& rightName) {
    auto it = mod->find(rightName);
    if (it == mod->end()) {
        throw std::runtime_error{std::format("{} not found in module", rightName)};
    }

    return it->second;
}

RuntimeValue Interpreter::visitDotExpr(const DotExpr& expr) {
    LOG_DEBUG << "Visiting DotExpr";
    const auto& leftName = dynamic_cast<const VariableExpr&>(expr.getLeft()).getName().getValue<std::string>();

    // TODO support nested dot expressions
    auto var = _currentEnvironment->getVar(leftName);
    const auto& rightName = expr.getRight().getValue<std::string>();
    if (std::holds_alternative<Module>(var)) {
        return handleModuleCall(std::get<Module>(var), rightName);
    }
    if (std::holds_alternative<Vector>(var)) {
        return callVectorMethod(std::get<Vector>(var), rightName);
    }

    throw std::runtime_error{"Unknown dot expression"};
}

RuntimeValue Interpreter::visitVectorExpr(const VectorExpr& expr) {
    LOG_DEBUG << "Visiting VectorExpr";
    
    auto vector = std::make_shared<Vector::element_type>();
    const auto& initializers = expr.getInitializers();
    if (initializers.empty()) {
        LOG_DEBUG << "Created empty vector";
        return vector;
    }
    const auto vectorSize = initializers.size();
    vector->data.reserve(vectorSize);

    auto firstElement = initializers[0]->accept(*this);
    vector->typeId = firstElement.index();

    for (size_t i{0ull}; i < vectorSize; ++i) {
        auto nextValue = initializers[i]->accept(*this);
        if (nextValue.index() != vector->typeId) {
            throw std::runtime_error{"All items in vector must be of the same type"};
        }
        vector->data.push_back(std::move(nextValue));
    }

    return RuntimeValue{vector};
}
