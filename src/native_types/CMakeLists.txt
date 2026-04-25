add_library(native_types
    common.cpp
    common.hpp
    String.cpp
    String.hpp
    Vector.cpp
    Vector.hpp
)

target_include_directories(native_types PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(native_types PUBLIC runtime utils)