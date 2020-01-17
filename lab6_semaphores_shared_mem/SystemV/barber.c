#include <sys/sem.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "common.h"

int mem_id;
int sem_id;

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
    key_t key = ftok(PROJECT_PATH, PROJECT_ID);
    if (key == -1) {
        EXIT_MSG("Barber: getting key for shared memory and semaphores failed\n")
    }
    mem_id = shmget(key, sizeof(struct barbershop), IPC_CREAT | S_IRWXU);
    if (mem_id == -1) {
        EXIT_MSG("Barber: cannot initialize shared memory segment\n")
    }

    barbershop = shmat(mem_id, NULL, 0);
    if (barbershop == (void *) -1) {
        EXIT_MSG("Barber: cannot access shared memory\n")
    }

    sem_id = semget(key, 1, IPC_CREAT | S_IRWXU);
    if (sem_id == -1) {
        EXIT_MSG("Barber: could not create semaphore\n")
    }
    sem_return(sem_id);
    barbershop->total_clients = 0;
    barbershop->curr_client = 0;
    barbershop->status = SLEEPING;
    barbershop->waiting_room_size = seats;
}

void clean_up() {
    shmdt(barbershop);
    if (sem_id != 0) {
        semctl(sem_id, 0,IPC_RMID);
    }
    if (mem_id != 0) {
        shmctl(mem_id, IPC_RMID,NULL);
    }
}

void sig_handler(int _) {
    printf("Closing barbershop\n");
    exit(0);
}
