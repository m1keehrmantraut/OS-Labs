#ifndef MARKER_APP_HPP
#define MARKER_APP_HPP

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <atomic>

struct ThreadInfo {
    int array_size;
    int* arr;
    int thread_index;
    std::atomic<bool> should_start{false};
    std::atomic<bool> should_terminate{false};
    std::atomic<bool> can_continue{false};
    std::atomic<bool> is_blocked{false};
};

extern std::mutex global_mutex;
extern std::condition_variable cv_all_blocked;
extern std::condition_variable cv_thread_unblock;

void set_zeros(int* arr, int size, int thread_index);
void print_array(const int* arr, int size, std::ostream& out = std::cout);
bool all_threads_terminated(const bool* terminated_threads, int size);

void thread_func(ThreadInfo* info);

#endif