#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mqueue.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include "common.h"
#include "server.h"

int main() {
    if (atexit(close_queue) == -1) {
        EXIT_MSG("Server: registering 'atexit' failed\n");
    }
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        EXIT_MSG("Server: registering SIGINT handler failed\n");
    }
    struct mq_attr current_state;
    struct mq_attr posix_attr;
    posix_attr.mq_maxmsg = MAX_MSGS;
    posix_attr.mq_msgsize = MSG_SIZE;

    queue_desc = mq_open(server_path, O_RDONLY | O_CREAT, 0666, &posix_attr);

    if (queue_desc == -1) {
        EXIT_MSG("Server: creation of public queue failed\n");
    }

    Message msg;
    while (1) {
        if (active == 0) {
            if (mq_getattr(queue_desc, &current_state) == -1) {
                EXIT_MSG("Server: getting of attributes of public queue failed\n");
            }
            if (current_state.mq_curmsgs == 0) exit(0);
        }

        if (mq_receive(queue_desc, (char*) &msg, MSG_SIZE, NULL) == -1)
        EXIT_MSG("server: receiving message by server failed\n");
        dispatch_request(&msg);
    }
}

void dispatch_request(struct Message *msg) {
    if (msg == NULL) return;
    if (msg->type == REGISTER) {
        handle_register(msg);
    } else if (msg->type == MIRROR) {
        handle_mirror(msg);
    } else if (msg->type == TIME) {
        handle_time(msg);
    } else if (msg->type == END) {
        handle_end(msg);
    } else if (msg->type == QUIT) {
        handle_quit(msg);
    }
}

void handle_register(struct Message *msg) {
    int client_pid = msg->sender_pid;
    char client_path[20];
    sprintf(client_path, "/%d", client_pid);

    int client_qid = mq_open(client_path, O_WRONLY);
    if (client_qid == -1) EXIT_MSG("Server: reading client queue ID failed\n");
    msg->type = REGISTER;
    msg->sender_pid = getpid();

    if (client_count > MAX_CLIENTS - 1) {
        printf("Server: maximum amount of clients reached\n");
        sprintf(msg->content, "%d", -1);
        if (mq_send(client_qid, (char*) msg, MSG_SIZE, 1) == -1) {
            EXIT_MSG("server: login response failed\n");
        }
        if (mq_close(client_qid) == -1) {
            EXIT_MSG("server: closing client's queue failed\n");
        }
    } else {
        clients_data[client_count][0] = client_pid;
        clients_data[client_count++][1] = client_qid;
        sprintf(msg->content, "%d", client_count - 1);
        if (mq_send(client_qid, (char*) msg, MSG_SIZE, 1) == -1) {
            EXIT_MSG("server: login response failed\n");
        }
    }
}

void handle_mirror(struct Message *msg) {
    int client_queue_id = create_msg(msg);
    if (client_queue_id == -1) return;

    int msgLen = (int) strlen(msg->content);
    if (msg->content[msgLen-1] == '\n') {
        msgLen--;
    }

    for (int i = 0; i < msgLen / 2; i++) {
        char buff = msg->content[i];
        msg->content[i] = msg->content[msgLen-i-1];
        msg->content[msgLen-i-1] = buff;
    }

    if (mq_send(client_queue_id, (char*) msg, MSG_SIZE, 1) == -1) {
        EXIT_MSG("Server: MIRROR response sending failed\n");
    }
}

void handle_time(struct Message *msg) {
    int client_queue_id = create_msg(msg);
    if (client_queue_id == -1) return;

    time_t timet;
    time(&timet);
    char* time_str = convert_time(&timet);

    sprintf(msg->content, "%s", time_str);
    free(time_str);

    if (mq_send(client_queue_id, (char*) msg, MSG_SIZE, 1) == -1) {
        EXIT_MSG("Server: TIME response sending failed");
    }
}

void handle_end(struct Message *msg) {
    active = 0;
}

void handle_quit(struct Message *msg) {
    int i = 0;
    for (; i < client_count; i++) {
        if (clients_data[i][0] == msg->sender_pid) {
            break;
        }
    }
    if (i == client_count) {
        printf("Server: client not registered\n");
        return;
    }
    if (mq_close(clients_data[i][1]) == -1) {
        EXIT_MSG("Server: closing client's queue in during handling of QUIT request failed\n");
    }
    for (; i < client_count-1; i++) {
        clients_data[i][0] = clients_data[i+1][0];
        clients_data[i][1] = clients_data[i+1][1];
    }
    client_count--;
    printf("Server: data of client that quit was deleted\n");
}

int create_msg(struct Message *msg) {
    int client_queue_id  = get_qid(msg->sender_pid);
    if (client_queue_id == -1) {
        printf("Server: client not registered\n");
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
    for (int i = 0; i < client_count; i++) {
        if (mq_close(clients_data[i][1]) == -1) {
            printf("Server: error during closing %d client queue\n", i);
        }
        if (kill(clients_data[i][0], SIGINT) == -1) {
            printf("Server: error sending SIGINT to %d client\n", i);
        }
    }
    if (queue_desc > -1) {
        if (mq_close(queue_desc) == -1) {
            printf("Server: error during closing of public queue\n");
        } else {
            printf("Server: queue was closed successfully\n");
        }
        if (mq_unlink(server_path) == -1) {
            printf("Server: error during deletion public queue\n");
        } else {
            printf("Server: queue was deleted successfully\n");
        }
    } else {
        printf("Server: queue not found, could be already deleted\n");
    }
}

char* convert_time(const time_t *mtime) {
    char* buff = malloc(sizeof(char) * 30);
    struct tm * tm;
    tm = localtime (mtime);
    strftime(buff, 20, "%d.%m.%Y %H:%M:%S", tm);
    return buff;
}

void sigint_handler(int _) {
    active = 0;
    exit(2);
}
