# ADS_Project_3
project 3 of advanced data structures.


Sorting Benchmarks (C++)

This project benchmarks multiple sorting algorithms on large integer datasets.
Each dataset is generated as a .bin file containing random 64-bit integers.

1) Build the dataset files (5 → 5 billion elements)

If you haven’t already, compile and run the dataset generator:

g++ -std=c++17 -O2 -Wall -Wextra generate_datasets.cpp -o generate_datasets.exe
.\generate_datasets.exe


Inside generate_datasets.cpp, you can define any sizes you want—from very small to very large.
For example:

vector<unsigned long long> sizes = {
    5, 50, 500, 5000, 50000,
    500000, 5000000, 50000000,
    500000000, 5000000000ULL
};


This will create dataset files named:

data_5.bin
data_50.bin
data_500.bin
data_5000.bin
data_50000.bin
data_500000.bin
data_5000000.bin
data_50000000.bin
data_500000000.bin
data_5000000000.bin


⚠️ Warning:
Sizes above ~50 million elements require hundreds of gigabytes of RAM.
For example:

data_50000000.bin (50 million) ≈ 400 MB

data_500000000.bin (500 million) ≈ 4 GB

data_5000000000.bin (5 billion) ≈ 40 GB

Generate only as large as your hardware allows.
The generator automatically skips sizes that exceed available memory.

2) Compile the benchmark executable

(unchanged from before)

g++ -std=c++17 -O2 -Wall -Wextra main.cpp `
    insertion_sort.cpp selection_sort.cpp shell_sort.cpp `
    heap_sort.cpp merge_sort.cpp quick_sort.cpp radix_sort.cpp `
    -o sorting_all.exe

3) Run the benchmarks
sorting_all <n> <runs>


Examples:

.\sorting_all.exe 5000000 10
.\sorting_all.exe 500000000 2
.\sorting_all.exe 543 100


If an exact dataset file (data_<n>.bin) does not exist, the program automatically finds the next largest available file and loads only the first <n> values.

For large n (> 50 000), the O(n²) sorts are skipped automatically.

Default case

If you run it without arguments:

sorting_all


You’ll see:

Usage: sorting_all <n> <runs>
Example: sorting_all 5000 200

⚙️ System requirements for large arrays
Dataset size	Approx. file size	Recommended RAM	Notes
5 000 000	40 MB	≥ 4 GB	Safe on modern desktops
500 000 000	4 GB	≥ 32 GB	May need swap space
5 000 000 000	40 GB	≥ 256 GB	Requires HPC or server hardware

If your system runs out of memory, reduce the sizes in generate_datasets.cpp.
