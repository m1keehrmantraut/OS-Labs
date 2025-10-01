cmake_minimum_required(VERSION 3.14)
project(OS_practice_lab_2)

set(CMAKE_CXX_STANDARD 17)
enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/heads/main.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(main_app main.cpp)

add_executable(run_tests tests.cpp)
target_link_libraries(run_tests gtest_main)

add_test(NAME ThreadTests COMMAND run_tests)