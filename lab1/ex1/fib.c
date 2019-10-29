#include <stdio.h>
#include <time.h>

int fib(int n)
{
    if (n == 0 || n == 1) return n;
    else return fib(n-1) + fib(n-2);
}

int main()
{
    long one_bill = 1 * 1000 * 1000 * 1000;
    int n = 43;
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    fib(n);
    clock_gettime(CLOCK_REALTIME, &end);

    long start_nanos = start.tv_sec * one_bill + start.tv_nsec;
    long end_nanos = end.tv_sec * one_bill + end.tv_nsec;
    long duration_secs = (end_nanos - start_nanos) / one_bill;
    long duration_nanos = (end_nanos - start_nanos) % one_bill;
    printf("Program duration was %ld secs and %ld nanos\n", duration_secs, duration_nanos);
    return 0;
}