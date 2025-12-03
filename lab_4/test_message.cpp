#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "shared_memory.h"
#include <cstring>

TEST_CASE("Message default is empty") {
    Message m;
    REQUIRE(m.is_empty() == true);
    REQUIRE(std::string(m.get_text()) == "");
}

TEST_CASE("Message constructor with text") {
    Message m("hello");
    REQUIRE(m.is_empty() == false);
    REQUIRE(std::string(m.get_text()) == "hello");
}

TEST_CASE("Message truncates long text") {
    std::string longstr(25, 'a');
    Message m(longstr);
    REQUIRE(m.is_empty() == false);
    REQUIRE(std::strlen(m.get_text()) == 19);
}

TEST_CASE("Message buffer exactly 20 bytes") {
    REQUIRE(sizeof(Message::text) == 20);
}

TEST_CASE("Multiple messages in array") {
    Message msgs[5];
    msgs[0] = Message("first");
    msgs[1] = Message("second");
    msgs[2] = Message("third");

    REQUIRE(std::string(msgs[0].get_text()) == "first");
    REQUIRE(std::string(msgs[1].get_text()) == "second");
    REQUIRE(std::string(msgs[2].get_text()) == "third");
    REQUIRE(msgs[3].is_empty() == true);
    REQUIRE(msgs[4].is_empty() == true);
}

TEST_CASE("SharedMemoryData initialization") {
    SharedMemoryData data;
    REQUIRE(data.head == 0);
    REQUIRE(data.tail == 0);
    REQUIRE(data.count == 0);
    REQUIRE(data.max_records == MAX_RECORDS);

    for (int i = 0; i < MAX_RECORDS; ++i) {
        REQUIRE(data.messages[i].is_empty() == true);
    }
}

TEST_CASE("Ring buffer FIFO behavior") {
    SharedMemoryData data;

    for (int i = 0; i < 3; ++i) {
        data.messages[data.tail] = Message("msg" + std::to_string(i));
        data.tail = (data.tail + 1) % data.max_records;
        data.count++;
    }

    REQUIRE(data.count == 3);
    REQUIRE(data.head == 0);
    REQUIRE(data.tail == 3);

    REQUIRE(std::string(data.messages[data.head].get_text()) == "msg0");
    data.head = (data.head + 1) % data.max_records;
    data.count--;

    REQUIRE(std::string(data.messages[data.head].get_text()) == "msg1");
    data.head = (data.head + 1) % data.max_records;
    data.count--;

    REQUIRE(std::string(data.messages[data.head].get_text()) == "msg2");
    data.head = (data.head + 1) % data.max_records;
    data.count--;

    REQUIRE(data.count == 0);
}

TEST_CASE("Ring buffer wrap around") {
    SharedMemoryData data;
    data.max_records = 10;

    data.tail = 8;
    data.count = 2;

    data.messages[data.tail] = Message("wrap");
    data.tail = (data.tail + 1) % data.max_records;
    data.count++;

    REQUIRE(data.tail == 9);

    data.messages[data.tail] = Message("wrap2");
    data.tail = (data.tail + 1) % data.max_records;
    data.count++;

    REQUIRE(data.tail == 0);
    REQUIRE(data.count == 4);
}