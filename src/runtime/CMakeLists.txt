add_library(runtime
    RuntimeValue.hpp
    RuntimeValue.cpp
    Environment.hpp
    Environment.cpp
)

target_include_directories(runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
