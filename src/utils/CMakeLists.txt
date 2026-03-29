add_library(utils
    Logging.cpp
    Logging.hpp
    AstPrinter.cpp
    AstPrinter.hpp
)

target_include_directories(utils PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(utils PUBLIC ast)
