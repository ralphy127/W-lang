add_executable(VMTests VMTests.cpp)
target_link_libraries(VMTests vm)
target_include_directories(VMTests PRIVATE ../include/vm)
