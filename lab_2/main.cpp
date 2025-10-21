#include <iostream>
#include <windows.h>
#include <vector>

CRITICAL_SECTION cs;
std::vector<int> arr;
int min_val, max_val, average_val;

DWORD WINAPI min_max(LPVOID param) {
    if (arr.empty()) return 0;

    //EnterCriticalSection(&cs);
    min_val = max_val = arr[0];
    //LeaveCriticalSection(&cs);

    for (size_t i = 1; i < arr.size(); i++) {
        //EnterCriticalSection(&cs);
        if (arr[i] < min_val) min_val = arr[i];
        //LeaveCriticalSection(&cs);
        Sleep(7);

        //EnterCriticalSection(&cs);
        if (arr[i] > max_val) max_val = arr[i];
        //LeaveCriticalSection(&cs);
        Sleep(7);
    }

    std::cout << "Min: " << min_val << std::endl;
    std::cout << "Max: " << max_val << std::endl;
    return 0;
}

DWORD WINAPI average(LPVOID param) {
    if (arr.empty()) {
        average_val = 0;
        std::cout << "Average: " << average_val << std::endl;
        return 0;
    }

    //EnterCriticalSection(&cs);
    long long sum = 0;
    for (size_t i = 0; i < arr.size(); i++) {
        sum += arr[i];
        //LeaveCriticalSection(&cs);
        Sleep(12);
        //EnterCriticalSection(&cs);
    }
    average_val = static_cast<int>(sum / arr.size());
    //
    //LeaveCriticalSection(&cs);

    std::cout << "Average: " << average_val << std::endl;
    return 0;
}

int main() {
    InitializeCriticalSection(&cs);

    int n;
    std::cout << "Enter array size: ";
    std::cin >> n;

    arr.resize(n);
    std::cout << "Enter array elements: ";
    for (int i = 0; i < n; i++) {
        std::cin >> arr[i];
    }

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    for (size_t i = 0; i < arr.size(); i++) {
        if (arr[i] == min_val || arr[i] == max_val) {
            arr[i] = average_val;
        }
    }

    std::cout << "Final array: ";
    for (size_t i = 0; i < arr.size(); i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    DeleteCriticalSection(&cs);
    return 0;
}