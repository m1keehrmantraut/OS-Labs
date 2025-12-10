#include <windows.h>
#include <iostream>
#include <string>

#include "../include/employee.h"

using namespace std;

static const char* PIPE_NAME = R"(\\.\pipe\Lab5EmployeePipe)";

enum class RequestType : int {
    READ = 1,
    MODIFY = 2,
    SEND_MODIFIED = 3,
    FINISH_RECORD = 4,
    EXIT_CLIENT = 5
};

enum class ResponseStatus : int {
    OK = 0,
    NOT_FOUND = 1,
    SERVER_ERROR = 2
};

#pragma pack(push, 1)
struct Request {
    RequestType type;
    int id;
    employee emp;
};

struct Response {
    ResponseStatus status;
    employee emp;
};
#pragma pack(pop)

bool read_exact(HANDLE h, void* buf, DWORD size) {
    DWORD read = 0;
    char* p = static_cast<char*>(buf);
    while (read < size) {
        DWORD chunk = 0;
        if (!ReadFile(h, p + read, size - read, &chunk, nullptr)) {
            return false;
        }
        if (chunk == 0) return false;
        read += chunk;
    }
    return true;
}

bool write_exact(HANDLE h, const void* buf, DWORD size) {
    DWORD written = 0;
    const char* p = static_cast<const char*>(buf);
    while (written < size) {
        DWORD chunk = 0;
        if (!WriteFile(h, p + written, size - written, &chunk, nullptr)) {
            return false;
        }
        if (chunk == 0) return false;
        written += chunk;
    }
    return true;
}

int main() {
    cout << "Client starting, connecting to pipe...\n";

    if (!WaitNamedPipeA(PIPE_NAME, NMPWAIT_WAIT_FOREVER)) {
        cout << "WaitNamedPipe failed, error " << GetLastError() << "\n";
        return 1;
    }

    HANDLE hPipe = CreateFileA(
        PIPE_NAME,
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        OPEN_EXISTING,
        0,
        nullptr
    );

    if (hPipe == INVALID_HANDLE_VALUE) {
        cout << "CreateFile for pipe failed, error " << GetLastError() << "\n";
        return 1;
    }

    bool running = true;

    while (running) {
        cout << "\n1 - read, 2 - modify, 0 - exit: ";
        int action;
        if (!(cin >> action)) {
            break;
        }

        if (action == 0) {
            Request req{};
            req.type = RequestType::EXIT_CLIENT;
            req.id = 0;
            write_exact(hPipe, &req, sizeof(req));

            Response resp{};
            if (read_exact(hPipe, &resp, sizeof(resp))) {
                cout << "Server acknowledged exit.\n";
            }
            running = false;
        } else if (action == 1) {
            int id;
            cout << "Enter employee ID: ";
            cin >> id;

            Request req{};
            req.type = RequestType::READ;
            req.id = id;
            write_exact(hPipe, &req, sizeof(req));

            Response resp{};
            if (!read_exact(hPipe, &resp, sizeof(resp))) {
                cout << "Error reading response from server\n";
                continue;
            }
            if (resp.status == ResponseStatus::NOT_FOUND) {
                cout << "Employee not found\n";
            } else if (resp.status != ResponseStatus::OK) {
                cout << "Server error\n";
            } else {
                cout << "Record:\n";
                cout << "ID: " << resp.emp.get_num()
                     << ", Name: " << resp.emp.get_name()
                     << ", Hours: " << resp.emp.get_hours() << "\n";

                cout << "Press any key and Enter to finish access to record: ";
                string dummy;
                cin >> dummy;

                Request fin{};
                fin.type = RequestType::FINISH_RECORD;
                fin.id = id;
                write_exact(hPipe, &fin, sizeof(fin));

                Response fin_resp{};
                read_exact(hPipe, &fin_resp, sizeof(fin_resp));
            }
        } else if (action == 2) {
            int id;
            cout << "Enter employee ID: ";
            cin >> id;

            Request req{};
            req.type = RequestType::MODIFY;
            req.id = id;
            write_exact(hPipe, &req, sizeof(req));

            Response resp{};
            if (!read_exact(hPipe, &resp, sizeof(resp))) {
                cout << "Error reading response from server\n";
                continue;
            }
            if (resp.status == ResponseStatus::NOT_FOUND) {
                cout << "Employee not found\n";
            } else if (resp.status != ResponseStatus::OK) {
                cout << "Server error\n";
            } else {
                cout << "Current record:\n";
                cout << "ID: " << resp.emp.get_num()
                     << ", Name: " << resp.emp.get_name()
                     << ", Hours: " << resp.emp.get_hours() << "\n";

                string new_name;
                double new_hours;
                cout << "Enter new name: ";
                cin >> new_name;
                cout << "Enter new hours: ";
                cin >> new_hours;

                employee modified = resp.emp;
                modified.set_name(new_name);
                modified.set_hours(new_hours);

                Request send_mod{};
                send_mod.type = RequestType::SEND_MODIFIED;
                send_mod.id = id;
                send_mod.emp = modified;
                write_exact(hPipe, &send_mod, sizeof(send_mod));

                Response upd_resp{};
                if (!read_exact(hPipe, &upd_resp, sizeof(upd_resp))) {
                    cout << "Error reading update response\n";
                } else if (upd_resp.status == ResponseStatus::OK) {
                    cout << "Record updated.\n";
                } else {
                    cout << "Failed to update record.\n";
                }

                cout << "Press any key and Enter to finish access to record: ";
                string dummy;
                cin >> dummy;

                Request fin{};
                fin.type = RequestType::FINISH_RECORD;
                fin.id = id;
                write_exact(hPipe, &fin, sizeof(fin));

                Response fin_resp{};
                read_exact(hPipe, &fin_resp, sizeof(fin_resp));
            }
        } else {
            cout << "Invalid action\n";
        }
    }

    CloseHandle(hPipe);
    cout << "Client finished\n";
    return 0;
}
