cmake_minimum_required(VERSION 3.16)

project(cpu
    VERSION 1.0
    LANGUAGES C CXX
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(cpu
    src/main.cpp
)

target_include_directories(cpu
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

add_executable(cpu_tests
    tests/test.cpp
)

target_link_libraries(cpu_tests
    gtest_main
)

target_include_directories(cpu_tests
    PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

include(GoogleTest)
gtest_discover_tests(cpu_tests)

