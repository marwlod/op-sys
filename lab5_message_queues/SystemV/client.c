#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "common.h"
#include "client.h"

int main() {
    if (atexit(close_queue) == -1) {
        EXIT_MSG("Client: registering 'atexit' failed\n");
    }
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        EXIT_MSG("Client: registering SIGINT handler failed\n");
    }
    char* path = getenv("HOME");
    if (path == NULL) {
        EXIT_MSG("Client: getting HOME env variable failed\n");
    }
    queue_desc = create_priv_queue(path, PROJECT_ID);

    key_t key = ftok(path, getpid());
    if (key == -1) {
        EXIT_MSG("Client: generation of private key failed\n");
    }

    priv_qid = msgget(key, IPC_CREAT | 0666);
    if (priv_qid == -1) {
        EXIT_MSG("Client: creation of private queue failed\n");
    }
    register_client(key);

    char request[50];
    Message msg;
    while (1) {
        msg.sender_pid = getpid();
        printf("Client: waiting for request: \n");
        if (fgets(request, 50, stdin) == NULL){
            printf("Client: request is invalid, try again\n");
            continue;
        }
        int n = strlen(request);
        if (request[n-1] == '\n') request[n-1] = 0;

        if (strcmp(request, "mirror") == 0) {
            request_mirror(&msg);
        } else if (strcmp(request, "time") == 0) {
            request_time(&msg);
        } else if (strcmp(request, "end") == 0) {
            request_end(&msg);
        } else if (strcmp(request, "quit") == 0) {
            exit(0);
        } else {
            printf("Client: unknown request type %s\n", request);
        }
    }
}

void register_client(key_t key) {
    Message msg;
    msg.type = REGISTER;
    msg.sender_pid = getpid();
    sprintf(msg.content, "%d", key);

    if (msgsnd(queue_desc, &msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("Client: REGISTER request sending failed\n");
    }
    if (msgrcv(priv_qid, &msg, MSG_SIZE, 0, 0) == -1) {
        EXIT_MSG("Client: getting REGISTER response failed\n");
    }
    if (sscanf(msg.content, "%d", &session_num) < 1) {
        EXIT_MSG("Client: REGISTER response is invalid\n");
    }
    if (session_num < 0) {
        EXIT_MSG("Client: server is unable to register any more clients\n");
    }

    printf("Client: client registration succeeded, session: %d\n", session_num);
}

void request_mirror(Message *msg) {
    msg->type = MIRROR;
    printf("Enter a string to mirror: \n");
    if (fgets(msg->content, MAX_CONT_SIZE, stdin) == 0) {
        printf("Client: maximum number of characters exceeded: %d\n", MAX_CONT_SIZE);
        return;
    }
    if (msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("Client: MIRROR request sending failed\n");
    }
    if (msgrcv(priv_qid, msg, MSG_SIZE, 0, 0) == -1) {
        EXIT_MSG("Client: getting MIRROR response failed\n");
    }
    printf("%s", msg->content);
}

void request_time(Message *msg) {
    msg->type = TIME;
    if (msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("Client: TIME request sending failed\n");
    }
    if (msgrcv(priv_qid, msg, MSG_SIZE, 0, 0) == -1) {
        EXIT_MSG("Client: getting TIME response failed\n");
    }
    printf("%s\n", msg->content);
}

void request_end(Message *msg) {
    msg->type = END;
    if (msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("Client: END request sending failed\n");
    }
}

int create_priv_queue(char *path, int id) {
    int key = ftok(path, id);
    if (key == -1) {
        EXIT_MSG("Client: generation of key for private queue failed\n");
    }
    int q_id = msgget(key, 0);
    if (q_id == -1) {
        EXIT_MSG("Client: opening private queue failed\n");
    }
    return q_id;
}

void close_queue() {
    if (priv_qid > -1) {
        if (msgctl(priv_qid, IPC_RMID, NULL) == -1) {
            printf("Client: deletion of client's queue failed\n");
        } else {
            printf("Client: queue deleted successfully\n");
        }
    }
}

// handle SIGINT
void sigint_handler(int _) {
    exit(2);
}