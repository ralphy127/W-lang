#pragma once

#include "runtime/RuntimeValue.hpp"

class VarDefinitionStmt;
class ReassignStmt;
class BlockStmt;
class IfStmt;
class LoopStmt;
class RepeatStmt;
class PrintStmt;
class ReturnStmt;
class BreakStmt;
class FunctionStmt;
class ExpressionStmt;

class LiteralExpr;
class VariableExpr;
class BinaryExpr;
class UnaryExpr;
class CallExpr;

class Visitor {
public:
    virtual ~Visitor() = default;
    
    virtual RuntimeValue visitVarDefinitionStmt(const VarDefinitionStmt&) = 0;
    virtual RuntimeValue visitReassignStmt(const ReassignStmt&) = 0;
    virtual RuntimeValue visitBlockStmt(const BlockStmt&) = 0;
    virtual RuntimeValue visitIfStmt(const IfStmt&) = 0;
    virtual RuntimeValue visitLoopStmt(const LoopStmt&) = 0;
    virtual RuntimeValue visitRepeatStmt(const RepeatStmt&) = 0;
    virtual RuntimeValue visitPrintStmt(const PrintStmt&) = 0;
    virtual RuntimeValue visitReturnStmt(const ReturnStmt&) = 0;
    virtual RuntimeValue visitBreakStmt(const BreakStmt&) = 0;
    virtual RuntimeValue visitFunctionStmt(const FunctionStmt&) = 0;
    virtual RuntimeValue visitExpressionStmt(const ExpressionStmt&) = 0;
    
    virtual RuntimeValue visitLiteralExpr(const LiteralExpr&) = 0;
    virtual RuntimeValue visitVariableExpr(const VariableExpr&) = 0;
    virtual RuntimeValue visitBinaryExpr(const BinaryExpr&) = 0;
    virtual RuntimeValue visitUnaryExpr(const UnaryExpr&) = 0;
    virtual RuntimeValue visitCallExpr(const CallExpr&) = 0;
};