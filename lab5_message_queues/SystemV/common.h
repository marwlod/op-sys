#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define MAX_CONT_SIZE 2048
#define MAX_CLIENTS  15
#define PROJECT_ID 0x007

typedef struct Message {
    long type;
    pid_t sender_pid;
    char content[MAX_CONT_SIZE];
} Message;

typedef enum type {
    REGISTER = 1, MIRROR = 2, TIME = 3, END = 4
} type;

const size_t MSG_SIZE = sizeof(Message) - sizeof(long);
#endif