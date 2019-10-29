#include <dlfcn.h>
#include <stdio.h>
#include <time.h>

int main()
{
    long one_bill = 1 * 1000 * 1000 * 1000;
    int arr[] = {20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1};
    int n = sizeof(arr)/sizeof(int);
    struct timespec start = {0,0}, end = {0,0};

    clock_gettime(CLOCK_REALTIME, &start);
    void *handle = dlopen("libsort_library.so", RTLD_LAZY);
    if (!handle) {
        return 1;
    }
    void (*ins_sort)();
    ins_sort = (void (*)())dlsym(handle, "insertion_sort");
    for (int i = 0; i < 5 * 1000 * 1000; i++) {
        ins_sort(arr, n);
    }
    clock_gettime(CLOCK_REALTIME, &end);

    long start_nanos = start.tv_sec * one_bill + start.tv_nsec;
    long end_nanos = end.tv_sec * one_bill + end.tv_nsec;
    long duration_secs = (end_nanos - start_nanos) / one_bill;
    long duration_nanos = (end_nanos - start_nanos) % one_bill;
    printf("Program duration was %ld secs and %ld nanos\n", duration_secs, duration_nanos);
    return 0;
}