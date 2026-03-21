add_library(errors
    ErrorReporter.hpp
    ErrorReporter.cpp
)

target_include_directories(errors PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(errors PUBLIC runtime)