// bin_to_int.hpp
#pragma once
#include <vector>
#include <climits>

inline void bin_to_int_array(const std::vector<uint64_t>& src, std::vector<int>& dst) {
    dst.resize(src.size());
    for (size_t i = 0; i < src.size(); ++i) {
        dst[i] = static_cast<int>(src[i] % (static_cast<uint64_t>(INT_MAX)));
    }
}
