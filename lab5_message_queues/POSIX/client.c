#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
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
    sprintf(client_path, "/%d", getpid());

    pub_qid = mq_open(server_path, O_WRONLY);
    if (pub_qid == -1) EXIT_MSG("Client: public queue opening failed\n");

    struct mq_attr posixAttr;
    posixAttr.mq_maxmsg = MAX_MSGS;
    posixAttr.mq_msgsize = MSG_SIZE;

    priv_qid = mq_open(client_path, O_RDONLY | O_CREAT, 0666, &posixAttr);
    if (priv_qid == -1) EXIT_MSG("Client: creation of private queue failed\n");
    register_client();

    char request[50];
    Message msg;
    while (1) {
        msg.sender_pid = getpid();
        printf("Client: please enter your request:\n");
        if (fgets(request, 50, stdin) == NULL) {
            printf("Client: invalid request, probably too long\n");
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
            printf("Client: unknown request %s\n", request);
        }
    }
}

void register_client() {
    Message msg;
    msg.type = REGISTER;
    msg.sender_pid = getpid();

    if (mq_send(pub_qid, (char*) &msg, MSG_SIZE, 1) == -1) {
        EXIT_MSG("Client: REGISTER request sending failed\n");
    }
    if (mq_receive(priv_qid, (char*) &msg, MSG_SIZE, NULL) == -1) {
        EXIT_MSG("Client: getting REGISTER response failed\n");
    }
    if (sscanf(msg.content, "%d", &session_num) < 1) {
        EXIT_MSG("Client: parsing REGISTER response failed\n");
    }
    if (session_num < 0) {
        EXIT_MSG("Client: server was unable to register any more clients\n");
    }
    printf("Client: registration successful, session: %d\n", session_num);
}

void request_mirror(struct Message *msg){
    msg->type = MIRROR;
    printf("Client: enter a string to mirror: \n");
    if (fgets(msg->content, MAX_CONT_SIZE, stdin) == NULL) {
        printf("Client: string too long, unable to proceed\n");
        return;
    }
    if (mq_send(pub_qid, (char*) msg, MSG_SIZE, 1) == -1) {
        EXIT_MSG("Client: MIRROR request sending failed\n");
    }
    if (mq_receive(priv_qid, (char*) msg, MSG_SIZE, NULL) == -1) {
        EXIT_MSG("Client: getting MIRROR response failed\n");
    }
    printf("%s", msg->content);
}

void request_time(struct Message *msg){
    msg->type = TIME;
    if (mq_send(pub_qid, (char*) msg, MSG_SIZE, 1) == -1) {
        EXIT_MSG("Client: TIME request sending failed\n");
    }
    if (mq_receive(priv_qid, (char*) msg, MSG_SIZE, NULL) == -1) {
        EXIT_MSG("Client: getting TIME response failed\n");
    }
    printf("%s\n", msg->content);
}

void request_end(struct Message *msg) {
    msg->type = END;

    if (mq_send(pub_qid, (char*) msg, MSG_SIZE, 1) == -1) {
        EXIT_MSG("Client: END request sending failed\n");
    }
}

void request_quit(struct Message *msg) {
    msg->type = QUIT;

    if (mq_send(pub_qid, (char*) msg, MSG_SIZE, 1) == -1) {
        printf("Client: QUIT request sending failed\n");
    }
    fflush(stdout);
}

void close_queue() {
    if (priv_qid > -1) {
        if (session_num >= 0) {
            printf("Client: sending QUIT request to server\n");
            Message msg;
            msg.sender_pid = getpid();
            request_quit(&msg);
        }

        if (mq_close(pub_qid) == -1) {
            printf("Client: error during closing of public queue\n");
        } else {
            printf("Client: public queue closed successfully\n");
        }

        if (mq_close(priv_qid) == -1) {
            printf("Client: error during closing of private queue\n");
        } else {
            printf("Client: private queue closed successfully\n");
        }

        if (mq_unlink(client_path) == -1) {
            printf("Client: error during deleting of private queue\n");
        } else {
            printf("Client: private queue deleted successfully\n");
        }
    } else {
        printf("Client: queue was already deleted\n");
    }
}

// handle SIGINT signals
void sigint_handler(int _) {
    exit(2);
}