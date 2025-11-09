#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>
#include"message.h"
#include<vector>
#include<limits>

using namespace std;

HANDLE start_process(const wstring& command_line) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    wchar_t* cmd_line = _wcsdup(command_line.c_str());

    wcout << L"Starting: " << command_line << endl;

    BOOL success = CreateProcessW(
        NULL,
        cmd_line,
        NULL,
        NULL,
        FALSE,
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        LPSTARTUPINFOW(&si),
        &pi
    );

    free(cmd_line);

    if (!success) {
        DWORD error = GetLastError();
        wcout << L"CreateProcess error: " << error << endl;
        return NULL;
    }

    CloseHandle(pi.hThread);
    return pi.hProcess;
}

int main() {
    wstring file_name;
    wcout << L"Enter file name:";
    wcin >> file_name;

    int number_of_records;
    wcout << L"Enter number of records:";
    cin >> number_of_records;

    fstream file(file_name.c_str(), ios::binary | ios::out);
    if (!file.is_open()) {
        wcout << L"Error creating file!" << endl;
        return -1;
    }

    message empty_msg;
    for (int i = 0; i < number_of_records; ++i) {
        file << empty_msg;
    }
    file.close();
    wcout << L"File created successfully with " << number_of_records << L" records" << endl;

    int number_of_senders;
    wcout << L"Enter number of Senders:";
    cin >> number_of_senders;

    HANDLE mutex = CreateMutexA(NULL, FALSE, "mutex");
    HANDLE write_sem = CreateSemaphoreA(NULL, number_of_records, number_of_records, "write_sem");
    HANDLE read_sem = CreateSemaphoreA(NULL, 0, number_of_records, "read_sem");

    if (!mutex || !write_sem || !read_sem){
        wcout << L"Error creating sync objects" << endl;
        return -1;
    }
    wcout << L"Sync objects created" << endl;

    HANDLE* senders = new HANDLE[number_of_senders];
    HANDLE* events = new HANDLE[number_of_senders];

    for (int i = 0; i < number_of_senders; ++i) {
        wstring command_line = L"Sender.exe " + file_name + L" " + to_wstring(number_of_records) + L" " + to_wstring(i);

        string event_name = to_string(i) + "ready";
        HANDLE event = CreateEventA(NULL, FALSE, FALSE, event_name.c_str());
        events[i] = event;

        senders[i] = start_process(command_line);
        if (senders[i] == NULL) {
            wcout << L"Failed to create sender " << i << endl;
            return -1;
        }
        wcout << L"Sender " << i << L" started successfully" << endl;
    }

    wcout << L"Waiting for senders to be ready..." << endl;
    WaitForMultipleObjects(number_of_senders, events, TRUE, INFINITE);
    wcout << L"All senders ready!" << endl;

    int read_index = 0;
    int action;
    bool running = true;

    while (running) {
        wcout << L"1 - read message, 0 - exit: ";

        if (!(cin >> action)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            wcout << L"Invalid input. Please enter 0 or 1." << endl;
            continue;
        }

        if (action == 0) {
            running = false;
        }
        else if (action == 1) {
            DWORD result = WaitForSingleObject(read_sem, 1000);
            if (result == WAIT_OBJECT_0) {
                WaitForSingleObject(mutex, INFINITE);

                file.open(file_name.c_str(), ios::binary | ios::in | ios::out);
                if (!file.is_open()) {
                    wcout << L"Error opening file" << endl;
                    ReleaseMutex(mutex);
                    continue;
                }

                message mes;
                file.seekg(read_index * sizeof(message));
                file >> mes;

                if (!mes.is_empty()) {
                    wcout << L"Message: " << mes.get_text() << endl;

                    message empty;
                    file.seekp(read_index * sizeof(message));
                    file << empty;

                    read_index = (read_index + 1) % number_of_records;
                } else {
                    wcout << L"No message at position " << read_index << endl;
                }

                file.close();
                ReleaseMutex(mutex);
                ReleaseSemaphore(write_sem, 1, NULL);
            }
            else if (result == WAIT_TIMEOUT) {
                wcout << L"No messages available" << endl;
            }
        }
        else {
            wcout << L"Unknown command. Please enter 0 or 1." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    wcout << L"Terminating sender processes..." << endl;
    for (int i = 0; i < number_of_senders; ++i) {
        if (senders[i]) {
            TerminateProcess(senders[i], 0);
            WaitForSingleObject(senders[i], 1000);
            CloseHandle(senders[i]);
        }
        if (events[i]) {
            CloseHandle(events[i]);
        }
    }

    delete[] events;
    delete[] senders;
    CloseHandle(mutex);
    CloseHandle(read_sem);
    CloseHandle(write_sem);

    wcout << L"Receiver finished" << endl;
    return 0;
}