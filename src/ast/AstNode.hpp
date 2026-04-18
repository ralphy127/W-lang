#pragma once

#include "core/SourceRange.hpp"

struct RuntimeValue;
class AstVisitor;

class AstNode {
public:
    AstNode(SourceRange srcRange) : _srcRange{std::move(srcRange)} {}
    virtual ~AstNode() = default;

    virtual RuntimeValue accept(AstVisitor&) const = 0;
    const SourceRange& getSrcRange() const { return _srcRange; }

private:
    SourceRange _srcRange;
};
