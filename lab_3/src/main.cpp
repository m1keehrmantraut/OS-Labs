#include "../include/marker_app.hpp"
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

int main() {
    int size;
    cout << "Enter array size: ";
    cin >> size;

    int* arr = new int[size];
    for (int i = 0; i < size; ++i) {
        arr[i] = 0;
    }

    cout << "Enter number of marker threads: ";
    int number_of_threads;
    cin >> number_of_threads;

    vector<ThreadInfo> thread_infos(number_of_threads);
    vector<thread> threads;
    vector<bool> terminated_threads(number_of_threads, false);

    for (int i = 0; i < number_of_threads; ++i) {
        thread_infos[i].arr = arr;
        thread_infos[i].array_size = size;
        thread_infos[i].thread_index = i + 1;
        threads.emplace_back(thread_func, &thread_infos[i]);
    }

    for (int i = 0; i < number_of_threads; ++i) {
        thread_infos[i].should_start = true;
    }

    int ended_threads = 0;

    while (ended_threads != number_of_threads) {
        {
            unique_lock<mutex> lock(global_mutex);
            bool all_blocked = false;
            while (!all_blocked) {
                all_blocked = true;
                for (int i = 0; i < number_of_threads; ++i) {
                    if (!terminated_threads[i] && !thread_infos[i].is_blocked) {
                        all_blocked = false;
                        break;
                    }
                }
                if (!all_blocked) {
                    lock.unlock();
                    this_thread::sleep_for(chrono::milliseconds(10));
                    lock.lock();
                }
            }
        }

        cout << "\n";
        print_array(arr, size);

        bool is_thread_terminated = false;
        while (!is_thread_terminated) {
            int thread_to_terminate_ind;
            cout << "Enter index of thread to be terminated (starts with 1): ";
            cin >> thread_to_terminate_ind;
            thread_to_terminate_ind--;

            if (thread_to_terminate_ind >= number_of_threads || thread_to_terminate_ind < 0) {
                cout << "No thread with such index\n";
                continue;
            }

            if (terminated_threads[thread_to_terminate_ind]) {
                cout << "Thread is already terminated\n";
                continue;
            }

            thread_infos[thread_to_terminate_ind].should_terminate = true;

            threads[thread_to_terminate_ind].join();

            print_array(arr, size);
            terminated_threads[thread_to_terminate_ind] = true;
            is_thread_terminated = true;
            ended_threads++;
        }

        for (int j = 0; j < number_of_threads; ++j) {
            if (!terminated_threads[j]) {
                thread_infos[j].can_continue = true;
                thread_infos[j].is_blocked = false;
            }
        }
    }

    cout << "All threads are terminated\n";

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    delete[] arr;

    return 0;
}
