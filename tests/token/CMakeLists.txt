add_executable(token_tests TokenTests.cpp)
target_link_libraries(token_tests PRIVATE token GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(token_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
