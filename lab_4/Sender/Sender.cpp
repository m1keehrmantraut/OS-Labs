#include<iostream>
#include<fstream>
#include<string>
#include<Windows.h>
#include"../Receiver/message.h"
#include<vector>
#include<limits>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Not enough arguments" << endl;
        return -1;
    }

    cout << "Sender started with file: " << argv[1] << endl;

    string event_name = to_string(atoi(argv[3])) + "ready";
    HANDLE ready = OpenEventA(EVENT_MODIFY_STATE, FALSE, event_name.c_str());
    HANDLE mutex = OpenMutexA(MUTEX_ALL_ACCESS, FALSE, "mutex");
    HANDLE write_sem = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "write_sem");
    HANDLE read_sem = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, "read_sem");

    if (!ready || !mutex || !write_sem || !read_sem) {
        cout << "Error opening sync objects" << endl;
        return -1;
    }

    cout << "Sync objects opened" << endl;
    SetEvent(ready);
    cout << "Ready signal sent" << endl;

    int write_index = 0;
    int action;
    bool running = true;
    int number_of_records = atoi(argv[2]);

    while (running) {
        cout << "1 - write message, 0 - exit: ";

        if (!(cin >> action)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter 0 or 1." << endl;
            continue;
        }

        if (action == 0) {
            running = false;
        }
        else if (action == 1) {
            cin.ignore();
            string text;
            cout << "Enter message: ";
            getline(cin, text);

            if (text.length() >= 20) {
                cout << "Message too long" << endl;
                continue;
            }

            DWORD result = WaitForSingleObject(write_sem, 1000);
            if (result == WAIT_OBJECT_0) {
                WaitForSingleObject(mutex, INFINITE);

                fstream file(argv[1], ios::binary | ios::in | ios::out);
                if (!file.is_open()) {
                    cout << "Error opening file" << endl;
                    ReleaseMutex(mutex);
                    ReleaseSemaphore(write_sem, 1, NULL);
                    continue;
                }

                bool written = false;
                for (int i = 0; i < number_of_records; ++i) {
                    int index = (write_index + i) % number_of_records;
                    message mes;
                    file.seekg(index * sizeof(message));
                    file >> mes;

                    if (mes.is_empty()) {
                        message new_msg(text);
                        file.seekp(index * sizeof(message));
                        file << new_msg;
                        written = true;
                        write_index = (index + 1) % number_of_records;
                        break;
                    }
                }

                file.close();

                if (written) {
                    ReleaseMutex(mutex);
                    ReleaseSemaphore(read_sem, 1, NULL);
                    cout << "Message sent: " << text << endl;
                } else {
                    cout << "No space in file" << endl;
                    ReleaseMutex(mutex);
                    ReleaseSemaphore(write_sem, 1, NULL);
                }
            }
            else if (result == WAIT_TIMEOUT) {
                cout << "No space available, try again later" << endl;
            }
        }
        else {
            cout << "Unknown command. Please enter 0 or 1." << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }

    CloseHandle(mutex);
    CloseHandle(ready);
    CloseHandle(write_sem);
    CloseHandle(read_sem);

    cout << "Sender finished" << endl;
    return 0;
}