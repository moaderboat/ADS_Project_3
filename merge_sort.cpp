#include <iostream>
using namespace std;

static void merge_sections(int A[], int aux[], int left, int mid, int right) {
    for (int k = left; k <= right; ++k) aux[k] = A[k];

    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (aux[i] <= aux[j]) A[k++] = aux[i++];
        else                  A[k++] = aux[j++];
    }
    while (i <= mid) A[k++] = aux[i++];    // right half leftovers already placed
}

static void merge_sort_rec(int A[], int aux[], int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    merge_sort_rec(A, aux, left, mid);
    merge_sort_rec(A, aux, mid + 1, right);
    if (A[mid] <= A[mid + 1]) return;      // small optimization
    merge_sections(A, aux, left, mid, right);
}

void merge_sort(int A[], int n) {
    if (n <= 1) return;
    int* aux = new int[n];
    merge_sort_rec(A, aux, 0, n - 1);
    delete[] aux;
}
