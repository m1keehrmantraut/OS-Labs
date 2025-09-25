#include <iostream>
#include <fstream>
#include <sstream>
#include "employee.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage: Creator <filename>" << endl;
        return 1;
    }

    ofstream out(argv[1], ios::binary);
    if (!out) {
        cout << "Error: cannot open file " << argv[1] << endl;
        return 1;
    }

    int n;
    cout << "Enter number of employees: ";
    cin >> n;

    employee emp;
    for (int i = 0; i < n; i++) {
        cout << "Enter employee number: ";
        cin >> emp.num;
        cout << "Enter employee name: ";
        cin >> emp.name;
        cout << "Enter hours worked: ";
        cin >> emp.hours;

        out.write((char*)&emp, sizeof(employee));
    }

    out.close();
    return 0;
}
