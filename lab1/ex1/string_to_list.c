#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct digit {
    long dig;
    struct digit* next;
};

// transforms number (123456) into list representing number (<root> 6->5->4->3->2->1 <tail>)
struct digit* create_num_as_list(struct digit* a, long number) {
    if (!a) return NULL;

    struct digit* trav = a;
    trav->dig = number % 10;
    number = number / 10;
    while (number > 0) {
        trav->next = malloc(sizeof(struct digit));
        trav = trav->next;
        trav->dig = number % 10;
        number = number / 10;
    }
    trav->next = NULL;
    return a;
}

// adds up two lists representing numbers, e.g. (1<-2<-3) + (2<-3<-4) = (3<-5<-7)
struct digit* add(struct digit* a, struct digit* b) {
    if (!a && !b) return NULL;

    struct digit* added = malloc(sizeof(struct digit));
    struct digit* travadded = added, *trava = a, *travb = b;
    long leftover = 0, sum = 0;

    while (trava || travb || leftover) {
        sum = (trava ? trava->dig : 0) +
              (travb ? travb->dig : 0) + leftover;
        travadded->dig = sum % 10;
        leftover = sum / 10;

        if (trava) trava = trava->next;
        if (travb) travb = travb->next;

        if (trava || travb || leftover) {
            travadded->next = malloc(sizeof(struct digit));
            travadded = travadded->next;
        }
    }
    return added;
}

int main()
{
    long one_bill = 1 * 1000 * 1000 * 1000;
    struct digit *total = malloc(sizeof(struct digit));
    struct digit *to_add = malloc(sizeof(struct digit));
    total = create_num_as_list(total, one_bill);
    to_add = create_num_as_list(to_add, one_bill);
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < 5 * 1000 * 1000; i++) {
        total = add(total, to_add);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    long start_nanos = start.tv_sec * one_bill + start.tv_nsec;
    long end_nanos = end.tv_sec * one_bill + end.tv_nsec;
    long duration_secs = (end_nanos - start_nanos) / one_bill;
    long duration_nanos = (end_nanos - start_nanos) % one_bill;
    printf("Program duration was %ld secs and %ld nanos\n", duration_secs, duration_nanos);
    return 0;
}
