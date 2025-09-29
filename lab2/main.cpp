#include <iostream>
#include <Windows.h>

using namespace std;

#define SLEEP_MINMAX 7
#define SLEEP_AVERAGE 12

struct array_data {
    array_data(int* arr, int size) : arr(arr), size(size), min(0), max(0), average(0) {}
    int* arr;
    int min;
    int max;
    int average;
    int size;
};

DWORD WINAPI average_func(LPVOID arguments) {
    array_data* pArr = (array_data*)arguments;

    long long sum = 0;
    for (int i = 0; i < pArr->size; ++i) {
        sum += pArr->arr[i];
        Sleep(SLEEP_AVERAGE);
    }

    pArr->average = static_cast<int>(sum / pArr->size);
    cout << "Array average: " << pArr->average << endl;

    return 0;
}

DWORD WINAPI min_max_element(LPVOID arguments) {
    array_data* pArr = (array_data*)arguments;
    pArr->max = pArr->min = pArr->arr[0];

    for (int i = 1; i < pArr->size; ++i) {
        if (pArr->max < pArr->arr[i]) {
            pArr->max = pArr->arr[i];
        }
        Sleep(SLEEP_MINMAX);

        if (pArr->min > pArr->arr[i]) {
            pArr->min = pArr->arr[i];
        }
        Sleep(SLEEP_MINMAX);
    }

    cout << "Max element of array: " << pArr->max << endl;
    cout << "Min element of array: " << pArr->min << endl;
    return 0;
}

void replace_minmax_with_average(array_data* pArr) {
    for (int i = 0; i < pArr->size; ++i) {
        if (pArr->arr[i] == pArr->max || pArr->arr[i] == pArr->min) {
            pArr->arr[i] = pArr->average;
        }
    }
}

void print_array(array_data* pArr) {
    cout << "Result array: ";
    for (int i = 0; i < pArr->size; ++i) {
        cout << pArr->arr[i] << " ";
    }
    cout << endl;
}

int main() {
    int size;
    cout << "Enter array size: ";
    cin >> size;

    if (size <= 0) {
        cout << "Invalid array size!" << endl;
        return 1;
    }

    int* arr = new int[size];
    cout << "Enter array elements: ";
    for (int i = 0; i < size; ++i) {
        cin >> arr[i];
    }

    array_data* data = new array_data(arr, size);

    HANDLE hThread_min_max = CreateThread(NULL, 0, min_max_element, (LPVOID)data, 0, NULL);
    HANDLE hThread_average = CreateThread(NULL, 0, average_func, (LPVOID)data, 0, NULL);

    if (hThread_min_max == NULL || hThread_average == NULL) {
        cerr << "Error creating threads!" << endl;
        delete[] arr;
        delete data;
        return GetLastError();
    }

    WaitForSingleObject(hThread_min_max, INFINITE);
    WaitForSingleObject(hThread_average, INFINITE);

    CloseHandle(hThread_min_max);
    CloseHandle(hThread_average);

    cout << endl;

    replace_minmax_with_average(data);
    print_array(data);

    delete[] arr;
    delete data;

    return 0;
}
