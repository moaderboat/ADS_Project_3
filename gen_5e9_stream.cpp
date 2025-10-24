#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <cstdint>

int main(int argc, char* argv[]) {
    using u64 = std::uint64_t;

    // Params (override with args: <total_count> <chunk_count> <seed>)
    u64 TOTAL = 5000000000ULL;                 // 5,000,000,000 values
    u64 CHUNK = 10'000'000ULL;                 // write 10M values per batch (~80 MB)
    unsigned long long SEED = 0;               // 0 => random_device seed

    if (argc >= 2) TOTAL = std::strtoull(argv[1], nullptr, 10);
    if (argc >= 3) CHUNK = std::strtoull(argv[2], nullptr, 10);
    if (argc >= 4) SEED  = std::strtoull(argv[3], nullptr, 10);

    if (CHUNK == 0 || TOTAL == 0) {
        std::cerr << "Invalid TOTAL or CHUNK.\n";
        return 1;
    }

    std::cout << "Generating " << TOTAL << " uint64_t values in chunks of "
              << CHUNK << "...\n";

    // RNG
    std::mt19937_64 gen( SEED ? SEED : std::random_device{}() );
    std::uniform_int_distribution<u64> dist(0, 2*TOTAL);

    // Open output (binary)
    std::string filename = "data_" + std::to_string(TOTAL) + ".bin";
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        std::cerr << "Failed to open " << filename << " for writing.\n";
        return 1;
    }

    // Single reusable buffer
    std::vector<u64> buf;
    buf.resize(static_cast<size_t>(CHUNK));

    u64 written = 0;
    while (written < TOTAL) {
        u64 batch = std::min(CHUNK, TOTAL - written);
        for (u64 i = 0; i < batch; ++i) buf[static_cast<size_t>(i)] = dist(gen);
        out.write(reinterpret_cast<const char*>(buf.data()),
                  static_cast<std::streamsize>(batch * sizeof(u64)));
        if (!out) {
            std::cerr << "Write error after " << written << " values.\n";
            return 1;
        }
        written += batch;

        // lightweight progress
        if (written % (100ULL * 1000 * 1000) == 0 || written == TOTAL) {
            std::cout << "  " << written << " / " << TOTAL << " written\r" << std::flush;
        }
    }

    out.close();
    std::cout << "\nDone. Wrote " << filename << " (~"
              << (TOTAL * sizeof(u64) / 1e9) << " GB).\n";
    return 0;
}
