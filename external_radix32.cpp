#include <bits/stdc++.h>
using namespace std;

// Tunables
static constexpr size_t READ_CHUNK = 10'000'000;  // 10M * 4B ~= 40MB read buffer
static constexpr size_t BUCKET_BUF = 64 * 1024;   // 64KB staging per bucket (total ~16MB)

// Helper: open 256 bucket files for a given pass
static void open_buckets(const string& dir, int pass, vector<ofstream>& buckets){
    buckets.resize(256);
    for(int b=0;b<256;b++){
        ostringstream name;
        name << dir << "/pass" << pass << "_b" << setw(3) << setfill('0') << b << ".bin";
        buckets[b].open(name.str(), ios::binary | ios::trunc);
        if(!buckets[b]) { throw runtime_error("Failed opening bucket file: " + name.str()); }
    }
}
static void close_buckets(vector<ofstream>& buckets){
    for(auto& f: buckets) f.close();
}

// Concatenate buckets in order into output file
static void concat_buckets(const string& dir, int pass, const string& out){
    ofstream dst(out, ios::binary | ios::trunc);
    if(!dst) throw runtime_error("Failed opening: " + out);
    vector<char> buf(4*1024*1024); // 4MB concat buffer
    for(int b=0;b<256;b++){
        ostringstream name;
        name << dir << "/pass" << pass << "_b" << setw(3) << setfill('0') << b << ".bin";
        ifstream src(name.str(), ios::binary);
        if(!src) throw runtime_error("Missing bucket: " + name.str());
        while(src){
            src.read(buf.data(), buf.size());
            streamsize got = src.gcount();
            if(got>0) dst.write(buf.data(), got);
        }
        src.close();
        // remove bucket file to save space
        remove(name.str().c_str());
    }
    dst.close();
}

// External LSD radix on uint32_t file
// input:  path to uint32 file
// output: final sorted file path
int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " <input_u32.bin> <output_sorted.bin> [workdir]\n";
        cerr << "Example: " << argv[0] << " data_5000000000_u32.bin sorted_u32.bin /tmp\n";
        return 1;
    }
    string inpath = argv[1];
    string outpath = argv[2];
    string workdir = (argc>=4 ? string(argv[3]) : string("."));

    // We'll alternate between two files each pass: src -> buckets -> dst
    string src = inpath, dst;

    // Per-bucket staging buffers
    vector<vector<uint32_t>> bucketStage(256);
    for(auto& v: bucketStage) v.reserve(BUCKET_BUF/sizeof(uint32_t));

    // 4 passes (LSD, bytes 0..3)
    for(int pass=0; pass<4; ++pass){
        cerr << "Pass " << pass << " (byte "<<pass<<")...\n";
        vector<ofstream> buckets;
        open_buckets(workdir, pass, buckets);

        ifstream in(src, ios::binary);
        if(!in){ cerr << "Cannot open input: " << src << "\n"; return 1; }

        vector<uint32_t> buf(READ_CHUNK);
        uint64_t total=0;

        auto flushBucket = [&](int b){
            auto& v = bucketStage[b];
            if(!v.empty()){
                buckets[b].write(reinterpret_cast<const char*>(v.data()),
                                 static_cast<streamsize>(v.size()*sizeof(uint32_t)));
                v.clear();
            }
        };

        while(true){
            in.read(reinterpret_cast<char*>(buf.data()),
                    static_cast<streamsize>(buf.size()*sizeof(uint32_t)));
            size_t got = static_cast<size_t>(in.gcount()/sizeof(uint32_t));
            if(got==0) break;
            total += got;

            for(size_t i=0;i<got;i++){
                uint32_t x = buf[i];
                int b = (x >> (8*pass)) & 0xFF;
                auto& v = bucketStage[b];
                v.push_back(x);
                if(v.size()*sizeof(uint32_t) >= BUCKET_BUF) flushBucket(b);
            }
            if(total % 100'000'000ULL == 0) {
                cerr << "  processed " << total << " items\r";
            }
        }
        in.close();
        for(int b=0;b<256;b++) flushBucket(b);
        close_buckets(buckets);

        // Next file name
        ostringstream next;
        next << workdir << "/radix_pass" << pass << "_out.bin";
        dst = next.str();

        concat_buckets(workdir, pass, dst);

        // For next pass, use this as source
        src = dst;
    }

    // Final rename/move to desired output if needed
    if(src != outpath){
        // replace if exists
        remove(outpath.c_str());
        if(rename(src.c_str(), outpath.c_str()) != 0){
            cerr << "Rename failed; copying instead...\n";
            ifstream a(src, ios::binary);
            ofstream b(outpath, ios::binary|ios::trunc);
            b << a.rdbuf();
        }
    }
    cerr << "Done. Output: " << outpath << "\n";
    return 0;
}
