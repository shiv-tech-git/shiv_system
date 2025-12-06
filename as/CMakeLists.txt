cmake_minimum_required(VERSION 3.16)
project(as LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

BISON_TARGET(Parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser.y
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
)

FLEX_TARGET(Lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

add_executable(as
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
)

target_include_directories(as PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
