#ifndef COMMON_H
#define COMMON_H

#define MAX_CONT_SIZE 2048
#define MAX_CLIENTS  5
#define MSG_SIZE sizeof(Message)
#define MAX_MSGS 9

typedef struct Message {
    long type;
    pid_t sender_pid;
    char content[MAX_CONT_SIZE];
} Message;
typedef enum type {
    REGISTER = 1, MIRROR = 2, TIME = 3, END = 4, QUIT = 5
} type;

const char server_path[] = "/server";
#endif