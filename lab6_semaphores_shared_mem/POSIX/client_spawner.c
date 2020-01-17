#include <sys/wait.h>
#include <errno.h>
#include <zconf.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "common.h"

enum client_status {
    INVITED,
    SHAVEN,
    WAITING,
} client_status;

int mem_id;
sem_t *sem_id;
void initialize_clients();
void arrive_at_barbershop(int shaves);
bool first_shave;

int main(int argc, char** argv) {
    if (argc != 3) {
        EXIT_MSG("Usage: ./client_spawner clients_num shaves_num\n")
    }
    int clients = (int) strtol(argv[1], 0, 10);
    int shaves = (int) strtol(argv[2], 0, 10);
    initialize_clients();

    for (int i = 0; i < clients; i++) {
        if (!fork()) {
            arrive_at_barbershop(shaves);
            exit(0);
        }
    }
    while (wait(0)) if (errno != ECHILD) break;
}

void arrive_at_barbershop(int shaves) {
    first_shave = true;
    client_status = WAITING;
    for (int curr_shave = 1; curr_shave <= shaves; curr_shave++) {
        sem_take(sem_id);
        if (client_status == WAITING && q_full()) {
            printf("%ld Client %d: leaving barbershop, no free chair in the waiting room\n", curr_time(), getpid());
            sem_return(sem_id);
            return;
        } else if (barbershop->status == SLEEPING) {
            barbershop->status = WOKEN_UP;
            enqueue(getpid());
            printf("%ld Client %d: waking up barber\n", curr_time(), getpid());
        } else if (first_shave) {
            enqueue(getpid());
            printf("%ld Client %d: starting to wait in queue\n", curr_time(), getpid());
        }
        sem_return(sem_id);

        while (client_status != INVITED) {
            sem_take(sem_id);
            if (barbershop->status == INVITING && getpid() == barbershop->curr_client) {
                client_status = INVITED;
                barbershop->status = START_SHAVING;
                printf("%ld Client %d: sitting at barber's chair\n", curr_time(), getpid());
            }
            sem_return(sem_id);
        }

        while (client_status != SHAVEN) {
            sem_take(sem_id);
            if (barbershop->status == END_SHAVING && getpid() == barbershop->curr_client) {
                client_status = SHAVEN;
                barbershop->status = FREE;
                if (curr_shave < shaves) {
                    if (!q_full()) {
                        enqueue(getpid());
                        printf("%ld Client %d: ended shave %d, starting to wait in queue\n", curr_time(), getpid(), curr_shave);
                    } else if (q_full()) {
                        printf("%ld Client %d: leaving barbershop, no free chair in the waiting room\n", curr_time(), getpid());
                        sem_return(sem_id);
                        return;
                    }
                }
            }
            sem_return(sem_id);
        }
        first_shave = false;
    }
    printf("%ld Client %d: ended shave %d, leaving barbershop\n", curr_time(), getpid(), shaves);
}

void initialize_clients() {
    mem_id = shm_open(PROJECT_NAME, O_RDWR, S_IRWXU | S_IRWXG);
    if (mem_id == -1) {
        EXIT_MSG("Client: error while getting shared memory\n")
    }
    barbershop = mmap(NULL, sizeof(*barbershop), PROT_READ | PROT_WRITE, MAP_SHARED, mem_id, 0);
    if (barbershop == (void *) -1) {
        EXIT_MSG("Client: error while accessing shared memory\n")
    }
    sem_id = sem_open(PROJECT_NAME, O_WRONLY);
    if (sem_id == SEM_FAILED) {
        EXIT_MSG("Client: error while getting semaphore\n")
    }
}