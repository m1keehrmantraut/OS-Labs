#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include "../include/employee.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Not enough arguments" << endl;
        return -1;
    }

    string filename = argv[1];
    string client_id = argv[2];

    cout << "Client " << client_id << " started" << endl;

    int action;
    bool running = true;

    while (running) {
        cout << "\n1 - read, 2 - modify, 0 - exit: ";
        cin >> action;

        if (action == 0) {
            running = false;
        }
        else if (action == 1) {
            int id;
            cout << "Enter employee ID: ";
            cin >> id;

            fstream file(filename.c_str(), ios::binary | ios::in);
            if (!file.is_open()) {
                cout << "Error opening file" << endl;
                continue;
            }

            employee emp;
            bool found = false;
            while (file >> emp) {
                if (emp.get_num() == id) {
                    cout << "Found: ID: " << emp.get_num() << ", Name: "
                         << emp.get_name() << ", Hours: " << emp.get_hours() << endl;
                    found = true;
                    break;
                }
            }

            if (!found) {
                cout << "Employee not found" << endl;
            }

            file.close();
        }
        else if (action == 2) {
            int id;
            cout << "Enter employee ID: ";
            cin >> id;

            fstream file(filename.c_str(), ios::binary | ios::in | ios::out);
            if (!file.is_open()) {
                cout << "Error opening file" << endl;
                continue;
            }

            employee emp;
            bool found = false;
            int record_pos = 0;
            int current_pos = 0;

            while (file >> emp) {
                if (emp.get_num() == id) {
                    cout << "Found: ID: " << emp.get_num() << ", Name: "
                         << emp.get_name() << ", Hours: " << emp.get_hours() << endl;
                    found = true;
                    record_pos = current_pos;
                    break;
                }
                current_pos++;
            }

            if (found) {
                string new_name;
                double new_hours;

                cout << "Enter new name: ";
                cin >> new_name;
                cout << "Enter new hours: ";
                cin >> new_hours;

                emp.set_name(new_name);
                emp.set_hours(new_hours);

                file.seekp(record_pos * sizeof(employee));
                file << emp;

                cout << "Record updated" << endl;
            } else {
                cout << "Employee not found" << endl;
            }

            file.close();
        }
        else {
            cout << "Invalid action" << endl;
        }
    }

    cout << "Client " << client_id << " finished" << endl;
    return 0;
}
