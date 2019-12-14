#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <memory.h>
#include <wait.h>

#define EXIT(code, format, ...) { fprintf(stderr, format, ##__VA_ARGS__); exit(code);}
#define PRINT_OUT(format, ...) { char buffer[255]; sprintf(buffer, format, ##__VA_ARGS__); write(1, buffer, strlen(buffer));}

volatile int L;
volatile int TYPE;
volatile int sent_to_child = 0;
volatile int received_by_child = 0;
volatile int received_by_parent = 0;
volatile pid_t cpid;

void print_stats()
{
    printf("Signals sent to child: %d\n", sent_to_child);
    printf("Signals received by parent: %d\n", received_by_parent);
}

void child_handler(int signum, siginfo_t *info, void *context)
{
    if (signum == SIGINT)
    {
        sigset_t mask;
        sigfillset(&mask);
        sigprocmask(SIG_SETMASK, &mask, NULL);
        PRINT_OUT("Signals received by child: %d\n", received_by_child);
        exit((unsigned) received_by_child);
    }
    if (info->si_pid != getppid()) return;

    if (TYPE == 1 || TYPE == 2) 
    {
        if (signum == SIGUSR1) 
        {
            received_by_child++;
            kill(getppid(), SIGUSR1);
            PRINT_OUT("Child: SIGUSR1 received and sending back\n")
        } 
        else if (signum == SIGUSR2) 
        {
            received_by_child++;
            PRINT_OUT("Child: SIGUSR2 received, terminating\n")
            PRINT_OUT("Signals received by child: %d\n", received_by_child);
            exit((unsigned) received_by_child);
        }
    } 
    else if (TYPE == 3) 
    {
        if (signum == SIGRTMIN) 
        {
            received_by_child++;
            kill(getppid(), SIGRTMIN);
            PRINT_OUT("Child: SIGRTMIN received and sending back\n")
        } 
        else if (signum == SIGRTMAX) 
        {
            received_by_child++;
            PRINT_OUT("Child: SIGRTMAX received, terminating\n")
            PRINT_OUT("Signals received by child: %d\n", received_by_child);
            exit((unsigned) received_by_child);
        }
    }
}

void parent_handler(int signum, siginfo_t *info, void *context) 
{
    if (signum == SIGINT) 
    {
        PRINT_OUT("Parent: received SIGINT\n");
        kill(cpid, SIGUSR2);
        print_stats();
        exit(9);
    }
    if (info->si_pid != cpid) return;

    if ((TYPE == 1 || TYPE == 2) && signum == SIGUSR1) 
    {
        received_by_parent++;
        PRINT_OUT("Parent: received SIGUSR1 from child\n");
    } 
    else if (TYPE == 3 && signum == SIGRTMIN) 
    {
        received_by_parent++;
        PRINT_OUT("Parent: received SIGRTMIN from child\n");
    }
}

void child_process() 
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = child_handler;

    if (sigaction(SIGINT, &act, NULL) == -1) EXIT(1, "Setting up SIGINT handling failed\n");
    if (sigaction(SIGUSR1, &act, NULL) == -1) EXIT(1, "Setting up SIGUSR1 handling failed\n");
    if (sigaction(SIGUSR2, &act, NULL) == -1) EXIT(1, "Setting up SIGUSR2 handling failed\n");
    if (sigaction(SIGRTMIN, &act, NULL) == -1) EXIT(1, "Setting up SIGRTMIN handling failed\n");
    if (sigaction(SIGRTMAX, &act, NULL) == -1) EXIT(1, "Setting up SIGRTMAX handling failed\n");

    while (1) 
    {
        sleep(1);
    }
}

void parent_process() 
{
    sleep(1);

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = parent_handler;

    if (sigaction(SIGINT, &act, NULL) == -1) EXIT(1, "Setting up SIGINT handling failed\n");
    if (sigaction(SIGUSR1, &act, NULL) == -1) EXIT(1, "Setting up SIGUSR1 handling failed\n");
    if (sigaction(SIGRTMIN, &act, NULL) == -1) EXIT(1, "Setting up SIGRTMIN handling failed\n");

    if (TYPE == 1 || TYPE == 2) 
    {
        sigset_t mask;
        sigfillset(&mask);
        sigdelset(&mask, SIGUSR1);
        sigdelset(&mask, SIGINT);
        for (; sent_to_child < L; sent_to_child++)
        {
            PRINT_OUT("Parent: sending SIGUSR1\n");
            kill(cpid, SIGUSR1);
            if (TYPE == 2) sigsuspend(&mask);
        }
        PRINT_OUT("Parent: sending SIGUSR2\n");
        kill(cpid, SIGUSR2);
    } 
    else if (TYPE == 3)
    {
        for (; sent_to_child < L; sent_to_child++)
        {
            PRINT_OUT("Parent: sending SIGRTMIN\n");
            kill(cpid, SIGRTMIN);
        }
        sent_to_child++;
        PRINT_OUT("Parent: sending SIGRTMAX\n");
        kill(cpid, SIGRTMAX);
    }

    int status = 0;
    waitpid(cpid, &status, 0);
    if (WIFEXITED(status)) 
    {
        received_by_child = WEXITSTATUS(status);
    }
    else EXIT(1, "Terminating of child failed\n");
}

int main(int argc, char *argv[]) {

    if (argc != 3)
    {
        fprintf(stderr, "Usage ./user_sigs L type\n");
        return 1;
    }
    L = (int) strtol(argv[1], NULL, 10);
    TYPE = (int) strtol(argv[2], NULL, 10);

    if (L < 1)
    {
        fprintf(stderr, "Invalid L (number of signals) argument\n");
        return 1;
    }
    if (TYPE < 1 || TYPE > 3)
    {
        fprintf(stderr, "Invalid type argument: only 1, 2 and 3 allowed\n");
        return 1;
    }

    cpid = fork();
    if (!cpid) 
    {
        child_process();
    }
    else if (cpid > 0) 
    {
        parent_process();
    }
    else 
    {
        fprintf(stderr, "Error during forking\n");
        return 2;
    }
    print_stats();
    return 0;
}

