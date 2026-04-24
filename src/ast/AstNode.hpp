#pragma once

#include "core/SourceRange.hpp"
#include "LValue.hpp"
#include <optional>

struct RuntimeValue;
class AstVisitor;

class AstNode {
public:
    AstNode(SourceRange srcRange) : _srcRange{std::move(srcRange)} {}
    virtual ~AstNode() = default;

    virtual RuntimeValue accept(AstVisitor&) const = 0;
    virtual std::optional<LValue> getLValue() const { return std::nullopt; }
    const SourceRange& getSrcRange() const { return _srcRange; }

private:
    SourceRange _srcRange;
};
