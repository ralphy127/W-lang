add_executable(lexer_tests LexerTests.cpp)
target_link_libraries(lexer_tests PRIVATE lexer GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(lexer_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
