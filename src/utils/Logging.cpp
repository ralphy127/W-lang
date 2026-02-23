#include "Logging.hpp"
#include <filesystem>
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace logger::detail {

namespace {
class LogFileHandler {
public:
    static inline std::ofstream& getStream() {
        static LogFileHandler instance;
        return instance._file;
    }

private:
    std::ofstream _file;
    LogFileHandler() {
        std::filesystem::path p = std::filesystem::current_path();
        
        bool found{false};
        for (int i = 0; i < 3; ++i) {
            if (std::filesystem::exists(p / "build")) {
                p /= "build";
                found = true;
                break;
            }
            if (p.has_parent_path()) p = p.parent_path();
        }
        
        if (not found) {
            throw std::logic_error{"file logging error: build folder not found"};
        }
        _file.open(p / "logs.txt", std::ios::app);
    };
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

    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream time_ss;
    time_ss << std::put_time(std::localtime(&time_t_now), "%Y-%m-%d %H:%M:%S");
    time_ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    auto& file = LogFileHandler::getStream();
    if (file.is_open()) {
        file << "[" << time_ss.str() << "] "
             << "[" << _level << "] " 
             << prefix 
             << "[" << filepath << ":" << _location.line() << "] " 
             << _stream.str() << "\n";
    }
}

}
