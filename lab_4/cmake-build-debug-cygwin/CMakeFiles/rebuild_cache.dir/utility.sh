set -e

cd /cygdrive/e/OS_practice/lab_4/cmake-build-debug-cygwin
/cygdrive/c/Users/Mike/AppData/Local/JetBrains/CLion2024.3/cygwin_cmake/bin/cmake.exe --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
