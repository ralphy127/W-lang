
#include "ErrorMsgBuilder.hpp"

#include <format>

ErrorMsgBuilder::ErrorMsgBuilder(std::string&& message) : _message(std::move(message)) {}

ErrorMsgBuilder ErrorMsgBuilder::expected(std::string_view what) {
    return ErrorMsgBuilder{std::format("Bruh, missing {}", what)};
}

ErrorMsgBuilder ErrorMsgBuilder::cannot(std::string_view thing) {
    return ErrorMsgBuilder{std::format("Nice try, but you can't {}", thing)};
}

ErrorMsgBuilder ErrorMsgBuilder::unclosed(std::string_view thing) {
    return ErrorMsgBuilder{std::format("Yo, you left {} wide open", thing)};
}

ErrorMsgBuilder& ErrorMsgBuilder::after(std::string_view what) {
    _message += std::format(" right after {}", what);
    return *this;
}

ErrorMsgBuilder& ErrorMsgBuilder::need(std::string_view unexpected) {
    _message += std::format(", need {}", unexpected);
    return *this;
}
