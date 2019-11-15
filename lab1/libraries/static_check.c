#include <stdio.h>
#include <time.h>
#include "sort_library.c"
#include "utils.c"

int main()
{
    int n = sizeof(arr)/sizeof(int);
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < 5*M; i++) {
        insertion_sort(arr, n);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec duration = calc_duration(start, end);
    printf("Program duration was %ld secs and %ld nanos\n", duration.tv_sec, duration.tv_nsec);
    return 0;
}