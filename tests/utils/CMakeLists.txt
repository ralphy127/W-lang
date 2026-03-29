add_executable(ast_printer_tests AstPrinterTests.cpp)

target_link_libraries(ast_printer_tests PRIVATE utils lexer parser GTest::gtest_main)

include(GoogleTest)

gtest_discover_tests(ast_printer_tests WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
