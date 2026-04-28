#pragma once

#include "core/SourceRange.hpp"
#include "LValue.hpp"
#include <optional>
#include <source_location>

struct RuntimeValue;
class AstVisitor;

class AstNode {
public:
    AstNode(SourceRange srcRange) : _srcRange{std::move(srcRange)} {}
    virtual ~AstNode() = default;

    virtual RuntimeValue accept(AstVisitor&) const = 0;
    virtual std::optional<LValue> getLValue() const { return std::nullopt; }
    LValue getLValueUnsafe(const std::source_location& = std::source_location::current()) const;
    const SourceRange& getSrcRange() const { return _srcRange; }

private:
    SourceRange _srcRange;
};
