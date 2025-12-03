cmake_minimum_required(VERSION 3.30)
project(lab_3_cpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BOOST_INCLUDE_DIR "C:/vcpkg/installed/x64-windows/include")
include_directories(${BOOST_INCLUDE_DIR} include)

# Main executable
add_executable(lab_3 src/main.cpp src/marker_app.cpp)

# Test executable
add_executable(test_marker test/test_marker.cpp src/marker_app.cpp)

if(WIN32)
    target_link_libraries(lab_3 Kernel32.lib)
    target_link_libraries(test_marker Kernel32.lib)
endif()

target_include_directories(lab_3 PRIVATE include)
target_include_directories(test_marker PRIVATE include)