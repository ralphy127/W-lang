add_executable(vm_tests VMTests.cpp)
target_link_libraries(vm_tests PRIVATE vm GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(vm_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
