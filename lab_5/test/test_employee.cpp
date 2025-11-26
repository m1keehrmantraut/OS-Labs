#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../doctest.h"
#include "../include/employee.h"
#include <sstream>

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
    CHECK(ss.str().length() == sizeof(employee));
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

    employee copy1, copy2;
    ss >> copy1 >> copy2;

    CHECK(copy1.get_num() == 1);
    CHECK(string(copy1.get_name()) == "Alice");
    CHECK(copy2.get_num() == 2);
    CHECK(string(copy2.get_name()) == "Bob");
}

TEST_CASE("employee struct size") {
    CHECK(sizeof(employee) >= sizeof(int) + sizeof(char[10]) + sizeof(double));
}


TEST_CASE("employee name buffer exactly 10 bytes") {
    CHECK(sizeof(employee::name) == 10);
}
