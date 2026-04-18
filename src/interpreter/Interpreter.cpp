#include "Interpreter.hpp"
#include <iostream>
#include <cassert>
#include "utils/Logging.hpp"
#include "modules/Gossip.hpp"
#include "runtime/RuntimeErrors.hpp"

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
            throw std::invalid_argument("Math is only mathing on numbers"); 
        }
    }, left, right);
}

}

Interpreter::Interpreter(
    std::vector<std::unique_ptr<Stmt>> statements,
    AstResolver astResolver,
    std::string mainFolderPath)
    : _statements{std::move(statements)}
    , _astResolver{std::move(astResolver)}
    , _mainFolderPath{std::move(mainFolderPath)} {

    for (const auto& stmt : _statements) {
        assert(stmt);
    }
    assert(astResolver);
}

void Interpreter::interpret() {
    LOG_DEBUG << std::format("Starting interpretation of {} statements", _statements.size());
    for (const auto& stmt : _statements) {
        evaluate(*stmt);
    }

    LOG_DEBUG << "Looking for entry point 'macho'";
    try {
        auto mainFunc = tryAs<Function>(_currentEnvironment->getVar("macho"));
        LOG_DEBUG << "Executing 'macho' function";
        mainFunc(std::vector<RuntimeValue>{});
    }
    catch (ReturnStatementException) {}
    catch (const std::exception& e) {
        LOG_ERROR << "[FATAL] Caught unexpected error: " << e.what();
        throw RuntimeError{RuntimeError::Type::Undefined, _currentRange, e.what()};
    }
    LOG_DEBUG << "Interpretation completed";
}

RuntimeValue Interpreter::evaluate(const AstNode& node) {
    auto previousRange = _currentRange; 
    _currentRange = node.getSrcRange(); 

    RuntimeValue result = node.accept(*this); 
    _currentRange = previousRange; 
    return result;
}

RuntimeValue Interpreter::visitVarDefinitionStmt(const VarDefinitionStmt& stmt) {
    LOG_DEBUG << "Visiting VarDefinitionStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    auto value = evaluate(stmt.getInitializer());
    LOG_DEBUG << std::format("Defining variable {} with {} at scope depth {}",
        name, stringify(value), _scopeDepth);
    _currentEnvironment->defineVar(name, std::move(value));
    return Null{};
}

RuntimeValue Interpreter::visitReassignStmt(const ReassignStmt& stmt) {
    LOG_DEBUG << "Visiting ReassignStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    auto newValue = evaluate(stmt.getValue());
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
            evaluate(*innerStmt);
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
    const auto condition = evaluate(stmt.getCondition());
    if (not is<Bool>(condition)) {
        throw RuntimeError{
            RuntimeError::Type::Value,
            stmt.getCondition().getSrcRange(),
            "That check needs Bool vibes only"};
    }
    if (as<Bool>(condition)) {
        evaluate(stmt.getThenBlock());
        return Null{};
    }
    for (const auto& elseIfClause : stmt.getElseIfClauses()) {
        const auto elseIfCondition = evaluate(*elseIfClause.condition);
        if (not is<Bool>(elseIfCondition)) {
            throw RuntimeError{RuntimeError::Type::Value,
                elseIfClause.condition->getSrcRange(),
                "That check needs Bool vibes only"};
        }
        if (as<Bool>(elseIfCondition)) {
            evaluate(*elseIfClause.body);
            return Null{};
        }
    }
    if (auto elseBlock = stmt.getElseBlock()) {
        evaluate(elseBlock->get());
    }
    return Null{};
}

