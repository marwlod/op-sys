#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct node
{
    int val;
    struct node *next;
};

int main()
{
    long one_bill = 1 * 1000 * 1000 * 1000;
    struct node *trav = malloc(sizeof(struct node));
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    // allocating new memory, losing reference to previously allocated memory
    for (int i = 0; i < 100 * 1000 * 1000; i++) {
        trav->val = i;
        trav->next = malloc(sizeof(struct node));
        trav = trav->next;
    }
    clock_gettime(CLOCK_REALTIME, &end);

    long start_nanos = start.tv_sec * one_bill + start.tv_nsec;
    long end_nanos = end.tv_sec * one_bill + end.tv_nsec;
    long duration_secs = (end_nanos - start_nanos) / one_bill;
    long duration_nanos = (end_nanos - start_nanos) % one_bill;
    printf("Program duration was %ld secs and %ld nanos\n", duration_secs, duration_nanos);
    return 0;
}