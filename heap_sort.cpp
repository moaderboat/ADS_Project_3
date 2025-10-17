#include <iostream>
using namespace std;

static void sift_down(int A[], int n, int i) {
    while (true) {
        int largest = i;
        int l = 2 * i + 1;              // left child
        int r = 2 * i + 2;              // right child
        if (l < n && A[l] > A[largest]) largest = l;
        if (r < n && A[r] > A[largest]) largest = r;
        if (largest == i) break;
        int tmp = A[i]; A[i] = A[largest]; A[largest] = tmp;
        i = largest;
    }
}

void heap_sort(int A[], int n) {
    for (int i = n / 2 - 1; i >= 0; --i) sift_down(A, n, i);  // build heap
    for (int end = n - 1; end > 0; --end) {                   // extract max
        int tmp = A[0]; A[0] = A[end]; A[end] = tmp;
        sift_down(A, end, 0);
    }
}
