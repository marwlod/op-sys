#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <signal.h>
#include "common.h"
#include "server.h"

int main() {
    if (atexit(close_queue) == -1) {
        EXIT_MSG("Server: registering server's atexit failed\n");
    }
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        EXIT_MSG("Server: registering SIGINT failed\n");
    }
    struct msqid_ds current_state;

    char* homePath = getenv("HOME");
    if (homePath == NULL) {
        EXIT_MSG("Server: cannot get 'HOME' env variable\n");
    }

    key_t key = ftok(homePath, PROJECT_ID);
    if (key == -1) {
        EXIT_MSG("Server: key generation for public queue failed\n");
    }

    queue_desc = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if (queue_desc == -1) {
        EXIT_MSG("Server: public queue creation failed\n");
    }

    Message buffer;
    while(1) {
        if (active == 0) {
            if (msgctl(queue_desc, IPC_STAT, &current_state) == -1) {
                EXIT_MSG("Server: getting current state of public queue failed\n");
            }
            if (current_state.msg_qnum == 0) break;
        }

        if (msgrcv(queue_desc, &buffer, MSG_SIZE, 0, 0) < 0) {
            EXIT_MSG("Server: receiving message failed\n");
        }
        dispatch_requests(&buffer);
    }
    return 0;
}

void dispatch_requests(struct Message *msg) {
    if (msg == NULL) return;
    switch(msg->type){
        case REGISTER:
            handle_register(msg);
            break;
        case MIRROR:
            handle_mirror(msg);
            break;
        case TIME:
            handle_time(msg);
            break;
        case END:
            handle_end(msg);
            break;
        default:
            break;
    }
}

void handle_register(struct Message *msg) {
    key_t client_qkey;
    if (sscanf(msg->content, "%d", &client_qkey) < 0) {
        EXIT_MSG("Server: reading client queue key failed\n");
    }

    int client_qid = msgget(client_qkey, 0);
    if (client_qid == -1) {
        EXIT_MSG("Server: reading client queue ID failed\n");
    }

    int client_pid = msg->sender_pid;
    msg->type = REGISTER;
    msg->sender_pid = getpid();

    if (client_count > MAX_CLIENTS - 1) {
        printf("Server: maximum number of clients reached\n");
        sprintf(msg->content, "%d", -1);
    } else {
        clients_data[client_count][0] = client_pid;
        clients_data[client_count++][1] = client_qid;
        sprintf(msg->content, "%d", client_count - 1);
    }

    if (msgsnd(client_qid, msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("Server: REGISTER response sending failed\n");
    }
}

void handle_mirror(Message *msg) {
    int client_queue_id = create_message(msg);
    if(client_queue_id == -1) return;

    int message_length = (int) strlen(msg->content);
    if (msg->content[message_length - 1] == '\n') {
        message_length--;
    }

    for (int i = 0; i < message_length / 2; i++) {
        char temp = msg->content[i];
        msg->content[i] = msg->content[message_length - i - 1];
        msg->content[message_length - i - 1] = temp;
    }

    if (msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("server: MIRROR response sending failed\n");
    }
}

void handle_time(struct Message *msg) {
    int client_queue_id = create_message(msg);
    if(client_queue_id == -1) return;
    time_t timet;
    time(&timet);
    char *time_str = convert_time(&timet);

    sprintf(msg->content, "%s", time_str);
    free(time_str);

    if (msgsnd(client_queue_id, msg, MSG_SIZE, 0) == -1) {
        EXIT_MSG("server: TIME response failed\n");
    }
}

void handle_end(struct Message *msg) {
    active = 0;
}

int create_message(struct Message *msg) {
    int client_queue_id = get_qid(msg->sender_pid);
    if (client_queue_id == -1) {
        printf("server: client not found\n");
        return -1;
    }

    msg->type = msg->sender_pid;
    msg->sender_pid = getpid();

    return client_queue_id;
}

// get identifier of queue by client PID
int get_qid(pid_t client_pid) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients_data[i][0] == client_pid) {
            return clients_data[i][1];
        }
    }
    return -1;
}

// covert time to local
char *convert_time(const time_t *mtime) {
    char *buff = malloc(sizeof(char) * 30);
    struct tm *tm;
    tm = localtime(mtime);
    strftime(buff, 20, "%d.%m.%Y %H:%M:%S", tm);
    return buff;
}

// close and remove queue
void close_queue() {
    if (queue_desc > -1) {
        int tmp = msgctl(queue_desc, IPC_RMID, NULL);
        if (tmp == -1) {
            printf("Server: error during deleting of queue\n");
        }
        printf("Server: queue deletion succeeded\n");
    }
}

// exit when SIGINT received
void sigint_handler(int _) {
    exit(2);
}