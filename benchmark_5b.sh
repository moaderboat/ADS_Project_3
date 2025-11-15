#!/bin/bash
echo "Running mt_external_radix_sort benchmark (10 runs)..."
total_time=0

for i in {1..10}; do
    echo "Run $i/10..."
    start=$(date +%s%3N)
    ./mt_external_radix_sort data_5000000000_u32.bin sorted_5b.bin /tmp > /dev/null 2>&1
    end=$(date +%s%3N)
    elapsed=$((end - start))
    echo "  Time: ${elapsed} ms"
    total_time=$((total_time + elapsed))
    rm -f sorted_5b.bin  # Clean up between runs
done

avg=$((total_time / 10))
echo ""
echo "Average time: ${avg} ms"
