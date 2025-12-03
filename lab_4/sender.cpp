#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string>
#include <cstring>
#include "shared_memory.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: Sender <sender_id>" << endl;
        return -1;
    }

    int sender_id = stoi(argv[1]);

    int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return -1;
    }

    SharedMemoryData* shm_data = (SharedMemoryData*)mmap(
        nullptr, sizeof(SharedMemoryData),
        PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0
    );

    if (shm_data == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    sem_t* write_sem = sem_open(WRITE_SEM_NAME, 0);
    sem_t* read_sem = sem_open(READ_SEM_NAME, 0);
    sem_t* mutex_sem = sem_open(MUTEX_SEM_NAME, 0);
    sem_t* ready_sem = sem_open(READY_SEM_NAME, 0);

    if (!write_sem || !read_sem || !mutex_sem || !ready_sem) {
        perror("sem_open");
        return -1;
    }

    cout << "Sender " << sender_id << " started" << endl;
    
    sem_post(ready_sem);
    cout << "Sender " << sender_id << " ready" << endl;

    int action;
    bool running = true;

    while (running) {
        cout << "\nSender " << sender_id << " - 1 - write message, 0 - exit: ";
        cin >> action;

        if (action == 0) {
            running = false;
        } else if (action == 1) {
            cin.ignore();
            string text;
            cout << "Enter message (max 19 chars): ";
            getline(cin, text);

            if (text.length() >= 20) {
                cout << "Message too long" << endl;
                continue;
            }

            int result = sem_trywait(write_sem);
            if (result == 0) {
                sem_wait(mutex_sem);

                Message new_msg(text);
                shm_data->messages[shm_data->tail] = new_msg;
                shm_data->tail = (shm_data->tail + 1) % shm_data->max_records;
                shm_data->count++;

                sem_post(mutex_sem);
                sem_post(read_sem);

                cout << "Message sent: " << text << endl;
            } else {
                cout << "No space available, try again later" << endl;
            }
        } else {
            cout << "Unknown command" << endl;
        }
    }

    cout << "Sender " << sender_id << " finishing..." << endl;

    munmap(shm_data, sizeof(SharedMemoryData));
    close(shm_fd);

    sem_close(write_sem);
    sem_close(read_sem);
    sem_close(mutex_sem);
    sem_close(ready_sem);

    return 0;
}