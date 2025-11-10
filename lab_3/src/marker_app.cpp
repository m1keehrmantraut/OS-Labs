#include "../include/marker_app.hpp"
#include <cstdlib>

CRITICAL_SECTION cs;

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

DWORD WINAPI thread_func(LPVOID params) {
    thread_info info = *static_cast<thread_info*>(params);
    bool end_thread = false;
    int number_of_marked_elements = 0;

    std::srand(info.thread_index);

    WaitForSingleObject(info.start_work, INFINITE);

    while (!end_thread) {
        int ind = std::rand() % info.array_size;
        EnterCriticalSection(&cs);
        if (info.arr[ind] == 0) {
            Sleep(5);
            info.arr[ind] = info.thread_index;
            LeaveCriticalSection(&cs);
            ++number_of_marked_elements;
            Sleep(5);
        } else {
            std::cout << "\nThread " << info.thread_index
                      << ", number of marked elements: " << number_of_marked_elements
                      << ", can't mark element with index " << ind;
            LeaveCriticalSection(&cs);
            SetEvent(info.stop_work);
            DWORD res = WaitForMultipleObjects(2, info.terminate_or_continue, FALSE, INFINITE);
            int k = static_cast<int>(res - WAIT_OBJECT_0);
            if (k == 0) {
                end_thread = true;
            }
        }
    }

    set_zeros(info.arr, info.array_size, info.thread_index);
    return 0;
}
