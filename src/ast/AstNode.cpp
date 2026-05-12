#include "AstNode.hpp"
#include "errors/InternalError.hpp"

LValue AstNode::getLValue(const std::source_location& loc) const {
    auto lValue = getLValueOpt();
    if (not lValue.has_value()) {
        throw InternalError{"Tried to extract LValue from a node that doesn't support it", loc};
    }
    return lValue.value();
}
