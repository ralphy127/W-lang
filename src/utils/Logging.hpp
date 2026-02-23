#pragma once

#include <sstream>
#include <source_location>
#include <fstream>
#include <string_view>

namespace logger::detail {

class LogStream {
public:
    LogStream(std::string_view level, std::source_location loc);
    ~LogStream();
    
    template <typename T>
    LogStream& operator<<(const T& value) {
        _stream << value;
        return *this;
    }

private:
    std::ostringstream _stream;
    std::string_view _level;
    std::source_location _location;
};

}

#define LOG_DEBUG logger::detail::LogStream("DEBUG", std::source_location::current())
#define LOG_INFO logger::detail::LogStream("INFO", std::source_location::current())
#define LOG_WARN logger::detail::LogStream("WARNING", std::source_location::current())
#define LOG_ERROR logger::detail::LogStream("ERROR", std::source_location::current())
