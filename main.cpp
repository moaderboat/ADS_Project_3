#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <cstdlib>      // atoll, atoi
#include <filesystem>   // std::filesystem::exists
using namespace std;

// --- Sorting function declarations ---
void insertion_sort(int A[], int n);
void selection_sort(int A[], int n);
void shell_sort(int A[], int n);
void heap_sort(int A[], int n);
void merge_sort(int A[], int n);
void quick_sort(int A[], int n);
void radix_sort(int A[], int n);

// --- Helpers ---
#include "load_dataset.hpp"
#include "bin_to_int.hpp"

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Args: <n> <runs>
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <n> <runs>\n"
             << "Example: " << argv[0] << " 543 100\n";
        return 1;
    }

    const size_t n   = static_cast<size_t>(atoll(argv[1]));
    const int    runs = atoi(argv[2]);
    if (n == 0 || runs <= 0) {
        cerr << "Error: <n> must be > 0 and <runs> >= 1.\n";
        return 1;
    }

    // --- Try to find an exact dataset; otherwise use next largest available ---
    const vector<size_t> available = {5, 50, 500, 5000, 50000, 500000, 5000000};
    string dataset = "data_" + to_string(n) + ".bin";
    string chosen  = dataset;

    if (!filesystem::exists(dataset)) {
        for (size_t size : available) {
            if (size >= n && filesystem::exists("data_" + to_string(size) + ".bin")) {
                chosen = "data_" + to_string(size) + ".bin";
                cout << "!!!  Exact file not found. Using " << chosen
                     << " and trimming to first " << n << " elements.\n";
                break;
            }
        }
    }

    cout << "\n--- Sorting Demo using dataset: " << chosen
         << " (" << n << " elements, " << runs << " runs each) ---\n";

    // --- Load dataset ---
    vector<uint64_t> raw;
    try {
        load_first_n_u64(chosen, n, raw);
    } catch (const exception& e) {
        cerr << "!!! Error loading dataset: " << e.what() << "\n";
        return 1;
    }

    // --- Convert to int array ---
    vector<int> baseA;
    bin_to_int_array(raw, baseA);

    // --- Run & time each algorithm ---
    auto run_algo = [&](auto sort_func, const string& name) {
        double total_ms = 0.0;
        bool ok_first = true;

        for (int r = 0; r < runs; ++r) {
            vector<int> A = baseA; // fresh copy
            auto t0 = chrono::high_resolution_clock::now();
            sort_func(A.data(), static_cast<int>(A.size()));
            auto t1 = chrono::high_resolution_clock::now();
            total_ms += chrono::duration<double, milli>(t1 - t0).count();

            if (r == 0) {
                for (size_t i = 1; i < A.size(); ++i)
                    if (A[i - 1] > A[i]) { ok_first = false; break; }
            }
        }

        cout << name << " -> avg time: " << (total_ms / runs)
             << " ms (" << runs << " runs), sorted? "
             << (ok_first ? "yes" : "NO") << "\n";
    };

    const bool big = (n > 50000);
    if (!big) run_algo(insertion_sort, "insertion_sort");
    if (!big) run_algo(selection_sort, "selection_sort");

    run_algo(shell_sort,  "shell_sort");
    run_algo(heap_sort,   "heap_sort");
    run_algo(merge_sort,  "merge_sort");
    run_algo(quick_sort,  "quick_sort");
    run_algo(radix_sort,  "radix_sort");

    cout << "\nAll sorting tests completed.\n";
    return 0;
}
