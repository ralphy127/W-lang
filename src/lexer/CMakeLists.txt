add_library(lexer
    Lexer.cpp
    Lexer.hpp
)

target_include_directories(lexer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(lexer PUBLIC token)
