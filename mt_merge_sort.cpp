#include <vector>
#include <future>
#include <algorithm>
#include <cmath>
#include <thread>

static inline void merge_ranges(int* A, int* B, int lo, int mid, int hi) {
    int i = lo, j = mid, k = lo;
    while (i < mid && j < hi)
        B[k++] = (A[i] <= A[j] ? A[i++] : A[j++]);
    while (i < mid) B[k++] = A[i++];
    while (j < hi)  B[k++] = A[j++];
    std::copy(B + lo, B + hi, A + lo);
}

static void merge_sort_seq(int* A, int* B, int lo, int hi) {
    if (hi - lo <= 32) {
        for (int i = lo + 1; i < hi; ++i) {
            int v = A[i], p = i - 1;
            while (p >= lo && A[p] > v) { A[p + 1] = A[p]; --p; }
            A[p + 1] = v;
        }
        return;
    }
    int mid = lo + (hi - lo) / 2;
    merge_sort_seq(A, B, lo, mid);
    merge_sort_seq(A, B, mid, hi);
    if (A[mid - 1] <= A[mid]) return;
    merge_ranges(A, B, lo, mid, hi);
}

static void merge_sort_par(int* A, int* B, int lo, int hi, int depth, int cutoff) {
    const int n = hi - lo;
    if (n <= cutoff || depth <= 0) {
        merge_sort_seq(A, B, lo, hi);
        return;
    }
    int mid = lo + n / 2;
    auto fut = std::async(std::launch::async, merge_sort_par, A, B, lo, mid, depth - 1, cutoff);
    merge_sort_par(A, B, mid, hi, depth - 1, cutoff);
    fut.get();
    if (A[mid - 1] <= A[mid]) return;
    merge_ranges(A, B, lo, mid, hi);
}

void mt_merge_sort(int* A, int n) {
    if (n <= 1) return;
    std::vector<int> tmp(n);
    const unsigned cores = std::max(1u, std::thread::hardware_concurrency());
    const int depth = std::max(0, (int)std::log2(cores));
    const int cutoff = 1 << 14; // ~16K; tune as needed
    merge_sort_par(A, tmp.data(), 0, n, depth, cutoff);
}
