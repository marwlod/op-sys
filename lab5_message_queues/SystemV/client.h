#ifndef CLIENT_H
#define CLIENT_H

#define EXIT_MSG(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
void register_client(key_t key);
void request_mirror(Message *msg);
void request_time(Message *msg);
void request_end(Message *msg);
int create_priv_queue(char*, int);
void close_queue();
void sigint_handler();

int session_num = -2;
int queue_desc = -1;
int priv_qid = -1;

#endif