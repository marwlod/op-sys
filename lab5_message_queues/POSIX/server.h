#ifndef SERVER_H
#define SERVER_H

#include <mqueue.h>
#define EXIT_MSG(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }

void dispatch_request(struct Message *msg);
void handle_register(struct Message *msg);
void handle_mirror(struct Message *msg);
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
#endif