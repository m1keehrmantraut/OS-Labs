#pragma once
#include <iostream>
#include <fstream>
#include <string>

#pragma warning(disable: 4996)

using namespace std;

struct employee {
    int num;
    char name[10];
    double hours;

    employee() : num(0), hours(0.0) {
        memset(name, 0, 10);
    }

    employee(int n, const string& nm, double h) : num(n), hours(h) {
        memset(name, 0, 10);
        strncpy(name, nm.c_str(), 9);
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

    void set_name(const string& nm) {
        memset(name, 0, 10);
        strncpy(name, nm.c_str(), 9);
    }

    void set_hours(double h) {
        hours = h;
    }

    friend ostream& operator<<(ostream& out, const employee& e) {
        out.write((char*)&e, sizeof(e));
        return out;
    }

    friend istream& operator>>(istream& in, employee& e) {
        in.read((char*)&e, sizeof(e));
        return in;
    }
};
