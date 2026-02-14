add_executable(utils_tests UtilsTests.cpp)
target_link_libraries(utils_tests PRIVATE utils GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(utils_tests)
