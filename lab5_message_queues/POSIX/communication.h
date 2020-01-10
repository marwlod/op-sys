#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#define MAX_CLIENTS  10
#define PROJECT_ID 0x099
#define MAX_CONT_SIZE 4096
#define MESSAGE_SIZE sizeof(Message)
#define MAX_MESSAGE_QUEUE_SIZE 9

typedef enum type {
    REGISTER = 1, MIRROR = 2, CALC = 3, TIME = 4, END = 5, INIT = 6, QUIT = 7
} type;

typedef struct Message {
    long type;
    pid_t sender_pid;
    char content[MAX_CONT_SIZE];
} Message;

const char server_path[] = "/server";

#endif