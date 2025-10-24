#include <iostream>
#include <vector>
#include <chrono>
#include <string>
#include <cstdlib>      // atoll, atoi
#include <filesystem>   // std::filesystem::exists
#include <unordered_map>
#include <functional>
#include <algorithm>

using namespace std;

// --- Sorting function declarations (unchanged) ---
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

static vector<string> split_csv_lower(const string& s) {
    vector<string> out;
    string cur;
    for (char c : s) {
        if (c == ',') {
            if (!cur.empty()) {
                transform(cur.begin(), cur.end(), cur.begin(), ::tolower);
                out.push_back(cur);
                cur.clear();
            }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) {
        transform(cur.begin(), cur.end(), cur.begin(), ::tolower);
        out.push_back(cur);
    }
    return out;
}

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Args: <n> <runs> [algos_csv|all]
    if (argc < 3 || argc > 4) {
        cerr << "Usage: " << argv[0] << " <n> <runs> [algos]\n"
             << "  <n>    : number of elements (>0)\n"
             << "  <runs> : repetitions per algorithm (>=1)\n"
             << "  [algos]: comma-separated list or 'all' (default: all)\n"
             << "           names: insertion_sort, selection_sort, shell_sort,\n"
             << "                  heap_sort, merge_sort, quick_sort, radix_sort\n"
             << "Examples:\n"
             << "  " << argv[0] << " 50000 5\n"
             << "  " << argv[0] << " 50000 5 quick_sort\n"
             << "  " << argv[0] << " 50000 5 shell_sort,merge_sort\n";
        return 1;
    }

    const size_t n   = static_cast<size_t>(atoll(argv[1]));
    const int    runs = atoi(argv[2]);
    if (n == 0 || runs <= 0) {
        cerr << "Error: <n> must be > 0 and <runs> >= 1.\n";
        return 1;
    }

    const string algos_arg = (argc == 4) ? argv[3] : "all";

    // --- Try to find an exact dataset; otherwise use next largest available ---
    const vector<size_t> available = {
        5ULL, 50ULL, 500ULL, 5000ULL, 50000ULL, 500000ULL, 5000000ULL,
        50000000ULL, 500000000ULL, 5000000000ULL
    };
    string dataset = "data_" + to_string(n) + ".bin";
    string chosen  = dataset;

    if (!filesystem::exists(dataset)) {
        for (size_t size : available) {
            string candidate = "data_" + to_string(size) + ".bin";
            if (size >= n && filesystem::exists(candidate)) {
                chosen = candidate;
                cout << "!!! Exact file not found. Using " << chosen
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

    // Map from canonical name -> function
    const unordered_map<string, function<void(int*,int)>> algo_map = {
        {"insertion_sort", insertion_sort},
        {"selection_sort", selection_sort},
        {"shell_sort",     shell_sort},
        {"heap_sort",      heap_sort},
        {"merge_sort",     merge_sort},
        {"quick_sort",     quick_sort},
        {"radix_sort",     radix_sort},
    };

    const bool big = (n > 50000);

    // Resolve selection
    vector<string> to_run;
    if (algos_arg == "all" || algos_arg == "ALL") {
        to_run = {"shell_sort","heap_sort","merge_sort","quick_sort","radix_sort"};
        // Keep your original behavior: only include quadratic on small n
        if (!big) {
            to_run.insert(to_run.begin(), {"insertion_sort","selection_sort"});
        }
    } else {
        to_run = split_csv_lower(algos_arg);
        // Validate names
        vector<string> validated;
        for (auto& name : to_run) {
            if (!algo_map.count(name)) {
                cerr << "!!! Unknown algorithm name: " << name << " (skipping)\n";
                continue;
            }
            if (big && (name == "insertion_sort" || name == "selection_sort")) {
                cerr << "!!! Warning: running quadratic algo '" << name
                     << "' on large n=" << n << ". This may be slow.\n";
            }
            validated.push_back(name);
        }
        to_run.swap(validated);
        if (to_run.empty()) {
            cerr << "No valid algorithms selected. Exiting.\n";
            return 1;
        }
    }

    // Timing runner
    auto run_algo = [&](const function<void(int*,int)>& sort_func, const string& name)
        -> pair<double,bool>
    {
        double total_ms = 0.0;
        bool ok_first = true;

        for (int r = 0; r < runs; ++r) {
            vector<int> A = baseA; // fresh copy
            auto t0 = chrono::high_resolution_clock::now();
            sort_func(A.data(), static_cast<int>(A.size()));
            auto t1 = chrono::high_resolution_clock::now();
            total_ms += chrono::duration<double, milli>(t1 - t0).count();

            if (r == 0) {
                for (size_t i = 1; i < A.size(); ++i) {
                    if (A[i - 1] > A[i]) { ok_first = false; break; }
                }
            }
        }

        cout << name << " -> avg: " << (total_ms / runs)
             << " ms  (" << runs << " runs),  sorted? "
             << (ok_first ? "yes" : "NO") << "\n";
        return {total_ms, ok_first};
    };

    // Execute selected block (one-by-one, sequentially)
    double block_total_ms = 0.0;
    int    bad_sorts = 0;
    for (const auto& name : to_run) {
        const auto& f = algo_map.at(name);
        auto [sum_ms, ok] = run_algo(f, name);
        block_total_ms += sum_ms;
        if (!ok) ++bad_sorts;
    }

    cout << "----\nBlock total time: " << block_total_ms << " ms\n";
    if (bad_sorts) {
        cout << "WARNING: " << bad_sorts << " algorithm(s) failed the sorted check on first run.\n";
    }
    cout << "All selected algorithms completed.\n";

    return 0;
}
