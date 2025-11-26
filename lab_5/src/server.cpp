#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <vector>
#include "../include/employee.h"

using namespace std;

int main() {
    string filename;
    cout << "Enter file name: ";
    cin >> filename;

    int num_employees;
    cout << "Enter number of employees: ";
    cin >> num_employees;

    vector<employee> employees;
    for (int i = 0; i < num_employees; ++i) {
        int id;
        string name;
        double hours;

        cout << "Employee " << (i + 1) << " ID: ";
        cin >> id;
        cout << "Name: ";
        cin >> name;
        cout << "Hours: ";
        cin >> hours;

        employees.push_back(employee(id, name, hours));
    }

    fstream file(filename.c_str(), ios::binary | ios::out);
    if (!file.is_open()) {
        cout << "Error creating file" << endl;
        return -1;
    }

    for (const auto& emp : employees) {
        file << emp;
    }
    file.close();

    cout << "\nFile created with " << num_employees << " records" << endl;
    cout << "\nFile contents:" << endl;

    file.open(filename.c_str(), ios::binary | ios::in);
    employee emp;
    int count = 0;
    while (file >> emp) {
        cout << "ID: " << emp.get_num() << ", Name: " << emp.get_name()
             << ", Hours: " << emp.get_hours() << endl;
        count++;
    }
    file.close();

    int num_clients;
    cout << "\nEnter number of clients: ";
    cin >> num_clients;

    HANDLE* clients = new HANDLE[num_clients];
    for (int i = 0; i < num_clients; ++i) {
        string cmd = "client.exe " + filename + " " + to_string(i);
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        char cmd_line[256];
        strcpy(cmd_line, cmd.c_str());

        BOOL success = CreateProcessA(
            NULL,
            cmd_line,
            NULL,
            NULL,
            FALSE,
            CREATE_NEW_CONSOLE,
            NULL,
            NULL,
            &si,
            &pi
        );

        if (!success) {
            cout << "Failed to create client " << i << endl;
            return -1;
        }

        clients[i] = pi.hProcess;
        CloseHandle(pi.hThread);
        cout << "Client " << i << " started" << endl;
    }

    cout << "\nWaiting for all clients to finish..." << endl;
    WaitForMultipleObjects(num_clients, clients, TRUE, INFINITE);

    cout << "\nAll clients finished. Final file contents:" << endl;
    file.open(filename.c_str(), ios::binary | ios::in);
    while (file >> emp) {
        cout << "ID: " << emp.get_num() << ", Name: " << emp.get_name()
             << ", Hours: " << emp.get_hours() << endl;
    }
    file.close();

    for (int i = 0; i < num_clients; ++i) {
        CloseHandle(clients[i]);
    }
    delete[] clients;

    cout << "\nServer finished" << endl;
    return 0;
}
