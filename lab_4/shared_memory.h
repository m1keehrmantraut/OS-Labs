#pragma once
#include <cstring>
#include <iostream>

struct Message {
    char text[20];
    bool empty;

    Message() : empty(true) {
        std::memset(text, 0, sizeof(text));
    }

    Message(const std::string& s) : empty(false) {
        std::memset(text, 0, sizeof(text));
        if (s.length() >= 20) {
            std::strncpy(text, s.c_str(), 19);
        } else {
            std::strcpy(text, s.c_str());
        }
    }

    const char* get_text() const {
        return text;
    }

    bool is_empty() const {
        return empty;
    }
};

constexpr int MAX_RECORDS = 256;
constexpr int MESSAGE_SIZE = sizeof(Message);

struct SharedMemoryData {
    int head;
    int tail;
    int count;
    int max_records;
    Message messages[MAX_RECORDS];

    SharedMemoryData() : head(0), tail(0), count(0), max_records(MAX_RECORDS) {
        for (int i = 0; i < MAX_RECORDS; ++i) {
            messages[i] = Message();
        }
    }
};

const char* SHM_NAME = "/lab4_shm";
const char* WRITE_SEM_NAME = "/lab4_write_sem";
const char* READ_SEM_NAME = "/lab4_read_sem";
const char* MUTEX_SEM_NAME = "/lab4_mutex_sem";
const char* READY_SEM_NAME = "/lab4_ready_sem";