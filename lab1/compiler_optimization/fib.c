#include <stdio.h>
#include <time.h>
#include "utils.c"

int fib(int n)
{
    if (n == 0 || n == 1) return n;
    else return fib(n-1) + fib(n-2);
}

int main()
{
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    // highest number to complete in relatively short time
    fib(43);
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec duration = calc_duration(start, end);
    printf("Program duration was %ld secs and %ld nanos\n", duration.tv_sec, duration.tv_nsec);
    return 0;
}