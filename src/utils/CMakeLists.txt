add_library(utils
    Utils.cpp
    Utils.hpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
