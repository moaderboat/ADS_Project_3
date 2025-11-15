#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <atomic>
using namespace std;

// Tunables
static constexpr size_t READ_CHUNK = 10'000'000;  // 10M * 4B = 40MB per thread
static constexpr size_t BUCKET_BUF = 64 * 1024;   // 64KB staging per bucket per thread

// Thread-safe bucket writers
class BucketWriter {
    array<ofstream, 256> files;
    array<mutex, 256> locks;

public:
    void open(const string& dir, int pass) {
        for(int b = 0; b < 256; b++) {
            ostringstream name;
            name << dir << "/pass" << pass << "_b" << setw(3) << setfill('0') << b << ".bin";
            files[b].open(name.str(), ios::binary | ios::trunc);
            if(!files[b]) throw runtime_error("Failed opening: " + name.str());
        }
    }

    void write(int bucket, const vector<uint32_t>& data) {
        lock_guard<mutex> lock(locks[bucket]);
        files[bucket].write(reinterpret_cast<const char*>(data.data()),
                           static_cast<streamsize>(data.size() * sizeof(uint32_t)));
    }

    void close() {
        for(auto& f : files) f.close();
    }
};

// Concatenate buckets in order
static void concat_buckets(const string& dir, int pass, const string& out) {
    ofstream dst(out, ios::binary | ios::trunc);
    if(!dst) throw runtime_error("Failed opening: " + out);
    vector<char> buf(4*1024*1024); // 4MB buffer

    for(int b = 0; b < 256; b++) {
        ostringstream name;
        name << dir << "/pass" << pass << "_b" << setw(3) << setfill('0') << b << ".bin";
        ifstream src(name.str(), ios::binary);
        if(!src) throw runtime_error("Missing bucket: " + name.str());

        while(src) {
            src.read(buf.data(), buf.size());
            streamsize got = src.gcount();
            if(got > 0) dst.write(buf.data(), got);
        }
        src.close();
        remove(name.str().c_str());
    }
    dst.close();
}

// Worker thread function
static void worker_thread(
    const string& inpath,
    BucketWriter& writer,
    int pass,
    atomic<uint64_t>& offset,
    atomic<uint64_t>& total_processed,
    uint64_t file_size
) {
    // Per-thread staging buffers
    vector<vector<uint32_t>> bucketStage(256);
    for(auto& v : bucketStage) v.reserve(BUCKET_BUF / sizeof(uint32_t));

    auto flushBucket = [&](int b) {
        if(!bucketStage[b].empty()) {
            writer.write(b, bucketStage[b]);
            bucketStage[b].clear();
        }
    };

    vector<uint32_t> buf(READ_CHUNK);

    while(true) {
        // Atomically claim next chunk
        uint64_t my_offset = offset.fetch_add(READ_CHUNK * sizeof(uint32_t));
        if(my_offset >= file_size) break;

        // Read chunk
        ifstream in(inpath, ios::binary);
        in.seekg(my_offset);
        size_t to_read = min(READ_CHUNK, (file_size - my_offset) / sizeof(uint32_t));
        in.read(reinterpret_cast<char*>(buf.data()),
                static_cast<streamsize>(to_read * sizeof(uint32_t)));
        size_t got = static_cast<size_t>(in.gcount() / sizeof(uint32_t));
        in.close();

        if(got == 0) break;

        // Distribute to buckets
        for(size_t i = 0; i < got; i++) {
            uint32_t x = buf[i];
            int b = (x >> (8 * pass)) & 0xFF;
            bucketStage[b].push_back(x);
            if(bucketStage[b].size() * sizeof(uint32_t) >= BUCKET_BUF) {
                flushBucket(b);
            }
        }

        total_processed += got;
    }

    // Flush remaining
    for(int b = 0; b < 256; b++) flushBucket(b);
}

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " <input_u32.bin> <output_sorted.bin> [workdir] [threads]\n";
        return 1;
    }

    string inpath = argv[1];
    string outpath = argv[2];
    string workdir = (argc >= 4 ? string(argv[3]) : string("."));
    unsigned num_threads = (argc >= 5 ? stoul(argv[4]) : thread::hardware_concurrency());

    if(num_threads == 0) num_threads = 1;

    cerr << "Multi-threaded external radix sort with " << num_threads << " threads\n";

    string src = inpath, dst;

    // 4 passes (LSD, bytes 0..3)
    for(int pass = 0; pass < 4; ++pass) {
        cerr << "Pass " << pass << " (byte " << pass << ")...\n";

        // Get file size
        ifstream test(src, ios::binary | ios::ate);
        uint64_t file_size = test.tellg();
        test.close();

        BucketWriter writer;
        writer.open(workdir, pass);

        atomic<uint64_t> offset(0);
        atomic<uint64_t> total_processed(0);

        // Launch worker threads
        vector<thread> threads;
        for(unsigned t = 0; t < num_threads; t++) {
            threads.emplace_back(worker_thread, ref(src), ref(writer),
                               pass, ref(offset), ref(total_processed), file_size);
        }

        // Progress monitor
        thread monitor([&]() {
            while(total_processed < file_size / sizeof(uint32_t)) {
                this_thread::sleep_for(chrono::milliseconds(500));
                cerr << "  processed " << total_processed << " items\r";
            }
        });

        for(auto& t : threads) t.join();
        monitor.join();

        writer.close();

        cerr << "  processed " << total_processed << " items (done)\n";

        // Concatenate buckets
        ostringstream next;
        next << workdir << "/radix_pass" << pass << "_out.bin";
        dst = next.str();
        concat_buckets(workdir, pass, dst);

        src = dst;
    }

    // Final move to output
    if(src != outpath) {
        remove(outpath.c_str());
        if(rename(src.c_str(), outpath.c_str()) != 0) {
            cerr << "Rename failed; copying...\n";
            ifstream a(src, ios::binary);
            ofstream b(outpath, ios::binary | ios::trunc);
            b << a.rdbuf();
        }
    }

    cerr << "Done. Output: " << outpath << "\n";
    return 0;
}
