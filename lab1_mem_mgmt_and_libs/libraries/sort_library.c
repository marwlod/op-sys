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