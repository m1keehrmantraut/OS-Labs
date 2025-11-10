#ifndef MARKER_APP_HPP
#define MARKER_APP_HPP

#include <windows.h>
#include <iostream>

struct thread_info {
    int array_size;
    int* arr;
    int thread_index;
    HANDLE stop_work;
    HANDLE start_work;
    HANDLE* terminate_or_continue;
};

extern CRITICAL_SECTION cs;

void set_zeros(int* arr, int size, int thread_index);
void print_array(const int* arr, int size, std::ostream& out = std::cout);
bool all_threads_terminated(const bool* terminated_threads, int size);

DWORD WINAPI thread_func(LPVOID params);

#endif