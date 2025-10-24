// load_dataset.hpp
#pragma once
#include <vector>
#include <string>
#include <fstream>
#include <stdexcept>

inline void load_first_n_u64(const std::string& path, size_t n, std::vector<uint64_t>& out) {
    out.clear();
    out.resize(n);

    std::ifstream fin(path, std::ios::binary);
    if (!fin) throw std::runtime_error("Failed to open: " + path);

    fin.read(reinterpret_cast<char*>(out.data()), static_cast<std::streamsize>(n * sizeof(uint64_t)));
    if (!fin) throw std::runtime_error("File read error (need at least n*8 bytes): " + path);
}
