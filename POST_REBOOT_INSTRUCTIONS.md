# Post-Reboot Instructions for MT Radix Sort Benchmarking

## Step 1: Complete WSL Setup
After rebooting, open PowerShell or Command Prompt and run:
```bash
wsl
```
Follow the prompts to create a Ubuntu username and password.

## Step 2: Install Build Tools in WSL
Once in WSL Ubuntu, run:
```bash
sudo apt update
sudo apt install build-essential
```

## Step 3: Navigate to Project Directory
```bash
cd /mnt/c/Users/david/OneDrive/Documents/GitHub/ADS_Project_3
```

## Step 4: Compile the Project
```bash
make clean
make
```

## Step 5: Check for Required Dataset Files
The benchmark needs dataset files. Check what exists:
```bash
ls -lh data_*.bin
```

If datasets are missing, generate them:
```bash
g++ -std=c++17 -O2 -Wall -Wextra generate_array_files.cpp -o generate_array_files
./generate_array_files
```

## Step 6: Run MT Radix Sort Benchmarks

Run benchmarks for different array sizes (10 runs each):
```bash
# Array size 5
./sorting.exe 5 10 mt_radix_sort

# Array size 50
./sorting.exe 50 10 mt_radix_sort

# Array size 500
./sorting.exe 500 10 mt_radix_sort

# Array size 5000
./sorting.exe 5000 10 mt_radix_sort

# Array size 50000
./sorting.exe 50000 10 mt_radix_sort

# Array size 500000
./sorting.exe 500000 10 mt_radix_sort

# Array size 5000000
./sorting.exe 5000000 10 mt_radix_sort

# Array size 50000000
./sorting.exe 50000000 10 mt_radix_sort

# Array size 500000000
./sorting.exe 500000000 10 mt_radix_sort
```

## Step 7: Record Results
Note the average times for each array size and add them to `run_times_book.xlsx`.

## Changes Already Made
The following files have been modified and are ready to use:
- `main.cpp` - Added mt_radix_sort, mt_merge_sort, mt_quick_sort declarations and mappings
- `Makefile` - Added multi-threaded source files and -pthread flag

These changes are currently uncommitted. You may want to commit them before rebooting.
