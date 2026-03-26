#pragma once

#include <string>
#include <vector>
#include "types.hpp"

class SourceManager {
public:
    FileId registerFile(const std::string& filePath);
    const std::string& getFilePath(FileId) const;

private:
    std::vector<std::string> _files{};
};