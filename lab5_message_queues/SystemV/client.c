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

// MAIN ////////////////////////////////////////////////////////////////////////

int main() {
    if(atexit(close_queue) == -1)
    EXIT_MSG("Registering client's atexit failed");
    if(signal(SIGINT, sigint_handler) == SIG_ERR)
    EXIT_MSG("Registering INT failed");

    char* path = getenv("HOME");
    if (path == NULL)
    EXIT_MSG("Getting $HOME failed");

    queue_desc = create_queue(path, PROJECT_ID);

    key_t privateKey = ftok(path, getpid());
    if (privateKey == -1)
    EXIT_MSG("Generation of private key failed");

    privateID = msgget(privateKey, IPC_CREAT | IPC_EXCL | 0666);
    if (privateID == -1)
    EXIT_MSG("Creation of private queue failed");

    register_client(privateKey);

    char cmd[20];
    Message msg;
    while(1) {
        msg.sender_pid = getpid();
        printf("client: enter your request: ");
        if (fgets(cmd, 20, stdin) == NULL){
            printf("client: error reading your command\n");
            continue;
        }
        int n = strlen(cmd);
        if (cmd[n-1] == '\n') cmd[n-1] = 0;


        if (strcmp(cmd, "mirror") == 0) {
            request_mirror(&msg);
        } else if (strcmp(cmd, "calc") == 0) {
            request_calc(&msg);
        } else if (strcmp(cmd, "time") == 0) {
            request_time(&msg);
        } else if (strcmp(cmd, "end") == 0) {
            request_end(&msg);
        } else if (strcmp(cmd, "quit") == 0) {
            exit(0);
        } else {
            printf("client: incorrect command\n");
        }
    }
}

void register_client(key_t privateKey) {
    Message msg;
    msg.mtype = LOGIN;
    msg.sender_pid = getpid();
    sprintf(msg.message_text, "%d", privateKey);

    if (msgsnd(queue_desc, &msg, MSG_SIZE, 0) == -1)
    EXIT_MSG("client: LOGIN request failed\n");
    if (msgrcv(privateID, &msg, MSG_SIZE, 0, 0) == -1)
    EXIT_MSG("client: catching LOGIN response failed\n");
    if (sscanf(msg.message_text, "%d", &sessionID) < 1)
    EXIT_MSG("client: scanning LOGIN response failed\n");
    if (sessionID < 0)
    EXIT_MSG("client: server cannot have more clients\n");

    printf("client: client registered. Session no: %d\n", sessionID);
}

// HANDLERS ////////////////////////////////////////////////////////////////////

void request_mirror(Message *msg) {
    msg->mtype = MIRROR;
    printf("Enter string of characters to Mirror: ");
    if (fgets(msg->message_text, MAX_CONT_SIZE, stdin) == 0) {
        printf("client: too many characters\n");
        return;
    }
    if (msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1)
    EXIT_MSG("client: MIRROR request failed");
    if (msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1)
    EXIT_MSG("client: catching MIRROR response failed");
    printf("%s", msg->message_text);
}

void request_calc(Message *msg) {
    msg->mtype = CALC;
    printf("Enter expression to calculate: ");
    if (fgets(msg->message_text, MAX_CONT_SIZE, stdin) == 0) {
        printf("client: too many characters\n");
        return;
    }
    if(msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1)
    EXIT_MSG("client: CALC request failed");
    if(msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1)
    EXIT_MSG("client: catching CALC response failed");
    printf("%s", msg->message_text);
}

void request_time(Message *msg) {
    msg->mtype = TIME;

    if(msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1)
    EXIT_MSG("client: TIME request failed");
    if(msgrcv(privateID, msg, MSG_SIZE, 0, 0) == -1)
    EXIT_MSG("client: catching TIME response failed");
    printf("%s\n", msg->message_text);
}

void request_end(Message *msg) {
    msg->mtype = END;

    if(msgsnd(queue_desc, msg, MSG_SIZE, 0) == -1)
    EXIT_MSG("client: END request failed");
}

// HELPERS /////////////////////////////////////////////////////////////////////

int create_queue(char *path, int ID) {
    int key = ftok(path, ID);
    if(key == -1) EXIT_MSG("Generation of key failed");

    int QueueID = msgget(key, 0);
    if (QueueID == -1) EXIT_MSG("Opening queue failed");

    return QueueID;
}

void close_queue() {
    if (privateID > -1) {
        if (msgctl(privateID, IPC_RMID, NULL) == -1){
            printf("There was some error deleting clients's queue\n");
        }
        else {
            printf("Client's queue deleted successfully\n");
        }
    }
}

void sigint_handler(int _) { exit(2); }