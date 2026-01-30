cmake_minimum_required(VERSION 3.10)
project(wlang)

add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(tokens)
add_subdirectory(utils)
add_subdirectory(vm)

add_executable(main main.cpp)
target_link_libraries(main lexer parser tokens utils vm)
target_include_directories(main PRIVATE lexer/include/lexer parser/include/parser tokens/include/tokens utils/include/utils vm/include/vm)
