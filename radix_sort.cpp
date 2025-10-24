#include <iostream>
using namespace std;

// Counting sort by digit (exp = 1, 10, 100, ...)
static void counting_sort_by_exp(int A[], int n, int exp) {
    int* output = new int[n];
    int count[10] = {0};

    for (int i = 0; i < n; ++i)
        ++count[(A[i] / exp) % 10];

    for (int i = 1; i < 10; ++i)
        count[i] += count[i - 1];

    for (int i = n - 1; i >= 0; --i) {
        int digit = (A[i] / exp) % 10;
        output[count[digit] - 1] = A[i];
        --count[digit];
    }

    for (int i = 0; i < n; ++i)
        A[i] = output[i];

    delete[] output;
}

void radix_sort(int A[], int n) {
    if (n <= 1) return;

    // assumes non-negative values
    int maxv = A[0];
    for (int i = 1; i < n; ++i)
        if (A[i] > maxv) maxv = A[i];

    for (int exp = 1; maxv / exp > 0; exp *= 10)
        counting_sort_by_exp(A, n, exp);
}
