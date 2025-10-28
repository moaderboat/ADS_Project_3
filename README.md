# ADS Project 3 — Sorting Benchmarks (C++)

This project benchmarks multiple sorting algorithms on large random integer datasets.
It supports dataset generation up to **5 billion entries** and external-memory radix sorting.

---

## 🚀 Building and Running on WSL (Windows 11)

### 1. Generate datasets

g++ -std=c++17 -O2 -Wall -Wextra generate_array_files.cpp -o generate_array_files
./generate_array_files

You can customize the dataset sizes in generate_array_files.cpp.

### 2. Build the benchmark executable
## A:
Copy code
# make
or manually:

## B:
Copy code
# g++ -std=c++17 -O2 -Wall -Wextra main.cpp insertion_sort.cpp selection_sort.cpp shell_sort.cpp heap_sort.cpp merge_sort.cpp quick_sort.cpp radix_sort.cpp external_radix32.cpp -o sorting_all


#### 3. Run a single algorithm benchmark
Example: sort 5 million elements 10 times

Copy code

# ./sorting_all 5000000 10
⚙️ Dataset Sizes and System Requirements
Dataset	Elements	Approx Size	Recommended RAM
data_5.bin	5	Tiny	Any
data_5000000.bin	5 M	40 MB	≥ 4 GB
data_500000000.bin	500 M	4 GB	≥ 32 GB
data_5000000000.bin	5 B	40 GB	≥ 256 GB

⚠️ Large files (≥ 50 M) require SSD storage and significant RAM.

📁 Project Structure
css
Copy code
ADS_Project_3/
├── main.cpp
├── insertion_sort.cpp
├── selection_sort.cpp
├── shell_sort.cpp
├── heap_sort.cpp
├── merge_sort.cpp
├── quick_sort.cpp
├── radix_sort.cpp
├── external_radix32.cpp
│
├── load_dataset.hpp
├── bin_to_int.hpp
│
├── generate_array_files.cpp
├── gen_5e9_stream.cpp
├── u64_to_u32_stream.cpp
│
├── Makefile
├── .gitignore
└── README.md
✅ Verification
Each algorithm runs individually and prints average runtime (ms).

Sorting correctness is verified inside main.cpp.

.gitignore prevents large .bin files from being uploaded.

Author: David Moad (@moaderboat)
Environment: Windows 11 + WSL (Ubuntu 22.04)

