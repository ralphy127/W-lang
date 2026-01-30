cmake_minimum_required(VERSION 3.10)
project(utils)

add_library(utils
    src/Utils.cpp
)

target_include_directories(utils PUBLIC include/utils)
