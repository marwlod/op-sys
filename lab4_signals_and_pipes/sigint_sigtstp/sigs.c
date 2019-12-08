#include <zconf.h>
#include <wait.h>
#include <stdio.h>
#include <stdlib.h>

pid_t child_pid = 0;
int is_stopped = 0;

void stop_resume() {
    printf("\nOczekuje na CTRL+Z - kontynuacja albo CTR+C - zakonczenie programu\n");
    if (is_stopped) {
        child_pid = fork();
        if (child_pid == 0) {
            int status = execl("./date.sh", "./date.sh", NULL);
            if (status < 0) {
                exit(4);
            }
        }
        is_stopped = 0;
    } else {
        kill(child_pid, SIGKILL);
        is_stopped = 1;
    }
}

void pkill() {
    printf("\nOdebrano sygnal SIGINT\n");
    kill(child_pid, SIGKILL);
    exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[]) {
    struct sigaction actions;
    actions.sa_flags = 0;
    sigemptyset(&actions.sa_mask);
    actions.sa_handler = stop_resume;

    pid_t cpid = fork();
    // parent process
    if (cpid) {
        child_pid = cpid;
        int cstat;
        sigaction(SIGTSTP, &actions, NULL);
        signal(SIGINT, pkill);
        while (1) {
            waitpid(cpid, &cstat, 0);
            if (cstat == -1) {
                return 3;
            }
        }
    }
    // child process
    else {
        int status = execl("./date.sh", "date.sh", NULL);
        if (status < 0) {
            return 4;
        }
    }
}