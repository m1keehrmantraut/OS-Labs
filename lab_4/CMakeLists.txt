cmake_minimum_required(VERSION 3.10)
project(lab_4_posix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Receiver receiver.cpp)
add_executable(Sender sender.cpp)
add_executable(test_message test_message.cpp)

if(CYGWIN)
    target_link_libraries(Receiver cygwin rt pthread)
    target_link_libraries(Sender cygwin rt pthread)
    target_compile_options(Receiver PRIVATE -D_GNU_SOURCE)
    target_compile_options(Sender PRIVATE -D_GNU_SOURCE)
endif()

enable_testing()
add_test(NAME MessageTests COMMAND test_message)


#cd /cygdrive/e/OS_practice/lab_4/cmake-build-debug
#./Receiver.exe
#./Sender.exe 0