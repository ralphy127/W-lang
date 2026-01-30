add_executable(LexerTests LexerTests.cpp)
target_link_libraries(LexerTests lexer)
target_include_directories(LexerTests PRIVATE ../include/lexer)
