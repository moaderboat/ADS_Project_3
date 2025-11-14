#include <future>
#include <algorithm>
#include <cmath>
#include <thread>

static void insertion_sort(int* lo, int* hi) {
    for (int* p = lo + 1; p < hi; ++p) {
        int v = *p; int* q = p - 1;
        while (q >= lo && *q > v) { *(q + 1) = *q; --q; }
        *(q + 1) = v;
    }
}

static inline int* median3(int* a, int* b, int* c) {
    if (*a < *b) {
        if (*b < *c) return b;
        return (*a < *c) ? c : a;
    } else {
        if (*a < *c) return a;
        return (*b < *c) ? c : b;
    }
}

static inline int* partition3(int* lo, int* hi) {
    int* mid = lo + (hi - lo) / 2;
    int* pv = median3(lo, mid, hi - 1);
    std::iter_swap(pv, hi - 1);
    int piv = *(hi - 1);
    int* i = lo;
    for (int* j = lo; j < hi - 1; ++j)
        if (*j < piv) std::iter_swap(i++, j);
    std::iter_swap(i, hi - 1);
    return i;
}

static void quick_sort_seq(int* lo, int* hi) {
    while (hi - lo > 32) {
        int* p = partition3(lo, hi);
        if (p - lo < hi - (p + 1)) {
            quick_sort_seq(lo, p);
            lo = p + 1;
        } else {
            quick_sort_seq(p + 1, hi);
            hi = p;
        }
    }
    insertion_sort(lo, hi);
}

static void quick_sort_par(int* lo, int* hi, int depth, int cutoff) {
    const int n = (int)(hi - lo);
    if (n <= cutoff || depth <= 0) {
        quick_sort_seq(lo, hi);
        return;
    }
    int* p = partition3(lo, hi);
    if ((p - lo) < (hi - (p + 1))) {
        auto fut = std::async(std::launch::async, quick_sort_par, lo, p, depth - 1, cutoff);
        quick_sort_par(p + 1, hi, depth - 1, cutoff);
        fut.get();
    } else {
        auto fut = std::async(std::launch::async, quick_sort_par, p + 1, hi, depth - 1, cutoff);
        quick_sort_par(lo, p, depth - 1, cutoff);
        fut.get();
    }
}

void mt_quick_sort(int* A, int n) {
    if (n <= 1) return;
    const unsigned cores = std::max(1u, std::thread::hardware_concurrency());
    const int depth = std::max(0, (int)std::log2(cores));
    const int cutoff = 1 << 14; // ~16K; tune as needed
    quick_sort_par(A, A + n, depth, cutoff);
}
