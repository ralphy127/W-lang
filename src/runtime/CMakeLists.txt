add_library(runtime
    RuntimeValue.hpp
    RuntimeValue.cpp
    Environment.hpp
    Environment.cpp
    RuntimeErrors.hpp
)

target_include_directories(runtime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(runtime PUBLIC token)
