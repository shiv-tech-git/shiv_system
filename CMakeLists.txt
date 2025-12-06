cmake_minimum_required(VERSION 3.16)

# Project name and language
project(shiv_sys
    VERSION 1.0
    LANGUAGES C CXX
)

include(FetchContent)

# Download GoogleTest
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)

FetchContent_MakeAvailable(googletest)
enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(cpu)
add_subdirectory(as)
