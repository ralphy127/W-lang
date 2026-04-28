add_library(ast
    AstNode.hpp
    AstNode.cpp
    Expressions.cpp
    Expressions.hpp
    Statements.cpp
    Statements.hpp
    AstVisitor.hpp
    LValue.hpp
)

target_include_directories(ast PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(ast PUBLIC token errors)
