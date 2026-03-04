add_library(interpreter
    Interpreter.cpp
    Interpreter.hpp
)

target_include_directories(interpreter PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(interpreter PUBLIC ast utils runtime)
