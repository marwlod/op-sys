#ifndef CLIENT_H
#define CLIENT_H

#define EXIT_MSG(format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(-1); }
void register_client();
void request_mirror(struct Message *msg);
void request_time(struct Message *msg);
void request_end(struct Message *msg);
void request_quit(struct Message *msg);
void close_queue();
void sigint_handler(int);

int session_num = -2;
mqd_t pub_qid = -1;
mqd_t priv_qid = -1;
char client_path[20];
#endif