cmake_minimum_required(VERSION 3.16)
project(Lab1)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${CMAKE_SOURCE_DIR})

add_executable(Creator Creator/creator.cpp)
add_executable(Reporter Reporter/reporter.cpp)
add_executable(Main Main/main.cpp)
