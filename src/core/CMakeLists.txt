add_library(core
    types.hpp
    SourceRange.hpp
    SourceManager.hpp
    SourceManager.cpp
)


target_include_directories(core INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/..)
