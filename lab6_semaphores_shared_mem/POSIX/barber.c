#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <zconf.h>
#include <semaphore.h>
#include "common.h"

int mem_id;
sem_t *sem_id;

void sig_handler(int _);
void clean_up();
void init_barbershop(int seats);
void invite_client();
void shave();
void fall_asleep();

int main(int argc, char** argv) {
    if (argc != 2) {
        EXIT_MSG("Usage: ./barber number_of_seats\n")
    }
    int seats = (int) strtol(argv[1], 0, 10);
    if (seats > MAX_CLIENTS) {
        EXIT_MSG("Too many seats specified, maximum %d\n", MAX_CLIENTS)
    }
    init_barbershop(seats);

    while (1) {
        sem_take(sem_id);
        switch (barbershop->status) {
            case WOKEN_UP:
                printf("%ld Barber: woken up\n", curr_time());
                invite_client();
                break;
            case START_SHAVING:
                shave();
                break;
            case FREE:
                if (!q_empty()) {
                    invite_client();
                } else {
                    fall_asleep();
                }
                break;
            default:
                break;
        }
        sem_return(sem_id);
    }
}

void fall_asleep() {
    barbershop->status = SLEEPING;
    printf("%ld Barber: falling asleep\n", curr_time());
}

void shave() {
    barbershop->status = END_SHAVING;
    printf("%ld Barber: started shaving client %d\n", curr_time(), barbershop->curr_client);
    printf("%ld Barber: ended shaving client %d\n", curr_time(), barbershop->curr_client);
}

void invite_client() {
    barbershop->status = INVITING;
    barbershop->curr_client = dequeue();
    printf("%ld Barber: inviting client %d\n", curr_time(), barbershop->curr_client);
}

void init_barbershop(int seats) {
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);
    atexit(clean_up);
    mem_id = shm_open(PROJECT_NAME, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
    if (mem_id == -1) {
        EXIT_MSG("Barber: cannot initialize shared memory segment\n")
    }

    if (ftruncate(mem_id, sizeof(*barbershop))) {
        EXIT_MSG("Barber: cannot set size of shared memory segment")
    }
    barbershop = mmap(NULL, sizeof(*barbershop), PROT_READ | PROT_WRITE, MAP_SHARED, mem_id, 0);
    if (barbershop == (void *) -1) {
        EXIT_MSG("Barber: cannot access shared memory\n")
    }

    sem_id = sem_open(PROJECT_NAME, O_WRONLY | O_CREAT, S_IRWXU | S_IRWXG, 1);
    if (sem_id == SEM_FAILED) {
        EXIT_MSG("Barber: could not create semaphore\n")
    }
    barbershop->total_clients = 0;
    barbershop->curr_client = 0;
    barbershop->status = SLEEPING;
    barbershop->waiting_room_size = seats;
}

void clean_up() {
    munmap(barbershop, sizeof(*barbershop));
    if (sem_id != 0) {
        sem_close(sem_id);
    }
    if (mem_id != 0) {
        shm_unlink(PROJECT_NAME);
    }
}

void sig_handler(int _) {
    printf("Closing barbershop\n");
    exit(0);
}
