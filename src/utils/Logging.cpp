#include "Logging.hpp"
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

std::uint8_t logLevel{logLevelDebug};

namespace logger::detail {

namespace {
class LogFileHandler {
public:
    static inline std::ofstream& getStream() {
        static LogFileHandler instance;
        return instance._file;
    }

private:
    LogFileHandler() = default;

    std::ofstream _file{"./logs.txt", std::ios::app};
};
}

LogStream::LogStream(std::string_view level, std::source_location loc)
    : _level(level)
    , _location(loc) {}

LogStream::~LogStream() {
    const auto filepath = std::filesystem::path(_location.file_name()).string();
    
    std::string prefix{"[UNKNOWN] "};
    if (filepath.find("lexer") != std::string::npos) {
        prefix = std::string{"[LEXER] "};
    }
    if (filepath.find("parser") != std::string::npos) {
        prefix = std::string{"[PARSER] "};
    }
    if (filepath.find("interpreter") != std::string::npos) {
        prefix = std::string{"[INTERPRETER] "};
    }

    const auto now = std::chrono::system_clock::now();
    const auto time_t_now = std::chrono::system_clock::to_time_t(now);
    const auto ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream time_ss{};
    time_ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    time_ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    auto& file = LogFileHandler::getStream();
    if (file.is_open()) {
        file << "[" << time_ss.str() << "] "
             << "[" << _level << "] " 
             << prefix 
             << "[" << filepath << ":" << _location.line() << "] " 
             << _stream.str() << "\n";
        file.flush();
    }
}

}
