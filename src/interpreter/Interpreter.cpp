#include "Interpreter.hpp"
#include <iostream>
#include <cassert>
#include "utils/Logging.hpp"
#include "modules/Gossip.hpp"
#include "runtime/RuntimeErrors.hpp"
#include "EnvironmentGuard.hpp"

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

void throwDevError(SourceRange range, const std::string& msg) {
    throw RuntimeError{
        RuntimeError::Type::Undefined,
        range,
        "Call the dev bud: " + msg};
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

int Interpreter::interpret() {
    LOG_DEBUG << std::format("Starting interpretation of {} statements", _statements.size());
    for (const auto& stmt : _statements) {
        evaluate(*stmt);
    }

    LOG_DEBUG << "Looking for entry point 'macho'";
    try {
        if (not _currentEnvironment->hasVar("macho")) {
            throw RuntimeError{
                RuntimeError::Type::Logic,
                _currentRange,
                "Macho gig does not exist"};
        }

        const auto mainFunc = _currentEnvironment->getVar("macho");
        if (not is<Function>(mainFunc)) {
            throw RuntimeError{
                RuntimeError::Type::TypeMismatch,
                _currentRange,
                "Macho is not a gig"};
        }

        LOG_DEBUG << "Executing 'macho' function";
        const auto ret = asUnsafe<Function>(mainFunc).exec(std::vector<RuntimeValue>{});
        if (is<Int>(ret)) {
            const auto val = asUnsafe<Int>(ret);
            LOG_DEBUG << "Returning an int from macho: " << val;
            return val;
        }
        if (is<Null>(ret)) {
            LOG_DEBUG << "Returning 0 (casted Null) from macho";
            return 0;
        }

        throw RuntimeError{
            RuntimeError::Type::Logic,
            _currentRange,
            "Casting is for the weak. Macho only yeets solids or ghosted"};
    }
    catch (const std::exception& e) {
        LOG_ERROR << "[FATAL] Caught unexpected error: " << e.what();
        throw RuntimeError{RuntimeError::Type::Undefined, _currentRange, e.what()};
    }
}

EvalProxy Interpreter::evaluate(const AstNode& node) {
    return EvalProxy{evaluateImpl(node), node.getSrcRange()};
}

RuntimeValue Interpreter::evaluateImpl(const AstNode& node) {
    auto previousRange = _currentRange; 
    _currentRange = node.getSrcRange(); 

    RuntimeValue result = node.accept(*this); 
    _currentRange = previousRange; 
    return result;
}

RuntimeValue Interpreter::visitVarDefinitionStmt(const VarDefinitionStmt& stmt) {
    LOG_DEBUG << "Visiting VarDefinitionStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    RuntimeValue value = evaluate(stmt.getInitializer());
    LOG_DEBUG << std::format("Defining variable {} with {} at scope depth {}",
        name, stringify(value), _scopeDepth);
    _currentEnvironment->defineVar(name, std::move(value));
    return Null{};
}

RuntimeValue Interpreter::visitReassignStmt(const ReassignStmt& stmt) {
    LOG_DEBUG << "Visiting ReassignStmt";

    const auto& name = stmt.getName().getValue<std::string>();
    RuntimeValue newValue = evaluate(stmt.getValue());
    _currentEnvironment->reassignVar(name, std::move(newValue));

    LOG_DEBUG << std::format("Reassigning variable {} to {} at scope depth {}",
        name, stringify(newValue), _scopeDepth);

    return Null{};
}

RuntimeValue Interpreter::visitBlockStmt(const BlockStmt& stmt) {
    LOG_DEBUG << "Visiting BlockStmt with " << stmt.getStatements().size() << " statements";

    LOG_DEBUG << "Jumping into new block environment";
    EnvironmentGuard guard{*this, std::make_shared<Environment>(_currentEnvironment)};

    for (const auto& innerStmt : stmt.getStatements()) {
        evaluate(*innerStmt);
    }

    return Null{};
}

RuntimeValue Interpreter::visitIfStmt(const IfStmt& stmt) {
    LOG_DEBUG << "Visiting IfStmt";
    const auto condition = evaluate(stmt.getCondition()).as<Bool>();
    if (condition) {
        evaluate(stmt.getThenBlock());
        return Null{};
    }
    for (const auto& elseIfClause : stmt.getElseIfClauses()) {
        const auto elseIfCondition = evaluate(*elseIfClause.condition).as<Bool>();
        if (elseIfCondition) {
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
    const auto count = evaluate(stmt.getCount()).as<Int>();
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
        RuntimeValue val = evaluate(returnValue->get());
        throw ReturnStatementException{val};
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

    auto closureEnvironment = _currentEnvironment;

    LOG_DEBUG << "Registering function: " << funcName;
    auto exec =
        [this, stmt = &stmt, closureEnvironment = _currentEnvironment]
        (const std::vector<RuntimeValue>& args) -> RuntimeValue {

        if (not stmt) {
            throwDevError(_currentRange, "gig statement has died");
        }
        auto localEnvironment = std::make_shared<Environment>(closureEnvironment);
        const auto& argNames = stmt->getParameters();
        const auto namesCount = argNames.size();

        const auto argsSize = args.size();
        if (namesCount != argsSize) {
            throw RuntimeError{
                RuntimeError::Type::OutOfBounds,
                stmt->getSrcRange(),
                std::format("Argument count don't vibe ({} is not {})", argsSize, namesCount)};
        }

        for (size_t i{0ull}; i < argsSize; ++i) {
            localEnvironment->defineVar(argNames[i].getValue<std::string>(), args[i]);
        }

        LOG_DEBUG << "Jumping into local function environment";
        EnvironmentGuard guard{*this, localEnvironment};

        try {
            evaluate(stmt->getBody());
        }
        catch (const ReturnStatementException& ret) {
            LOG_DEBUG << "Caught return value";
            return ret.value;
        }

        return Null{};
    };

    _currentEnvironment->defineVar(funcName, Function{std::move(exec), closureEnvironment});
        
    return Null{};
}

RuntimeValue Interpreter::visitExpressionStmt(const ExpressionStmt& stmt) {
    LOG_DEBUG << "Visiting ExpressionStmt";
    return evaluate(stmt.getExpression());
}

RuntimeValue Interpreter::visitImportStmt(const ImportStmt& stmt) {
    LOG_DEBUG << "Visiting ImportStmt";
    const auto& moduleName = stmt.getModuleName().getValue<std::string>();

    if (_moduleCache.contains(moduleName)) {
        LOG_DEBUG << std::format("Skipping module creation, {} already exists", moduleName);
        _currentEnvironment->defineVar(moduleName, _moduleCache[moduleName]);
        return Null{};
    }

    if (moduleName == "gossip") {
        LOG_DEBUG << "Defining native module " << moduleName;
        const auto [it, inserted] = _moduleCache.emplace(moduleName, modules::createGossipModule());
        _currentEnvironment->defineVar(moduleName, it->second);
        return Null{};
    }

    // TODO add possibility of changing global variables in modules
    // (look at /tests/test_files/user_modules/macho.weird)

    auto mod = Module{std::make_shared<Environment>()};

    _moduleCache[moduleName] = mod;

    auto userFileName = _mainFolderPath + moduleName + ".weird";

    LOG_DEBUG << std::format("Resolving user module: {} ast", moduleName);
    auto moduleAst = _astResolver(userFileName);
    LOG_DEBUG << std::format("Resolved user module: {} ast", moduleName);
    
    auto previousEnvironment = _currentEnvironment;
    _currentEnvironment = mod.env; 

    for (const auto& statement : moduleAst) {
        // TODO investigate if try/catch or guard regarding rollback to previous env is needed
        evaluate(*statement);
    }
    LOG_DEBUG << std::format("Interpreted all statements from user module: {} ast", moduleName);
        
    _currentEnvironment = previousEnvironment;
    _importedModuleAsts.emplace(moduleName, std::move(moduleAst));
    _currentEnvironment->defineVar(moduleName, mod);

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

    try {
        auto value = _currentEnvironment->getVar(name);
        LOG_DEBUG << std::format("Retrieved value: {}", stringify(value));
        return value;
    }
    catch (const NativeError& e) {
        throw RuntimeError{e.type, expr.getSrcRange(), e.what()};
    }
}

RuntimeValue Interpreter::visitBinaryExpr(const BinaryExpr& expr) {
    LOG_DEBUG << "Visiting BinaryExpr";
    const RuntimeValue& left = evaluate(expr.getLeft());
    const auto& op = expr.getOperator().getType();
    const RuntimeValue& right = evaluate(expr.getRight());

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
                throwDevError(expr.getSrcRange(), "unknown binary operator");
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
    return Null{};
}

RuntimeValue Interpreter::visitUnaryExpr(const UnaryExpr& expr) {
    LOG_DEBUG << "Visiting UnaryExpr";
    if (expr.getOperator().getType() == Token::Type::Incr) {
        const auto& right = expr.getRight();
        // TODO lValue is for sure here, make it 100% safe
        auto lValue = right.getLValue();
        if (not lValue.has_value()) {
            throwDevError(expr.getSrcRange(), "Can't get a grip on this");
        }
        auto target = lValue.value();

        auto oldValue = evaluate(right).as<Int>();
        auto newValue = oldValue + Int{1};

        if (auto* varTarget = std::get_if<LValue::Variable>(&target.location)) {
            LOG_DEBUG << std::format(
                "Incrementing variable {}: {} --> {}",
                varTarget->name,
                stringify(oldValue),
                stringify(newValue));
            _currentEnvironment->reassignVar(varTarget->name, newValue);
        }
        return Null{};
    }
    LOG_WARN << "Unknown unary operator";
    return Null{};
}

RuntimeValue Interpreter::visitCallExpr(const CallExpr& expr) {
    LOG_DEBUG << "Visiting CallExpr";
    auto function = evaluate(expr.getCallee()).as<Function>();
        
    const auto& callArgs = expr.getArgs();
    std::vector<RuntimeValue> evaluatedArgs{};
    evaluatedArgs.reserve(callArgs.size());

    for (const auto& arg : callArgs) {
        evaluatedArgs.push_back(evaluate(*arg));
    }
    LOG_DEBUG << std::format("Successfully valuated {} parameters", evaluatedArgs.size());

    return function.exec(evaluatedArgs);
}

RuntimeValue Interpreter::handleModuleCall(
    const Module& mod,
    const std::string& rightName,
    const DotExpr& expr) {

    if (not mod.env->hasVar(rightName)) {
        throw RuntimeError{
            RuntimeError::Type::TypeMismatch,
            expr.getSrcRange(),
            std::format("Hub ain't got '{}'", rightName)};
    }

    return mod.env->getVar(rightName);
}

RuntimeValue Interpreter::visitDotExpr(const DotExpr& expr) {
    LOG_DEBUG << "Visiting DotExpr";

    const RuntimeValue leftValue = evaluate(expr.getLeft());
    const auto& rightName = expr.getRight().getValue<std::string>();

    try {
        if (is<Module>(leftValue)) {
            return handleModuleCall(asUnsafe<Module>(leftValue), rightName, expr);
        }
        if (is<Vector>(leftValue)) {
            return callVectorMethod(asUnsafe<Vector>(leftValue), rightName);
        }
        if (is<String>(leftValue)) {
            return callStringMethod(asUnsafe<String>(leftValue), rightName);
        }
    }
    catch (const NativeError& e) {
        throw RuntimeError{e.type, _currentRange, e.what()};
    }
    catch (...) {
        throw RuntimeError{RuntimeError::Type::Undefined, _currentRange, "Unexpected crash, sorry"};
    }

    throw RuntimeError{RuntimeError::Type::TypeMismatch, expr.getSrcRange(), "Can't dot into that"};
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

    RuntimeValue firstElement = evaluate(*initializers[0]);
    vector->typeId = firstElement.index();

    for (size_t i{0ull}; i < vectorSize; ++i) {
        RuntimeValue nextValue = evaluate(*initializers[i]);
        if (nextValue.index() != vector->typeId) {
            throw RuntimeError{
                RuntimeError::Type::TypeMismatch,
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
    const auto leftBool = evaluate(expr.getLeft()).as<Bool>();
    if (op == Token::Type::Or) {
        if (leftBool) {
            return Bool{true};
        }
        return evaluate(expr.getRight()).as<Bool>();
    }

    if (op == Token::Type::And) {
        if (not leftBool) {
            return Bool{false};
        }
        return evaluate(expr.getRight()).as<Bool>();
    }

    throwDevError(expr.getSrcRange(), "unknown logical operator");
    return Null{};
}
