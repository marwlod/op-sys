#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <semaphore.h>

#define EXIT_MSG(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
int P, K, N, L, nk, search_type, full_info;
pthread_t *prod_threads;
pthread_t *cons_threads;
char **shared_resource;
int curr_prod, curr_cons;
char file_name[FILENAME_MAX];
FILE *file;
sem_t *sems;
bool done_producing;

void read_config(char *config_path);
void set_up();
void create_threads();
void wait_threads();
void clean_up();
void sig_handler(int _);
bool found_len(int len);
void *produce(void *_);
void *consume(void *_);

int main(int argc, char **argv) {
    if (argc != 2) {
        EXIT_MSG("Usage: ./main config_filename\n")
    }
    read_config(argv[1]);
    set_up();
    create_threads();
    wait_threads();
    clean_up();
    return 0;
}

// read config file and populate all necessary global variables
void read_config(char *config_path) {
    FILE *config_file = fopen(config_path, "r");
    if (config_file == NULL) {
        EXIT_MSG("Could not open file from path: %s\n", config_path)
    }
    fscanf(config_file, "%d %d %d %s %d %d %d %d", &P, &K, &N, file_name, &L, &search_type, &full_info, &nk);
    fclose(config_file);
}

// prepare all global resources like threads, conditions, mutexes, shared cyclic array
void set_up() {
    signal(SIGINT, sig_handler);
    if (nk > 0) {
        signal(SIGALRM, sig_handler);
        alarm(nk);
    }
    file = fopen(file_name, "r");
    if (file == NULL) {
        EXIT_MSG("File with filename %s could not be opened\n", file_name)
    }
    shared_resource = calloc(N, sizeof(char *));
    prod_threads = malloc(P * sizeof(pthread_t));
    cons_threads = malloc(K * sizeof(pthread_t));
    sems = malloc((N+3) * sizeof(sem_t));
    for (int n = 0; n < N+2; ++n) {
        sem_init(&sems[n], 0, 1);
    }
    sem_init(&sems[N+2], 0, (unsigned int) N);
    curr_prod = 0;
    curr_cons = 0;
}

// create new threads for producers and consumers
void create_threads() {
    for (int p = 0; p < P; ++p) {
        pthread_create(&prod_threads[p], NULL, produce, NULL);
    }
    for (int k = 0; k < K; ++k) {
        pthread_create(&cons_threads[k], NULL, consume, NULL);
    }
}

// wait for all threads to finish
void wait_threads() {
    for (int p = 0; p < P; ++p) {
        pthread_join(prod_threads[p], NULL);
    }
    done_producing = true;
    for (int k = 0; k < K; ++k) {
        pthread_join(cons_threads[k], NULL);
    }
}

// release all resources to the system
void clean_up() {
    for (int n = 0; n < N; ++n) {
        sem_destroy(&sems[n]);
    }
    free(sems);
    for (int n = 0; n < N; ++n) {
        if (shared_resource[n] != NULL) {
            free(shared_resource[n]);
        }
    }
    free(shared_resource);
    if (file != NULL) {
        fclose(file);
    }
}

// handle SIGINT and SIGALRM
void sig_handler(int _) {
    printf("Ending program\n");
    exit(0);
}

bool found_len(int len) {
    if ((search_type == -1 && len < L) || (search_type == 0 && len == L) || (search_type == 1 && len > L)) {
        return true;
    }
    return false;
}

// producer thread start routine
void *produce(void *_) {
    int curr_index;
    char line[LINE_MAX];
    while (fgets(line, LINE_MAX, file) != NULL) {
        if (full_info) {
            printf("Producer %ld: reading line from file\n", pthread_self());
        }
        sem_wait(&sems[N]);
        sem_wait(&sems[N+2]);

        curr_index = curr_prod;
        if (full_info) {
            printf("Producer %ld: obtaining shared resource access on index %d\n", pthread_self(), curr_index);
        }
        curr_prod = (curr_prod+1) % N;

        sem_wait(&sems[curr_index]);
        sem_post(&sems[N]);

        shared_resource[curr_index] = malloc((strlen(line)+1) * sizeof(char));
        strcpy(shared_resource[curr_index], line);
        if (full_info) {
            printf("Producer %ld: line copied to shared resource at index %d\n", pthread_self(), curr_index);
        }

        sem_post(&sems[curr_index]);
    }
    if (full_info) {
        printf("Producer %ld: finished producing\n", pthread_self());
    }
    return NULL;
}

// consumer thread start routine
void *consume(void *_) {
    int curr_index;
    char *line;
    while (1) {
        sem_wait(&sems[N+1]);
        while (shared_resource[curr_cons] == NULL) {
            sem_post(&sems[N+1]);
            if (done_producing) {
                if (full_info) {
                    printf("Consumer %ld: finished consuming\n", pthread_self());
                    return NULL;
                }
            }
            sem_wait(&sems[N+1]);
        }
        curr_index = curr_cons;
        if (full_info) {
            printf("Consumer %ld: obtaining shared resource access on index %d\n", pthread_self(), curr_index);
        }
        curr_cons = (curr_cons+1) % N;
        sem_wait(&sems[curr_index]);

        line = shared_resource[curr_index];
        shared_resource[curr_index] = NULL;
        if (full_info) {
            printf("Consumer %ld: reading from shared resource at index %d\n", pthread_self(), curr_index);
        }
        sem_post(&sems[N+2]);
        sem_post(&sems[N+1]);
        sem_post(&sems[curr_index]);

        if (line != NULL && found_len((int) strlen(line))) {
            printf("Consumer %ld: found line with length %lu %s %d\n", pthread_self(), strlen(line),
                    (search_type == 0 ? "equal" : (search_type == -1 ? "less than" : "greater than")), L);
        }
        free(line);
    }
}
