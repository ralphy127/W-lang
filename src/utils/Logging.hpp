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

constexpr std::uint16_t logLevelDebug{0u};
constexpr std::uint16_t logLevelInfo{1u};
constexpr std::uint16_t logLevelWarn{2u};
constexpr std::uint16_t logLevelError{3u};
extern std::uint8_t logLevel;

#define LOG_DEBUG \
    if (logLevel > logLevelDebug) {} \
    else logger::detail::LogStream("DEBUG", std::source_location::current())

#define LOG_INFO \
    if (logLevel > logLevelInfo) {} \
    else logger::detail::LogStream("INFO", std::source_location::current())

#define LOG_WARN \
    if (logLevel > logLevelWarn) {} \
    else logger::detail::LogStream("WARN", std::source_location::current())

#define LOG_ERROR \
    if (logLevel > logLevelError) {} \
    else logger::detail::LogStream("ERROR", std::source_location::current())