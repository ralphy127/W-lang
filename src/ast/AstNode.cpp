#include "AstNode.hpp"
#include "errors/Exceptions.hpp"

LValue AstNode::getLValueUnsafe(const std::source_location& loc) const {
    auto lValue = getLValue();
    if (not lValue.has_value()) {
        throw InternalError{"Tried to extract LValue from a node that doesn't support it", loc};
    }
    return lValue.value();
}
