#include "../include/marker_app.hpp"
#include <cstdlib>
#include <chrono>
#include <thread>

std::mutex global_mutex;
std::condition_variable cv_all_blocked;
std::condition_variable cv_thread_unblock;

void set_zeros(int* arr, int size, int thread_index) {
    for (int i = 0; i < size; ++i) {
        if (arr[i] == thread_index) {
            arr[i] = 0;
        }
    }
}

void print_array(const int* arr, int size, std::ostream& out) {
    out << "Array: ";
    for (int i = 0; i < size; ++i) {
        out << arr[i] << ' ';
    }
    out << '\n';
}

bool all_threads_terminated(const bool* terminated_threads, int size) {
    for (int i = 0; i < size; ++i) {
        if (!terminated_threads[i]) {
            return false;
        }
    }
    return true;
}

void thread_func(ThreadInfo* info) {
    bool end_thread = false;
    int number_of_marked_elements = 0;

    std::srand(info->thread_index);

    while (!info->should_start) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    while (!end_thread) {
        int ind = std::rand() % info->array_size;

        {
            std::unique_lock<std::mutex> lock(global_mutex);
            if (info->arr[ind] == 0) {
                lock.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(5));

                lock.lock();
                info->arr[ind] = info->thread_index;
                lock.unlock();

                ++number_of_marked_elements;
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
            } else {
                std::cout << "\nThread " << info->thread_index
                          << ", number of marked elements: " << number_of_marked_elements
                          << ", can't mark element with index " << ind << std::endl;
                lock.unlock();

                info->is_blocked = true;
                cv_all_blocked.notify_one();

                while (!info->should_terminate && !info->can_continue) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }

                if (info->should_terminate) {
                    end_thread = true;
                } else {
                    info->can_continue = false;
                    info->is_blocked = false;
                }
            }
        }
    }

    set_zeros(info->arr, info->array_size, info->thread_index);
}