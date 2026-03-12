add_library(modules
    Gossip.hpp
    Gossip.cpp
)

target_include_directories(modules PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(modules PUBLIC token utils ast)