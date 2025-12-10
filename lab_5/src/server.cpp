#include <windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>

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

bool find_employee(fstream& file, int id, employee& out_emp, streampos& pos) {
    file.clear();
    file.seekg(0, ios::beg);
    employee e;
    while (true) {
        streampos cur = file.tellg();
        if (!file.read(reinterpret_cast<char*>(&e), sizeof(e))) {
            return false;
        }
        if (e.num == id) {
            out_emp = e;
            pos = cur;
            return true;
        }
    }
}

void print_file(const string& filename) {
    fstream file(filename, ios::binary | ios::in);
    if (!file.is_open()) {
        cout << "Cannot open file for printing\n";
        return;
    }
    cout << "\nFile contents:\n";
    employee e;
    while (file.read(reinterpret_cast<char*>(&e), sizeof(e))) {
        cout << "ID: " << e.get_num()
             << ", Name: " << e.get_name()
             << ", Hours: " << e.get_hours() << "\n";
    }
    file.close();
}

struct LockInfo {
    int readers = 0;
    bool writer = false;
};

fstream g_file;
mutex g_file_mutex;
mutex g_lock_mutex;
map<int, LockInfo> g_locks;

void acquire_read_lock(int id) {
    while (true) {
        {
            lock_guard<mutex> lg(g_lock_mutex);
            LockInfo& info = g_locks[id];
            if (!info.writer) {
                info.readers++;
                return;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void release_read_lock(int id) {
    lock_guard<mutex> lg(g_lock_mutex);
    LockInfo& info = g_locks[id];
    if (info.readers > 0) {
        info.readers--;
    }
}

void acquire_write_lock(int id) {
    while (true) {
        {
            lock_guard<mutex> lg(g_lock_mutex);
            LockInfo& info = g_locks[id];
            if (!info.writer && info.readers == 0) {
                info.writer = true;
                return;
            }
        }
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void release_write_lock(int id) {
    lock_guard<mutex> lg(g_lock_mutex);
    LockInfo& info = g_locks[id];
    info.writer = false;
}

void serve_client(HANDLE hPipe) {
    bool running = true;
    bool has_lock = false;
    bool is_write_lock = false;
    int locked_id = 0;

    while (running) {
        Request req{};
        if (!read_exact(hPipe, &req, sizeof(req))) {
            break;
        }

        Response resp{};
        switch (req.type) {
        case RequestType::READ: {
            if (has_lock) {
                if (is_write_lock) {
                    release_write_lock(locked_id);
                } else {
                    release_read_lock(locked_id);
                }
                has_lock = false;
            }

            acquire_read_lock(req.id);
            has_lock = true;
            is_write_lock = false;
            locked_id = req.id;

            employee e;
            streampos pos;
            {
                lock_guard<mutex> fg(g_file_mutex);
                if (find_employee(g_file, req.id, e, pos)) {
                    resp.status = ResponseStatus::OK;
                    resp.emp = e;
                } else {
                    resp.status = ResponseStatus::NOT_FOUND;
                }
            }
            write_exact(hPipe, &resp, sizeof(resp));
            break;
        }
        case RequestType::MODIFY: {
            if (has_lock) {
                if (is_write_lock) {
                    release_write_lock(locked_id);
                } else {
                    release_read_lock(locked_id);
                }
                has_lock = false;
            }

            acquire_write_lock(req.id);
            has_lock = true;
            is_write_lock = true;
            locked_id = req.id;

            employee e;
            streampos pos;
            {
                lock_guard<mutex> fg(g_file_mutex);
                if (find_employee(g_file, req.id, e, pos)) {
                    resp.status = ResponseStatus::OK;
                    resp.emp = e;
                } else {
                    resp.status = ResponseStatus::NOT_FOUND;
                }
            }
            write_exact(hPipe, &resp, sizeof(resp));
            break;
        }
        case RequestType::SEND_MODIFIED: {
            if (!has_lock || !is_write_lock || locked_id != req.id) {
                resp.status = ResponseStatus::SERVER_ERROR;
                write_exact(hPipe, &resp, sizeof(resp));
                break;
            }

            employee e;
            streampos pos;
            bool ok;
            {
                lock_guard<mutex> fg(g_file_mutex);
                ok = find_employee(g_file, req.id, e, pos);
                if (ok) {
                    g_file.clear();
                    g_file.seekp(pos);
                    g_file.write(reinterpret_cast<const char*>(&req.emp), sizeof(req.emp));
                    g_file.flush();
                }
            }
            if (ok) {
                resp.status = ResponseStatus::OK;
            } else {
                resp.status = ResponseStatus::NOT_FOUND;
            }
            write_exact(hPipe, &resp, sizeof(resp));
            break;
        }
        case RequestType::FINISH_RECORD: {
            if (has_lock && locked_id == req.id) {
                if (is_write_lock) {
                    release_write_lock(locked_id);
                } else {
                    release_read_lock(locked_id);
                }
                has_lock = false;
            }
            resp.status = ResponseStatus::OK;
            write_exact(hPipe, &resp, sizeof(resp));
            break;
        }
        case RequestType::EXIT_CLIENT: {
            resp.status = ResponseStatus::OK;
            write_exact(hPipe, &resp, sizeof(resp));
            running = false;
            break;
        }
        default: {
            resp.status = ResponseStatus::SERVER_ERROR;
            write_exact(hPipe, &resp, sizeof(resp));
            break;
        }
        }
    }

    if (has_lock) {
        if (is_write_lock) {
            release_write_lock(locked_id);
        } else {
            release_read_lock(locked_id);
        }
    }

    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
}

int main() {
    string filename;
    cout << "Enter file name: ";
    cin >> filename;

    int n;
    cout << "Enter number of employees: ";
    cin >> n;

    vector<employee> employees;
    employees.reserve(n);
    for (int i = 0; i < n; ++i) {
        int id;
        string name;
        double hours;
        cout << "Employee " << (i + 1) << " ID: ";
        cin >> id;
        cout << "Name: ";
        cin >> name;
        cout << "Hours: ";
        cin >> hours;
        employees.emplace_back(id, name, hours);
    }

    {
        fstream file(filename, ios::binary | ios::out | ios::trunc);
        if (!file.is_open()) {
            cout << "Error creating file\n";
            return 1;
        }
        for (const auto& e : employees) {
            file.write(reinterpret_cast<const char*>(&e), sizeof(e));
        }
    }

    print_file(filename);

    g_file.open(filename, ios::binary | ios::in | ios::out);
    if (!g_file.is_open()) {
        cout << "Error opening file\n";
        return 1;
    }

    cout << "\nServer ready. Waiting for clients...\n";

    vector<thread> workers;
    bool accepting = true;

    while (accepting) {
        HANDLE hPipe = CreateNamedPipeA(
            PIPE_NAME,
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
            PIPE_UNLIMITED_INSTANCES,
            4096,
            4096,
            0,
            nullptr
        );

        if (hPipe == INVALID_HANDLE_VALUE) {
            cout << "CreateNamedPipe failed, error " << GetLastError() << "\n";
            break;
        }

        BOOL connected = ConnectNamedPipe(hPipe, nullptr) ?
                         TRUE :
                         (GetLastError() == ERROR_PIPE_CONNECTED);
        if (!connected) {
            CloseHandle(hPipe);
            continue;
        }

        workers.emplace_back(serve_client, hPipe);

        cout << "Client connected. Continue accepting? (y/n): ";
        char c;
        cin >> c;
        if (c == 'n' || c == 'N') {
            accepting = false;
        }
    }

    for (auto& t : workers) {
        if (t.joinable()) t.join();
    }

    g_file.close();

    cout << "\nFinal file contents:\n";
    print_file(filename);

    cout << "\nServer finished\n";
    return 0;
}
