add_executable(interpreter_tests
	InterpreterTests.cpp
	NativeTypesInterpreterTests.cpp
	InterpreterTestsFixture.hpp
)
target_link_libraries(interpreter_tests PRIVATE interpreter lexer parser GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(interpreter_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
