add_library(utils
    Logging.cpp
    Logging.hpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
