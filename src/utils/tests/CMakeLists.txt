add_executable(UtilsTests UtilsTests.cpp)
target_link_libraries(UtilsTests utils)
target_include_directories(UtilsTests PRIVATE ../include/utils)
