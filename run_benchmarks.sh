#!/bin/bash
# Automated benchmark script for mt_radix_sort
# Run this after WSL is set up and project is compiled

echo "Starting mt_radix_sort benchmarks..."
echo "======================================"

sizes=(5 50 500 5000 50000 500000 5000000 50000000 500000000)

for size in "${sizes[@]}"; do
    echo ""
    echo "Testing array size: $size"
    ./sorting.exe $size 10 mt_radix_sort
    echo "--------------------------------------"
done

echo ""
echo "All benchmarks completed!"
echo "Please record the average times in run_times_book.xlsx"
