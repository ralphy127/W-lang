add_subdirectory(token)
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(utils)
add_subdirectory(vm)
add_subdirectory(ast)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE lexer parser token utils vm)
