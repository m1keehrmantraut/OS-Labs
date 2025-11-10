cmake_minimum_required(VERSION 3.10)
project(ProcessSync)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(Receiver Receiver/Receiver.cpp Receiver/message.h)
add_executable(Sender Sender/Sender.cpp Receiver/message.h)
add_executable(test_message test_message.cpp Receiver/message.h)

if(WIN32)
    target_link_libraries(Receiver Kernel32.lib)
    target_link_libraries(Sender Kernel32.lib)
endif()
