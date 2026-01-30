cmake_minimum_required(VERSION 3.10)
project(vm)

add_library(vm
    src/VM.cpp
)

target_include_directories(vm PUBLIC include/vm)
