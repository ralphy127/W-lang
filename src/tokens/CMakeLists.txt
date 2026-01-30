add_library(tokens 
    src/Token.cpp 
    include/tokens/Token.hpp
)

target_include_directories(tokens PUBLIC include)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()