add_executable(vm_tests VMTests.cpp)
target_link_libraries(vm_tests PRIVATE vm GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(vm_tests)
