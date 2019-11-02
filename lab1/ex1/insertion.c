#include <stdio.h>
#include <time.h>
#include "utils.c"

void insertion_sort(int *arr, int n)
{
    int temp;
    for (int i = 1; i < n; i++) {
        for (int j = i; j > 0; j--) {
            if (arr[j-1] > arr[j]) {
                temp = arr[j];
                arr[j] = arr[j-1];
                arr[j-1] = temp;
            }
        }
    }
}

int main()
{
    // array sorted in descending order, worst case for insertion sort that sorts in ascending order
    int arr[] = {20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
    int n = sizeof(arr)/sizeof(int);
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    // repeat a lot of times to make program last long, thus make it possible to notice potential compiler optimisation
    // (reduction of program execution duration)
    for (int i = 0; i < 5*M; i++) {
        insertion_sort(arr, n);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    struct timespec duration = calc_duration(start, end);
    printf("Program duration was %ld secs and %ld nanos\n", duration.tv_sec, duration.tv_nsec);
    return 0;
}