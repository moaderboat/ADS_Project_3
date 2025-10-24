#include <iostream>
using namespace std;

void selection_sort(int A[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_index = i;
        for (int j = i + 1; j < n; j++) {
            if (A[j] < A[min_index]) {
                min_index = j;
            }
        }
        int temp = A[i];
        A[i] = A[min_index];
        A[min_index] = temp;
    }
}
