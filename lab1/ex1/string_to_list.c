#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.c"

struct digit {
    long dig;
    struct digit *next;
};

// transforms number (123456) into list representing number (<root> 6->5->4->3->2->1 <tail>)
struct digit *create_num_as_list(struct digit *a, long number) {
    if (!a) return NULL;

    struct digit *trav = a;
    trav->dig = number % 10;
    number = number / 10;
    while (number > 0) {
        trav->next = (struct digit *) malloc(sizeof(struct digit));
        trav = trav->next;
        trav->dig = number % 10;
        number = number / 10;
    }
    trav->next = NULL;
    return a;
}

void free_list(struct digit *head) {
    struct digit *trav = head, *prev = NULL;
    while (trav) {
        prev = trav;
        trav = trav->next;
        free(prev);
    }
}

// adds up two lists representing numbers, e.g. (1<-2<-3) + (2<-3<-4) = (3<-5<-7), stores result in first list
void add(struct digit *a, struct digit *b) {
    if (!a && !b) return;

    if (!a) {
        a = (struct digit *) calloc(1, sizeof(struct digit));
    }
    struct digit *travadded = a, *travb = b;
    long leftover = 0, sum = 0;

    while (travb || leftover) {
        sum = travadded->dig + (travb ? travb->dig : 0) + leftover;
        travadded->dig = sum % 10;
        leftover = sum / 10;
        if (travb) travb = travb->next;

        // if still some more digits to add while no more digits in result
        if (!travadded->next && (travb || leftover)) {
            travadded->next = (struct digit *) calloc(1, sizeof(struct digit));
        }
        travadded = travadded->next;
    }
}

int main()
{
    struct digit *total = (struct digit *) malloc(sizeof(struct digit));
    struct digit *to_add = (struct digit *) malloc(sizeof(struct digit));
    total = create_num_as_list(total, 999);
    to_add = create_num_as_list(to_add, 123456);
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    // repeat a lot of times to make program last long, thus make it possible to notice potential compiler optimisation
    // (reduction of program execution duration)
    for (int i = 0; i < 5*M; i++) {
        add(total, to_add);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    free_list(total);
    free_list(to_add);
    struct timespec duration = calc_duration(start, end);
    printf("Program duration was %ld secs and %ld nanos\n", duration.tv_sec, duration.tv_nsec);
    return 0;
}
