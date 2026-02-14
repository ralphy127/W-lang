add_library(parser
    Parser.cpp
    Parser.hpp
)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
