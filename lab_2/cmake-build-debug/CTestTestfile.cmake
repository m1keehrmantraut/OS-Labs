# CMake generated Testfile for 
# Source directory: E:/OS_practice/lab_2
# Build directory: E:/OS_practice/lab_2/cmake-build-debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(ThreadTests "E:/OS_practice/lab_2/cmake-build-debug/run_tests.exe")
set_tests_properties(ThreadTests PROPERTIES  _BACKTRACE_TRIPLES "E:/OS_practice/lab_2/CMakeLists.txt;24;add_test;E:/OS_practice/lab_2/CMakeLists.txt;0;")
subdirs("_deps/googletest-build")
