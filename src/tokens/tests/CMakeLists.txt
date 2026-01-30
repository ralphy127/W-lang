add_executable(tokens_tests TokenTests.cpp)

target_link_libraries(tokens_tests 
    PRIVATE 
        tokens 
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(tokens_tests)