#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Windows.h>
#include "..//Creator/employee.h"

using namespace std;

bool runProcess(const string& app, const string& args) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    string cmdLine = app + " " + args;

    if (!CreateProcess(NULL, (LPSTR)cmdLine.c_str(),
            NULL, NULL,
            FALSE,CREATE_NEW_CONSOLE,
            NULL, NULL, &si, &pi)) {
        cerr << "Error: cannot start " << app << endl;
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return true;
}

void printBinaryFile(const string& fileName) {
    ifstream in(fileName.c_str(), ios::binary);
    if (!in) {
        cerr << "Error: cannot open " << fileName << endl;
        return;
    }
    cout << "\nContents of binary file \"" << fileName << "\":\n";
    cout << "-------------------------------------------\n";

    employee e;
    while (in.read(reinterpret_cast<char*>(&e), sizeof(employee))) {
        cout << e.num << " " << e.name << " " << e.hours << "\n";
    }

    in.close();
    cout << "-------------------------------------------\n";
}

void printReport(const string& fileName) {
    ifstream in(fileName.c_str());
    if (!in) {
        cerr << "Error: cannot open report " << fileName << endl;
        return;
    }
    cout << "\nReport \"" << fileName << "\":\n";
    cout << "-------------------------------------------\n";

    string line;
    while (getline(in, line)) {
        cout << line << "\n";
    }

    in.close();
    cout << "-------------------------------------------\n";
}

int main() {
    string binFile, repFile;
    int n;
    double rate;

    cout << "Enter binary file name: ";
    cin >> binFile;
    cout << "Enter number of employees: ";
    cin >> n;

    stringstream ss1;
    ss1 << n;
    string creatorArgs = binFile + " " + ss1.str();
    if (!runProcess("Creator.exe", creatorArgs)) return 1;

    printBinaryFile(binFile);

    cout << "\nEnter report file name: ";
    cin >> repFile;
    cout << "Enter salary per hour: ";
    cin >> rate;

    stringstream ss2;
    ss2 << rate;
    string reporterArgs = binFile + " " + repFile + " " + ss2.str();
    if (!runProcess("Reporter.exe", reporterArgs)) return 1;

    printReport(repFile);

    cout << "\nDone.\n";
    system("pause");
    return 0;
}
