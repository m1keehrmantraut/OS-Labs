#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#pragma warning(disable: 4996)

struct employee {
    int num;
    char name[10];
    double hours;

    employee() : num(0), hours(0.0) {
        std::memset(name, 0, sizeof(name));
    }

    employee(int n, const std::string& nm, double h) : num(n), hours(h) {
        std::memset(name, 0, sizeof(name));
        std::strncpy(name, nm.c_str(), sizeof(name) - 1);
    }

    const char* get_name() const {
        return name;
    }

    int get_num() const {
        return num;
    }

    double get_hours() const {
        return hours;
    }

    void set_name(const std::string& nm) {
        std::memset(name, 0, sizeof(name));
        std::strncpy(name, nm.c_str(), sizeof(name) - 1);
    }

    void set_hours(double h) {
        hours = h;
    }
};

inline std::ostream& operator<<(std::ostream& out, const employee& e) {
    out.write(reinterpret_cast<const char*>(&e), sizeof(e));
    return out;
}

inline std::istream& operator>>(std::istream& in, employee& e) {
    in.read(reinterpret_cast<char*>(&e), sizeof(e));
    return in;
}
