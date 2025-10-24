#include <bits/stdc++.h>
using namespace std;

int main(int argc, char** argv){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " <input_u64.bin> <output_u32.bin>\n";
        return 1;
    }
    const string inpath = argv[1], outpath = argv[2];

    ifstream in(inpath, ios::binary);
    if(!in){ cerr << "Cannot open input: " << inpath << "\n"; return 1; }
    ofstream out(outpath, ios::binary|ios::trunc);
    if(!out){ cerr << "Cannot open output: " << outpath << "\n"; return 1; }

    constexpr size_t CHUNK = 10'000'000; // 10M x 8B = 80MB input buffer
    vector<uint64_t> buf64(CHUNK);
    vector<uint32_t> buf32(CHUNK);

    uint64_t total = 0;
    while(true){
        in.read(reinterpret_cast<char*>(buf64.data()),
                static_cast<streamsize>(buf64.size()*sizeof(uint64_t)));
        size_t got = static_cast<size_t>(in.gcount()/sizeof(uint64_t));
        if(got==0) break;

        for(size_t i=0;i<got;i++){
            // Map to 32-bit key; options:
            //   low 32 bits:  static_cast<uint32_t>(buf64[i]);
            //   modulo:       static_cast<uint32_t>(buf64[i] % (1ULL<<32));
            buf32[i] = static_cast<uint32_t>(buf64[i]); // low 32 bits
        }
        out.write(reinterpret_cast<char*>(buf32.data()),
                  static_cast<streamsize>(got*sizeof(uint32_t)));
        total += got;
        if(total % 100'000'000ULL == 0) {
            cerr << "Converted " << total << " items\r";
        }
    }
    cerr << "\nDone. Converted " << total << " items.\n";
    return 0;
}
