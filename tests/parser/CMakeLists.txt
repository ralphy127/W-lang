add_executable(parser_tests ParserTests.cpp)
target_link_libraries(parser_tests PRIVATE parser lexer ast GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(parser_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
