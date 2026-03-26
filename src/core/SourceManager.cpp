#include "SourceManager.hpp"

FileId SourceManager::registerFile(const std::string& filePath) {
    _files.push_back(filePath);
    return _files.size() - 1ull;
}

const std::string& SourceManager::getFilePath(FileId id) const {
    // TODO better error handling?
    return _files.at(id);
}
