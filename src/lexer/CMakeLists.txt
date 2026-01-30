cmake_minimum_required(VERSION 3.10)
project(lexer)

add_library(lexer
    src/Lexer.cpp
)

target_include_directories(lexer PUBLIC include/lexer)
