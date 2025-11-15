# ADS Project 3 — Sorting Benchmarks (C++)

This project benchmarks multiple sorting algorithms (both single-threaded and multi-threaded) on large random integer datasets.
It supports dataset generation up to **5 billion entries** and includes external-memory sorting for ultra-large datasets.

---

## 🚀 Building and Running on WSL (Windows 11)

### 1. Generate datasets

```bash
g++ -std=c++17 -O2 -Wall -Wextra generate_array_files.cpp -o generate_array_files
./generate_array_files
```

You can customize the dataset sizes in `generate_array_files.cpp`.

For the 5 billion element dataset:
```bash
g++ -std=c++17 -O2 -Wall -Wextra gen_5e9_stream.cpp -o gen_5e9_stream
./gen_5e9_stream  # Creates ~40GB file
```

### 2. Build the benchmark executable

**Option A (Recommended):**
```bash
make
```

**Option B (Manual):**
```bash
g++ -std=c++17 -O2 -Wall -Wextra -pthread \
    main.cpp insertion_sort.cpp selection_sort.cpp shell_sort.cpp \
    heap_sort.cpp merge_sort.cpp quick_sort.cpp radix_sort.cpp \
    mt_radix_sort.cpp mt_merge_sort.cpp mt_quick_sort.cpp \
    -o sorting.exe
```

### 3. Run algorithm benchmarks

**Syntax:**
```bash
./sorting.exe <elements> <runs> <algorithm_list>
```

**Examples:**
```bash
# Single algorithm
./sorting.exe 5000000 10 quick_sort

# Multiple algorithms (comma-separated, no spaces)
./sorting.exe 50000 10 merge_sort,quick_sort,radix_sort

# Multi-threaded algorithms
./sorting.exe 50000000 10 mt_radix_sort,mt_merge_sort,mt_quick_sort

# All algorithms
./sorting.exe 50000 10 all
```

### 4. External sorting for 5B dataset

For the 5 billion element dataset, use external sorting (disk-based):

```bash
# Convert to uint32 format
g++ -std=c++17 -O2 -Wall -Wextra u64_to_u32_stream.cpp -o u64_to_u32_stream
./u64_to_u32_stream data_5000000000.bin data_5000000000_u32.bin

# Build and run external multi-threaded radix sort
g++ -std=c++17 -O2 -Wall -Wextra -pthread mt_external_radix_sort.cpp -o mt_external_radix_sort
./mt_external_radix_sort data_5000000000_u32.bin sorted_output.bin /tmp
```
---

## ⚙️ Dataset Sizes and System Requirements

| Dataset | Elements | Approx Size | Recommended RAM |
|---------|----------|-------------|-----------------|
| data_5.bin | 5 | Tiny | Any |
| data_50.bin | 50 | < 1 KB | Any |
| data_500.bin | 500 | < 1 KB | Any |
| data_5000.bin | 5 K | < 1 MB | Any |
| data_50000.bin | 50 K | ~0.4 MB | Any |
| data_500000.bin | 500 K | ~4 MB | ≥ 2 GB |
| data_5000000.bin | 5 M | ~40 MB | ≥ 4 GB |
| data_50000000.bin | 50 M | ~400 MB | ≥ 8 GB |
| data_500000000.bin | 500 M | ~4 GB | ≥ 32 GB |
| data_5000000000.bin | 5 B | ~40 GB | External sort |

⚠️ **Note:** Large files (≥ 50M) require SSD storage. The 5B dataset requires external sorting (disk-based) due to RAM constraints.

---

## 📊 Available Algorithms

### Single-Threaded:
- `insertion_sort` - O(n²) comparison sort
- `selection_sort` - O(n²) comparison sort
- `shell_sort` - Optimized insertion sort variant
- `heap_sort` - O(n log n) heap-based sort
- `merge_sort` - O(n log n) divide-and-conquer
- `quick_sort` - O(n log n) average, partition-based
- `radix_sort` - O(nk) non-comparison sort

### Multi-Threaded:
- `mt_radix_sort` - Parallel radix sort
- `mt_merge_sort` - Parallel merge sort
- `mt_quick_sort` - Parallel quick sort

### External (for 5B dataset):
- `mt_external_radix_sort` - Disk-based parallel radix sort

---

## 📁 Project Structure

```
ADS_Project_3/
├── main.cpp                      # Main benchmark program
│
├── Single-threaded algorithms:
├── insertion_sort.cpp
├── selection_sort.cpp
├── shell_sort.cpp
├── heap_sort.cpp
├── merge_sort.cpp
├── quick_sort.cpp
├── radix_sort.cpp
│
├── Multi-threaded algorithms:
├── mt_radix_sort.cpp
├── mt_merge_sort.cpp
├── mt_quick_sort.cpp
├── mt_external_radix_sort.cpp    # External sort for 5B dataset
│
├── Headers:
├── load_dataset.hpp              # Dataset loading utilities
├── bin_to_int.hpp                # Binary conversion helpers
│
├── Dataset generation:
├── generate_array_files.cpp      # Generate standard datasets
├── gen_5e9_stream.cpp            # Generate 5B dataset (streaming)
├── u64_to_u32_stream.cpp         # Convert uint64 to uint32
│
├── Scripts:
├── run_benchmarks.sh             # Automated benchmark script
├── benchmark_5b.sh               # 5B external sort benchmark
│
├── Build & Documentation:
├── Makefile                      # Build automation
├── .gitignore                    # Git ignore rules
├── README.md                     # This file
└── POST_REBOOT_INSTRUCTIONS.md   # WSL setup guide
```

---

## ✅ Verification

- Each algorithm prints average runtime in milliseconds
- Sorting correctness is verified in `main.cpp`
- `.gitignore` prevents large `.bin` files from being committed
- All algorithms tested on datasets from 5 to 500M elements
- External sort tested on 5B dataset

---

## 📝 Notes

- **Performance:** Multi-threaded algorithms show significant speedup on datasets ≥ 500K elements
- **Memory:** In-memory algorithms (non-external) limited by available RAM
- **5B Dataset:** Requires external sorting due to 40GB file size; uses disk-based bucketing

---

**Author:** David Moad (@moaderboat)
**Environment:** Windows 11 + WSL (Ubuntu 22.04)
**Course:** Advanced Data Structures - Project 3

