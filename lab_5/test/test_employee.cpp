#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "../include/employee.h"
#include <sstream>
#include <cstring>
#include <string>

using namespace std;

TEST_CASE("employee default constructor") {
    employee e;
    CHECK(e.get_num() == 0);
    CHECK(e.get_hours() == 0.0);
    CHECK(string(e.get_name()) == "");
}

TEST_CASE("employee parameterized constructor") {
    employee e(1, "John", 40.5);
    CHECK(e.get_num() == 1);
    CHECK(string(e.get_name()) == "John");
    CHECK(e.get_hours() == 40.5);
}

TEST_CASE("employee name truncation") {
    employee e(2, "VeryLongNameThatExceedsLimit", 35.0);
    CHECK(strlen(e.get_name()) == 9);
}

TEST_CASE("employee set_name") {
    employee e;
    e.set_name("Alice");
    CHECK(string(e.get_name()) == "Alice");
}

TEST_CASE("employee set_hours") {
    employee e;
    e.set_hours(45.5);
    CHECK(e.get_hours() == 45.5);
}

TEST_CASE("employee serialization") {
    employee e(5, "Bob", 38.0);
    stringstream ss;
    ss << e;
    CHECK(ss.str().size() == sizeof(employee));
}

TEST_CASE("employee deserialization") {
    employee orig(3, "Carol", 42.0);
    stringstream ss;
    ss << orig;
    ss.seekg(0);

    employee copy;
    ss >> copy;

    CHECK(copy.get_num() == 3);
    CHECK(string(copy.get_name()) == "Carol");
    CHECK(copy.get_hours() == 42.0);
}

TEST_CASE("employee roundtrip multiple") {
    employee e1(1, "Alice", 40.0);
    employee e2(2, "Bob", 35.0);

    stringstream ss;
    ss << e1 << e2;
    ss.seekg(0);

    employee c1, c2;
    ss >> c1 >> c2;

    CHECK(c1.get_num() == 1);
    CHECK(string(c1.get_name()) == "Alice");
    CHECK(c2.get_num() == 2);
    CHECK(string(c2.get_name()) == "Bob");
}

TEST_CASE("employee struct size") {
    CHECK(sizeof(employee) >= sizeof(int) + sizeof(char[10]) + sizeof(double));
}

TEST_CASE("employee name buffer exactly 10 bytes") {
    CHECK(sizeof(((employee*)0)->name) == 10);
}
