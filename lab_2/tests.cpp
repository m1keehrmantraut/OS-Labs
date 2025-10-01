#include <gtest/gtest.h>
#include <windows.h>
#include <vector>
#include <iostream>

CRITICAL_SECTION cs;
std::vector<int> arr;
int min_val, max_val, average_val;

DWORD WINAPI min_max(LPVOID param) {
    if (arr.empty()) return 0;

    EnterCriticalSection(&cs);
    min_val = max_val = arr[0];
    LeaveCriticalSection(&cs);

    for (size_t i = 1; i < arr.size(); i++) {
        EnterCriticalSection(&cs);
        if (arr[i] < min_val) min_val = arr[i];
        LeaveCriticalSection(&cs);

        EnterCriticalSection(&cs);
        if (arr[i] > max_val) max_val = arr[i];
        LeaveCriticalSection(&cs);
    }

    return 0;
}

DWORD WINAPI average(LPVOID param) {
    if (arr.empty()) {
        average_val = 0;
        return 0;
    }

    EnterCriticalSection(&cs);
    long long sum = 0;
    for (size_t i = 0; i < arr.size(); i++) {
        sum += arr[i];
        LeaveCriticalSection(&cs);
        EnterCriticalSection(&cs);
    }
    average_val = static_cast<int>(sum / arr.size());
    LeaveCriticalSection(&cs);
    return 0;
}

class ThreadTest : public ::testing::Test {
protected:
    void SetUp() override {
        InitializeCriticalSection(&cs);
        arr.clear();
        min_val = 0;
        max_val = 0;
        average_val = 0;
    }
    void TearDown() override {
        DeleteCriticalSection(&cs);
    }
};

TEST_F(ThreadTest, MinMaxCalculation) {
    arr = {5, 3, 8, 1, 9};

    HANDLE hThread = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    EXPECT_EQ(min_val, 1);
    EXPECT_EQ(max_val, 9);
}

TEST_F(ThreadTest, AverageCalculation) {
    arr = {1, 2, 3, 4, 5};

    HANDLE hThread = CreateThread(NULL, 0, average, NULL, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);

    EXPECT_EQ(average_val, 3);
}

TEST_F(ThreadTest, SingleElement) {
    arr = {7};

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    EXPECT_EQ(min_val, 7);
    EXPECT_EQ(max_val, 7);
    EXPECT_EQ(average_val, 7);
}

TEST_F(ThreadTest, EmptyArray) {
    arr = {};

    HANDLE hMinMax = CreateThread(NULL, 0, min_max, NULL, 0, NULL);
    HANDLE hAverage = CreateThread(NULL, 0, average, NULL, 0, NULL);

    WaitForSingleObject(hMinMax, INFINITE);
    WaitForSingleObject(hAverage, INFINITE);

    CloseHandle(hMinMax);
    CloseHandle(hAverage);

    EXPECT_EQ(min_val, 0);
    EXPECT_EQ(max_val, 0);
    EXPECT_EQ(average_val, 0);
}

TEST_F(ThreadTest, ReplaceMinMaxWithAverage) {
    arr = {5, 3, 8, 1, 9};

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

    EXPECT_EQ(arr[3], average_val);
    EXPECT_EQ(arr[4], average_val);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}