#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <mqueue.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>

#include "communication.h"

#define EXIT_MSG(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }

void dispatch_requests(struct Message *msg);
void handle_register(struct Message *msg);
void handle_mirror(struct Message *msg);
void handle_calc(struct Message *msg);
void handle_time(struct Message *msg);
void handle_end(struct Message *msg);
void handle_quit(struct Message *msg);
int get_qid(pid_t client_pid);
int create_msg(struct Message *msg);
char* convert_time(const time_t *mtime);
void close_queue();
void sigint_handler(int);

int active = 1;
int clients_data[MAX_CLIENTS][2];
int client_count = 0;
mqd_t queue_desc = -1;

// MAIN ////////////////////////////////////////////////////////////////////////

int main() {
    if (atexit(close_queue) == -1)
    EXIT_MSG("server: registering server's atexit failed\n");
    if (signal(SIGINT, sigint_handler) == SIG_ERR)
    EXIT_MSG("server: registering INT failed\n");

    struct mq_attr current_state;
    struct mq_attr posix_attr;
    posix_attr.mq_maxmsg = MAX_MESSAGE_QUEUE_SIZE;
    posix_attr.mq_msgsize = MESSAGE_SIZE;

    queue_desc = mq_open(server_path, O_RDONLY | O_CREAT, 0666, &posix_attr);

    if (queue_desc == -1)
    EXIT_MSG("server: creation of public queue failed\n");

    Message buffer;
    while(1) {
        if(active == 0) {
            if (mq_getattr(queue_desc, &current_state) == -1)
            EXIT_MSG("server: couldnt read public queue parameters\n");
            if (current_state.mq_curmsgs == 0) exit(0);
        }

        if (mq_receive(queue_desc, (char*) &buffer, MESSAGE_SIZE, NULL) == -1)
        EXIT_MSG("server: receiving message by server failed\n");
        dispatch_requests(&buffer);
    }
}

void dispatch_requests(struct Message *msg) {
    if (msg == NULL) return;
    switch(msg->type) {
        case REGISTER:
            handle_register(msg);
            break;
        case MIRROR:
            handle_mirror(msg);
            break;
        case CALC:
            handle_calc(msg);
            break;
        case TIME:
            handle_time(msg);
            break;
        case END:
            handle_end(msg);
            break;
        case QUIT:
            handle_quit(msg);
            break;
        default:
            break;
    }
}

void handle_register(struct Message *msg) {
    int clientPID = msg->sender_pid;
    char clientPath[15];
    sprintf(clientPath, "/%d", clientPID);

    int client_queue_id = mq_open(clientPath, O_WRONLY);
    if (client_queue_id == -1) EXIT_MSG("server: reading client_queue_id failed\n");

    msg->type = INIT;
    msg->sender_pid = getpid();

    if (client_count > MAX_CLIENTS - 1) {
        printf("server: maximum amount of clients reached\n");
        sprintf(msg->content, "%d", -1);
        if (mq_send(client_queue_id, (char*) msg, MESSAGE_SIZE, 1) == -1)
        EXIT_MSG("server: login response failed\n");
        if (mq_close(client_queue_id) == -1)
        EXIT_MSG("server: closing client's queue failed\n");
    } else {
        clients_data[client_count][0] = clientPID;
        clients_data[client_count++][1] = client_queue_id;
        sprintf(msg->content, "%d", client_count - 1);
        if (mq_send(client_queue_id, (char*) msg, MESSAGE_SIZE, 1) == -1)
        EXIT_MSG("server: login response failed\n");
    }
}

void handle_mirror(struct Message *msg) {
    int client_queue_id = create_msg(msg);
    if (client_queue_id == -1) return;

    int msgLen = (int) strlen(msg->content);
    if (msg->content[msgLen - 1] == '\n') msgLen--;

    int i; for (i = 0; i < msgLen / 2; ++i) {
        char buff = msg->content[i];
        msg->content[i] = msg->content[msgLen - i - 1];
        msg->content[msgLen - i - 1] = buff;
    }

    if (mq_send(client_queue_id, (char*) msg, MESSAGE_SIZE, 1) == -1)
    EXIT_MSG("server: MIRROR response failed\n");
}

void handle_calc(struct Message *msg) {
    int client_queue_id = create_msg(msg);
    if (client_queue_id == -1) return;

    char cmd[4108];
    sprintf(cmd, "echo '%s' | bc", msg->content);
    FILE* calc = popen(cmd, "r");
    fgets(msg->content, MAX_CONT_SIZE, calc);
    pclose(calc);

    if (mq_send(client_queue_id, (char*) msg, MESSAGE_SIZE, 1) == -1)
    EXIT_MSG("server: CALC response failed\n");
}

void handle_time(struct Message *msg) {
    int client_queue_id = create_msg(msg);
    if (client_queue_id == -1) return;

    time_t timer;
    time(&timer);
    char* timeStr = convert_time(&timer);

    sprintf(msg->content, "%s", timeStr);
    free(timeStr);

    if (mq_send(client_queue_id, (char*) msg, MESSAGE_SIZE, 1) == -1)
    EXIT_MSG("server: TIME response failed");
}

void handle_end(struct Message *msg) { active = 0; }

void handle_quit(struct Message *msg) {
    int i; for (i = 0; i<client_count; ++i) {
        if(clients_data[i][0] == msg->sender_pid) break;
    }
    if(i == client_count) {
        printf("server: client not found\n");
        return;
    }
    if (mq_close(clients_data[i][1]) == -1)
    EXIT_MSG("server: closing clients queue in QUIT response failed\n");
    for (; i + 1 < client_count; ++i) {
        clients_data[i][0] = clients_data[i + 1][0];
        clients_data[i][1] = clients_data[i + 1][1];
    }
    client_count--;
    printf("server: cleared data of removed client\n");
}

// HELPERS /////////////////////////////////////////////////////////////////////

int create_msg(struct Message *msg) {
    int client_queue_id  = get_qid(msg->sender_pid);
    if (client_queue_id == -1) {
        printf("server: client not found\n");
        return -1;
    }

    msg->type = msg->sender_pid;
    msg->sender_pid = getpid();

    return client_queue_id;
}

int get_qid(pid_t sender_pid) {
    int i; for (i = 0; i < client_count; ++i)
        if (clients_data[i][0] == sender_pid)
            return clients_data[i][1];
    return -1;
}

void close_queue() {
    int i; for (i = 0; i < client_count; ++i) {
        if (mq_close(clients_data[i][1]) == -1) {
            printf("server: error closing %d client queue\n", i);
        }
        if (kill(clients_data[i][0], SIGINT) == -1) {
            printf("server: error killing %d client\n", i);
        }
    }
    if (queue_desc > -1) {
        if(mq_close(queue_desc) == -1) {
            printf("server: error closing public queue\n");
        } else {
            printf("server: queue closed\n");
        }

        if (mq_unlink(server_path) == -1) {
            printf("server: error deleting public queue\n");
        } else {
            printf("server: queue deleted successfully\n");
        }
    } else {
        printf("server: there was no need of deleting queue\n");
    }
}

char* convert_time(const time_t *mtime) {
    char* buff = malloc(sizeof(char) * 30);
    struct tm * timeinfo;
    timeinfo = localtime (mtime);
    strftime(buff, 20, "%b %d %H:%M", timeinfo);
    return buff;
}

void sigint_handler(int _) {
    active = 0;
    exit(2);
}
