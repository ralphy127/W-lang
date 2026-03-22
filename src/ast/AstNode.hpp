#pragma once

#include "core/SourceLocation.hpp"

class AstNode {
public:
    AstNode(SourceRange srcRange) : _srcRange{std::move(srcRange)} {}
    virtual ~AstNode() = default;

    const SourceRange& getSrcRange() const { return _srcRange; }

private:
    SourceRange _srcRange;
};
