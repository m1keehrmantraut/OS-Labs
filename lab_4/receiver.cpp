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

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return -1;
    }

    ftruncate(shm_fd, sizeof(SharedMemoryData));

    SharedMemoryData* shm_data = (SharedMemoryData*)mmap(
        nullptr, sizeof(SharedMemoryData),
        PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0
    );

    if (shm_data == MAP_FAILED) {
        perror("mmap");
        shm_unlink(SHM_NAME);
        return -1;
    }

    new (shm_data) SharedMemoryData();

    sem_unlink(WRITE_SEM_NAME);
    sem_unlink(READ_SEM_NAME);
    sem_unlink(MUTEX_SEM_NAME);
    sem_unlink(READY_SEM_NAME);

    sem_t* write_sem = sem_open(WRITE_SEM_NAME, O_CREAT, 0666, MAX_RECORDS);
    sem_t* read_sem = sem_open(READ_SEM_NAME, O_CREAT, 0666, 0);
    sem_t* mutex_sem = sem_open(MUTEX_SEM_NAME, O_CREAT, 0666, 1);
    sem_t* ready_sem = sem_open(READY_SEM_NAME, O_CREAT, 0666, 0);

    if (!write_sem || !read_sem || !mutex_sem || !ready_sem) {
        perror("sem_open");
        return -1;
    }

    cout << "Waiting for Senders to be ready..." << endl;
    cout << "Enter number of Senders: ";
    int num_senders;
    cin >> num_senders;

    for (int i = 0; i < num_senders; ++i) {
        sem_wait(ready_sem);
        cout << "Sender " << i << " ready" << endl;
    }
    cout << "All Senders ready!" << endl;

    int action;
    bool running = true;

    while (running) {
        cout << "\n1 - read message, 0 - exit: ";
        cin >> action;

        if (action == 0) {
            running = false;
        } else if (action == 1) {
            int result = sem_trywait(read_sem);
            if (result == 0) {
                sem_wait(mutex_sem);

                if (shm_data->count > 0) {
                    Message& msg = shm_data->messages[shm_data->head];
                    cout << "Message: " << msg.get_text() << endl;

                    msg = Message();
                    shm_data->head = (shm_data->head + 1) % shm_data->max_records;
                    shm_data->count--;
                }

                sem_post(mutex_sem);
                sem_post(write_sem);
            } else {
                cout << "No messages available" << endl;
            }
        } else {
            cout << "Unknown command" << endl;
        }
    }

    cout << "Receiver finishing..." << endl;

    munmap(shm_data, sizeof(SharedMemoryData));
    close(shm_fd);
    shm_unlink(SHM_NAME);

    sem_close(write_sem);
    sem_close(read_sem);
    sem_close(mutex_sem);
    sem_close(ready_sem);

    sem_unlink(WRITE_SEM_NAME);
    sem_unlink(READ_SEM_NAME);    sem_unlink(MUTEX_SEM_NAME);
    sem_unlink(READY_SEM_NAME);

    return 0;
}