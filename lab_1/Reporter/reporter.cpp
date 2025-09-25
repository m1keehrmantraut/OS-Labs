#include <iostream>
#include <fstream>
#include <sstream>
#include "..//Creator/employee.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Usage: Reporter <input file> <output file>" << endl;
        return 1;
    }

    ifstream in(argv[1], ios::binary);
    if (!in) {
        cout << "Error: cannot open file " << argv[1] << endl;
        return 1;
    }

    ofstream out(argv[2]);
    if (!out) {
        cout << "Error: cannot open file " << argv[2] << endl;
        return 1;
    }

    double payPerHour;
    cout << "Enter hourly rate: ";
    cin >> payPerHour;

    employee emp;
    out << "Report for file: " << argv[1] << endl << endl;

    while (in.read((char*)&emp, sizeof(employee))) {
        double salary = emp.hours * payPerHour;

        out << "Employee number: " << emp.num << endl;
        out << "Name: " << emp.name << endl;
        out << "Hours worked: " << emp.hours << endl;
        out << "Salary: " << salary << endl;
        out << "--------------------------" << endl;
    }

    in.close();
    out.close();
    return 0;
}
