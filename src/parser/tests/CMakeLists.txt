add_executable(ParserTests ParserTests.cpp)
target_link_libraries(ParserTests parser)
target_include_directories(ParserTests PRIVATE ../include/parser)