RuntimeValue Interpreter::visitLoopStmt(const LoopStmt& stmt) {
    LOG_DEBUG << "Visiting LoopStmt";
    const auto& body = stmt.getBody();
    while (true) {
        try {
            evaluate(body);
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
    const auto count = std::get<std::int32_t>(evaluate(stmt.getCount()));
    const auto& body = stmt.getBody();
    for (std::int32_t i{0}; i < count; ++i) {
        try {
            evaluate(body);
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
    if (auto returnValue = stmt.getValue()) {
        LOG_DEBUG << "Returning a value";
        throw ReturnStatementException{evaluate(returnValue->get())};
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
    // TODO make sure stmt always outlives function / refactor just in case
    auto function = [this, &stmt](const std::vector<RuntimeValue>& args) -> RuntimeValue {
        auto localEnvironment = std::make_shared<Environment>(_globalEnvironment);
        const auto& argNames = stmt.getParameters();
        const auto namesCount = argNames.size();

        const auto argsSize = args.size();
        if (namesCount != argsSize) {
            throw RuntimeError{
                RuntimeError::Type::OutOfBounds,
                stmt.getSrcRange(),
                std::format("Argument count don't vibe ({} is not {})", argsSize, namesCount)};
        }

        for (size_t i{0ull}; i < argsSize; ++i) {
            localEnvironment->defineVar(argNames[i].getValue<std::string>(), args[i]);
        }

        LOG_DEBUG << "Jumping into local function environemnt";
        auto previousEnv = _currentEnvironment;
        _currentEnvironment = localEnvironment;

        try {
            evaluate(stmt.getBody());
        }
        catch (const ReturnStatementException& ret) {
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
    };

    _currentEnvironment->defineVar(funcName, Function{std::move(function)});
        
    return Null{};
}

RuntimeValue Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    LOG_DEBUG << "Visiting ExpressionStmt";
    return evaluate(stmt.getExpression());
}

RuntimeValue Interpreter::visitImportStmt(const ImportStmt& stmt) {
    LOG_DEBUG << "Visiting ImportStmt";
    const auto& moduleName = stmt.getModuleName().getValue<std::string>();

    if (moduleName == "gossip") {
        LOG_DEBUG << "Defining native module " << moduleName;
        _currentEnvironment->defineVar(moduleName, modules::createGossipModule());
        return Null{};
    }

    auto userFileName = _mainFolderPath + moduleName + ".weird";

    LOG_DEBUG << std::format("Resolving user module: {} ast", moduleName);
    auto moduleAst = _astResolver(userFileName);
    LOG_DEBUG << std::format("Resolved user module: {} ast", moduleName);
    
    auto previousEnvironment = _currentEnvironment;
    _currentEnvironment = std::make_shared<Environment>();

    for (const auto& statement : moduleAst) {
        evaluate(*statement);
    }
    LOG_DEBUG << std::format("Interpreted all statements from user module: {} ast", moduleName);

    // ! TODO instead of stealing resources, consider other Module implementation
    auto module = Module{std::make_shared<std::unordered_map<std::string, RuntimeValue>>(
        _currentEnvironment->stealAllVariables())};
        
    _currentEnvironment = previousEnvironment;
    _currentEnvironment->defineVar(moduleName, std::move(module));
    _importedModuleAsts[moduleName] = std::move(moduleAst);
    return Null{};
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
    const auto& left = evaluate(expr.getLeft());
    const auto& op = expr.getOperator().getType();
    const auto& right = evaluate(expr.getRight());

    try {
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
            case Token::Type::Multiply: {
                LOG_DEBUG << "Found Multiply operator";
                return applyMath(left, right, [](auto&& a, auto&& b) { return a * b; });
            }
            case Token::Type::Divide: {
                LOG_DEBUG << "Found Divide operator";
                return applyMath(left, right, [](auto&& a, auto&& b) { return a / b; });
            }
            default:
                throw RuntimeError{
                    RuntimeError::Type::Undefined,
                    expr.getSrcRange(),
                    "Call the dev bud: unknown binary operator"};
        }
    }
    catch (const std::invalid_argument& e) {
        const auto& msg = e.what();
        LOG_ERROR << "Caught math exception: " << msg;
        throw RuntimeError{RuntimeError::Type::Math, expr.getSrcRange(), msg};
    }
    catch (const std::exception& e) {
        const auto& msg = e.what();
        LOG_ERROR << "Caught other exception: " << msg;
        throw RuntimeError{RuntimeError::Type::Undefined, expr.getSrcRange(), msg};
    }
}

RuntimeValue Interpreter::visitUnaryExpr(const UnaryExpr& expr) {
    LOG_DEBUG << "Visiting UnaryExpr";
    if (expr.getOperator().getType() == Token::Type::Incr) {
        // TODO might make some problems with other types / block to int only
        if (const auto* varExpr = dynamic_cast<const VariableExpr*>(&expr.getRight())) {
            const auto& name = varExpr->getName().getValue<std::string>();
            auto oldValue = std::get<std::int32_t>(evaluate(*varExpr));
            LOG_DEBUG << std::format(
                "Incrementing variable {}: {} --> {}", name, oldValue, oldValue + 1);
            _currentEnvironment->reassignVar(name, oldValue + 1);
            return Null{};
        }
        else {
            // TODO maybe chceck it somehow in parser ?
            throw RuntimeError{
                RuntimeError::Type::Logic,
                expr.getSrcRange(),
                "Can't pump_it into the void!"};
        }
    }
    LOG_WARN << "Unknown unary operator";
    return Null{};
}

RuntimeValue Interpreter::visitCallExpr(const CallExpr& expr) {
    LOG_DEBUG << "Visiting CallExpr";
    auto calleeValue = evaluate(expr.getCallee());

    if (not is<Function>(calleeValue)) {
        // TODO investigate a failure scenario
        throw RuntimeError{RuntimeError::Type::Undefined, expr.getSrcRange(), "Call the dev bud"};
    }
    LOG_DEBUG << "Executing Function";
            
    const auto& callArgs = expr.getArgs();
    std::vector<RuntimeValue> evaluatedArgs{};
    evaluatedArgs.reserve(callArgs.size());
            
    for (const auto& arg : callArgs) {
        evaluatedArgs.push_back(evaluate(*arg));
    }

    LOG_DEBUG << std::format("Successfully valuated {} parameters", evaluatedArgs.size());

    const auto& function = as<Function>(calleeValue);
    return function(evaluatedArgs);
}

RuntimeValue Interpreter::handleModuleCall(
    const Module& mod,
    const std::string& rightName,
    const DotExpr& expr) {

    auto it = mod->find(rightName);
    if (it == mod->end()) {
        throw RuntimeError{
            RuntimeError::Type::Value,
            expr.getSrcRange(),
            std::format("Module ain't got '{}'", rightName)};
    }

    return it->second;
}

RuntimeValue Interpreter::visitDotExpr(const DotExpr& expr) {
    LOG_DEBUG << "Visiting DotExpr";

    const auto leftValue = evaluate(expr.getLeft());
    const auto& rightName = expr.getRight().getValue<std::string>();

    if (is<Module>(leftValue)) {
        return handleModuleCall(as<Module>(leftValue), rightName, expr);
    }
    if (is<Vector>(leftValue)) {
        return callVectorMethod(as<Vector>(leftValue), rightName);
    }
    if (is<String>(leftValue)) {
        return callStringMethod(as<String>(leftValue), rightName);
    }

    throw RuntimeError{RuntimeError::Type::Value, expr.getSrcRange(), "Can't dot into that"};
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

    auto firstElement = evaluate(*initializers[0]);
    vector->typeId = firstElement.index();

    for (size_t i{0ull}; i < vectorSize; ++i) {
        auto nextValue = evaluate(*initializers[i]);
        if (nextValue.index() != vector->typeId) {
            throw RuntimeError{
                RuntimeError::Type::Value,
                expr.getSrcRange(),
                "Lineup got mixed vibes - all elements must be the same type"};
        }
        vector->data.push_back(std::move(nextValue));
    }

    return RuntimeValue{vector};
}

RuntimeValue Interpreter::visitLogicalExpr(const LogicalExpr& expr) {
    LOG_DEBUG << "Visiting LogicalExpr";

    const auto op = expr.getOperator().getType();
    const auto leftResult = evaluate(expr.getLeft());
    const auto leftBool = tryAs<Bool>(leftResult);
    if (op == Token::Type::Or) {
        if (leftBool) {
            return Bool{true};
        }
        const auto rightResult = evaluate(expr.getRight());
        return tryAs<Bool>(rightResult);
    }

    if (op == Token::Type::And) {
        if (not leftBool) {
            return Bool{false};
        }
        const auto rightResult = evaluate(expr.getRight());
        return tryAs<Bool>(rightResult);
    }

    // TODO investigate how parser would allow an unknown logical operator here
    throw RuntimeError{
        RuntimeError::Type::Undefined,
        expr.getSrcRange(),
        "Call the dev bud: unknown logical operator"};
}
