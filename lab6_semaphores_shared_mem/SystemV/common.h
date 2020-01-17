#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <stdbool.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <stdio.h>

#define EXIT_MSG(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
#define PROJECT_ID 5
#define MAX_CLIENTS 32
#define PROJECT_PATH getenv("HOME")

enum barber_status {
    SLEEPING,
    WOKEN_UP,
    INVITING,
    START_SHAVING,
    END_SHAVING,
    FREE
};

struct barbershop {
    enum barber_status status;
    int total_clients;
    int waiting_room_size;
    pid_t curr_client;
    pid_t q[MAX_CLIENTS];
} *barbershop;

long curr_time() {
    struct timespec timespec;
    clock_gettime(CLOCK_MONOTONIC, &timespec);
    return timespec.tv_nsec / 1000;
}

bool q_empty() {
    return barbershop->total_clients == 0;
}

bool q_full() {
    return barbershop->total_clients >= barbershop->waiting_room_size;
}

void enqueue(pid_t client) {
    if (!q_full()) {
        barbershop->q[barbershop->total_clients] = client;
        barbershop->total_clients++;
    }
}

pid_t dequeue() {
    if (!q_empty()) {
        pid_t client = barbershop->q[0];
        for (int i = 0; i < barbershop->total_clients-1; i++) {
            barbershop->q[i] = barbershop->q[i+1];
        }

        barbershop->q[barbershop->total_clients-1] = 0;
        barbershop->total_clients--;
        return client;
    }
    return -1;
}

void sem_take(int sem_id) {
    struct sembuf sembuf;
    sembuf.sem_op = -1;
    sembuf.sem_flg = 0;
    sembuf.sem_num = 0;
    if (semop(sem_id, &sembuf, 1)) {
        EXIT_MSG("Could not take semaphore\n")
    }
}

void sem_return(int sem_id) {
    struct sembuf sembuf;
    sembuf.sem_op = 1;
    sembuf.sem_flg = 0;
    sembuf.sem_num = 0;
    if (semop(sem_id, &sembuf, 1)) {
        EXIT_MSG("Could not return semaphore\n")
    }
}

#endif