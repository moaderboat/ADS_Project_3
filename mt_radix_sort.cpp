#include <vector>
#include <thread>
#include <future>
#include <algorithm>

void mt_radix_sort(int* A, int n) {
    if (n <= 1) return;

    const unsigned T = std::max(1u, std::thread::hardware_concurrency());
    const int RAD = 256;
    const int PASSES = 4; // 32-bit ints -> 4 bytes

    std::vector<int> B(n);
    int* src = A;
    int* dst = B.data();

    auto chunk = [&](unsigned t) {
        int s = (int)((1ull * n * t) / T);
        int e = (int)((1ull * n * (t + 1)) / T);
        return std::pair<int, int>{s, e};
    };

    // Per-thread histograms
    std::vector<std::vector<int>> hist(T, std::vector<int>(RAD));

    for (int pass = 0; pass < PASSES; ++pass) {
        int shift = pass * 8;

        // Zero local hists
        for (unsigned t = 0; t < T; ++t)
            std::fill(hist[t].begin(), hist[t].end(), 0);

        // Build local histograms in parallel
        std::vector<std::future<void>> futs;
        futs.reserve(T);
        for (unsigned t = 0; t < T; ++t) {
            futs.emplace_back(std::async(std::launch::async, [&, t] {
                auto [s, e] = chunk(t);
                for (int i = s; i < e; ++i) {
                    unsigned byte = (unsigned)((src[i] >> shift) & 0xFF);
                    hist[t][byte]++; // thread-local -> no races
                }
            }));
        }
        for (auto& f : futs) f.get();
        futs.clear();

        // Global histogram
        std::vector<int> global(RAD);
        for (int b = 0; b < RAD; ++b) {
            int sum = 0;
            for (unsigned t = 0; t < T; ++t) sum += hist[t][b];
            global[b] = sum;
        }

        // Global prefix
        std::vector<int> prefix(RAD);
        int run = 0;
        for (int b = 0; b < RAD; ++b) {
            prefix[b] = run;
            run += global[b];
        }

        // Thread-local starting offsets for scatter
        std::vector<std::vector<int>> local_off(T, std::vector<int>(RAD));
        for (int b = 0; b < RAD; ++b) {
            int base = prefix[b];
            for (unsigned t = 0; t < T; ++t) {
                local_off[t][b] = base;
                base += hist[t][b];
            }
        }

        // Scatter in parallel (no write races: each thread advances its own local_off)
        for (unsigned t = 0; t < T; ++t) {
            futs.emplace_back(std::async(std::launch::async, [&, t] {
                auto [s, e] = chunk(t);
                auto& off = local_off[t];
                for (int i = s; i < e; ++i) {
                    unsigned byte = (unsigned)((src[i] >> shift) & 0xFF);
                    dst[off[byte]++] = src[i];
                }
            }));
        }
        for (auto& f : futs) f.get();

        std::swap(src, dst);
    }

    // If odd number of swaps left data in temp buffer, copy back
    if (src != A) std::copy(src, src + n, A);
}
