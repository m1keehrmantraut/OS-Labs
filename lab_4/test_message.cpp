#define CATCH_CONFIG_MAIN
#include "Receiver/catch.hpp"
#include "Receiver/message.h"
#include <sstream>

TEST_CASE("message default is empty") {
    message m;
    REQUIRE(m.is_empty() == true);
    REQUIRE(std::string(m.get_text()) == "");
}

TEST_CASE("message constructor with text") {
    message m("hello");
    REQUIRE(m.is_empty() == false);
    REQUIRE(std::string(m.get_text()) == "hello");
}

TEST_CASE("message truncates long text") {
    std::string longstr(25, 'a');
    message m(longstr);
    REQUIRE(m.is_empty() == false);
    REQUIRE(strlen(m.get_text()) == 19);
}

TEST_CASE("message serializes to stream") {
    message m("test");
    std::stringstream ss;
    ss << m;
    REQUIRE(ss.str().length() == sizeof(message));
}

TEST_CASE("message deserializes from stream") {
    message orig("world");
    std::stringstream ss;
    ss << orig;
    ss.seekg(0);

    message copy;
    ss >> copy;

    REQUIRE(copy.is_empty() == false);
    REQUIRE(std::string(copy.get_text()) == "world");
}

TEST_CASE("empty message roundtrip") {
    message empty;
    std::stringstream ss;
    ss << empty;
    ss.seekg(0);

    message result;
    ss >> result;

    REQUIRE(result.is_empty() == true);
}

TEST_CASE("message text buffer exactly 20 bytes") {
    REQUIRE(sizeof(message::text) == 20);
}

TEST_CASE("message struct size") {
    REQUIRE(sizeof(message) == sizeof(char[20]) + sizeof(bool));
}

TEST_CASE("multiple messages serialize independently") {
    message m1("first");
    message m2("second");
    std::stringstream ss;
    ss << m1 << m2;
    ss.seekg(0);

    message copy1, copy2;
    ss >> copy1 >> copy2;

    REQUIRE(std::string(copy1.get_text()) == "first");
    REQUIRE(std::string(copy2.get_text()) == "second");
}
