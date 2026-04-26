#pragma once

#include <string>
#include <format>
#include <string_view>

class ErrorMsgBuilder {
public:
    static ErrorMsgBuilder expected(std::string_view what);
    static ErrorMsgBuilder cannot(std::string_view thing);
    static ErrorMsgBuilder unclosed(std::string_view thing);

    ErrorMsgBuilder& after(std::string_view what);
    ErrorMsgBuilder& need(std::string_view unexpected);

    std::string build() const { return _message; }

private:
    ErrorMsgBuilder(std::string&& message);
    
    std::string _message;
};
