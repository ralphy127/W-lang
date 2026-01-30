cmake_minimum_required(VERSION 3.10)
project(parser)

add_library(parser
    src/Parser.cpp
)

target_include_directories(parser PUBLIC include/parser)
