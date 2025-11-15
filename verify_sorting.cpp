#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <string>

// Algorithm declarations
void insertion_sort(int A[], int n);
void quick_sort(int A[], int n);
void merge_sort(int A[], int n);
void radix_sort(int A[], int n);
void mt_radix_sort(int A[], int n);
void mt_merge_sort(int A[], int n);
void mt_quick_sort(int A[], int n);

#include "load_dataset.hpp"
#include "bin_to_int.hpp"

void print_array(const char* algo_name, int* arr, int n, int count = 50) {
    std::cout << "\n" << algo_name << " (first " << count << " elements):\n";
    for (int i = 0; i < std::min(n, count); i++) {
        std::cout << arr[i];
        if (i < std::min(n, count) - 1) std::cout << ", ";
        if ((i + 1) % 10 == 0) std::cout << "\n";
    }
    std::cout << "\n";
}

bool is_sorted(int* arr, int n) {
    for (int i = 1; i < n; i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

int main(int argc, char* argv[]) {
    int n = (argc > 1) ? std::stoi(argv[1]) : 500;
    int show_count = (argc > 2) ? std::stoi(argv[2]) : 50;

    std::cout << "Loading dataset with " << n << " elements...\n";

    // Find dataset file
    std::string dataset = "data_" + std::to_string(n) + ".bin";

    // Load original dataset
    std::vector<uint64_t> raw;
    try {
        load_first_n_u64(dataset, n, raw);
    } catch (const std::exception& e) {
        std::cerr << "Failed to load dataset: " << e.what() << "\n";
        return 1;
    }

    // Convert to int array
    std::vector<int> original;
    bin_to_int_array(raw, original);

    std::cout << "Original data (first 50):\n";
    for (int i = 0; i < std::min(n, 50); i++) {
        std::cout << original[i];
        if (i < std::min(n, 50) - 1) std::cout << ", ";
        if ((i + 1) % 10 == 0) std::cout << "\n";
    }
    std::cout << "\n";
    std::cout << "========================================\n";

    // Test insertion_sort
    {
        std::vector<int> arr = original;
        insertion_sort(arr.data(), n);
        print_array("Insertion Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    // Test quick_sort
    {
        std::vector<int> arr = original;
        quick_sort(arr.data(), n);
        print_array("Quick Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    // Test merge_sort
    {
        std::vector<int> arr = original;
        merge_sort(arr.data(), n);
        print_array("Merge Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    // Test radix_sort
    {
        std::vector<int> arr = original;
        radix_sort(arr.data(), n);
        print_array("Radix Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    // Test mt_quick_sort
    {
        std::vector<int> arr = original;
        mt_quick_sort(arr.data(), n);
        print_array("MT Quick Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    // Test mt_merge_sort
    {
        std::vector<int> arr = original;
        mt_merge_sort(arr.data(), n);
        print_array("MT Merge Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    // Test mt_radix_sort
    {
        std::vector<int> arr = original;
        mt_radix_sort(arr.data(), n);
        print_array("MT Radix Sort", arr.data(), n, show_count);
        std::cout << "Sorted correctly? " << (is_sorted(arr.data(), n) ? "YES" : "NO") << "\n";
    }

    return 0;
}
