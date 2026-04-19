#include "SourceManager.hpp"
#include <format>

FileId SourceManager::registerFile(const std::string& filePath) {
    _files.push_back(filePath);
    return _files.size() - 1ull;
}

const std::string& SourceManager::getFilePath(FileId id) const {
    if (id >= _files.size()) {
        throw std::runtime_error{std::format("File with id: {} not found", id)};
    }
    return _files[id];
}
