add_library(token
    Token.cpp
    Token.hpp
)

target_include_directories(token PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
