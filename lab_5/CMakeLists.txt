cmake_minimum_required(VERSION 3.10)
project(Lab5NamedPipes)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(server src/server.cpp include/employee.h)
add_executable(client src/client.cpp include/employee.h)
add_executable(test_employee test/test_employee.cpp include/employee.h)

if(WIN32)
    target_link_libraries(server Kernel32.lib)
    target_link_libraries(client Kernel32.lib)
endif()
