
# ==== Compiler & Flags ====
CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra

# ==== Sources ====
SRCS := main.cpp \
        insertion_sort.cpp selection_sort.cpp shell_sort.cpp \
        heap_sort.cpp merge_sort.cpp quick_sort.cpp radix_sort.cpp

TARGET := sorting.exe

# ==== Fixed runs ====
RUNS := 10           # << always 100, by design

# ==== You choose these per-invocation ====
# Example: make run N=50000 ALG=quick_sort
N   ?= 50000
ALG ?= quick_sort     # use 'all' or comma-list (e.g., shell_sort,merge_sort)

# ---- default ----
all: $(TARGET)

$(TARGET): $(SRCS) bin_to_int.hpp load_dataset.hpp
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@

# ---- run exactly one block with your chosen size & algo(s) ----
run: $(TARGET)
	@echo "Running: N=$(N)  RUNS=$(RUNS)  ALG=$(ALG)"
	./$(TARGET) $(N) $(RUNS) $(ALG)

# ---- convenience: run 'all' algos for chosen N ----
run-all: $(TARGET)
	@echo "Running: N=$(N)  RUNS=$(RUNS)  ALG=all"
	./$(TARGET) $(N) $(RUNS) all

clean:
	$(RM) $(TARGET)

help:
	@echo "make                - build sorting.exe"
	@echo "make run N=<n> ALG=<name|list|all>"
	@echo "   examples:"
	@echo "     make run N=50000 ALG=quick_sort"
	@echo "     make run N=50000 ALG=shell_sort,merge_sort"
	@echo "     make run N=50000 ALG=all"
	@echo "make run-all N=<n>  - runs all algos for that size (100 runs)"
	@echo "make clean          - remove sorting.exe"

.PHONY: all run run-all clean help
