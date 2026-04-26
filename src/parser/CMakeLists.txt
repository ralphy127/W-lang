add_library(parser
    Parser.cpp
    Parser.hpp
    ErrorMsgBuilder.hpp
    ErrorMsgBuilder.cpp
)

target_include_directories(parser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(parser PUBLIC token utils ast)