add_library(vm
    VM.cpp
    VM.hpp
)

target_include_directories(vm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
