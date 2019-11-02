#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.c"

struct node
{
    int val;
    struct node *next;
};

int main()
{
    struct node *trav = malloc(sizeof(struct node));
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    // allocating new memory, losing reference to previously allocated memory
    for (int i = 0; i < 100*M; i++) {
        trav->val = i;
        trav->next = malloc(sizeof(struct node));
        trav = trav->next;
    }
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec duration = calc_duration(start, end);
    printf("Program duration was %ld secs and %ld nanos\n", duration.tv_sec, duration.tv_nsec);
    return 0;
}