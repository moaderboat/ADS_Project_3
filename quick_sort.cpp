#include <iostream>
using namespace std;

static int partition_array(int A[], int low, int high) {
    int pivot = A[high];
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (A[j] <= pivot) {
            ++i;
            int temp = A[i]; A[i] = A[j]; A[j] = temp;   // '=' not '-'
        }
    }
    int temp = A[i + 1]; A[i + 1] = A[high]; A[high] = temp;
    return i + 1;
}

static void quick_sort_rec(int A[], int low, int high) {
    if (low < high) {
        int pi = partition_array(A, low, high);
        quick_sort_rec(A, low, pi - 1);
        quick_sort_rec(A, pi + 1, high);
    }
}

void quick_sort(int A[], int n) {
    quick_sort_rec(A, 0, n - 1);
}
