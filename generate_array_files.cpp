#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <limits>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Target sizes
    vector<unsigned long long> sizes = {
        5, 50, 500, 5000, 50000, 500000, 5000000, 50000000, 500000000
    };

    random_device rd;
    mt19937_64 gen(rd());

    for (auto n : sizes) {
        cout << "Generating array of size " << n << "...\n";

        try {
            vector<unsigned long long> A(n);
            uniform_int_distribution<unsigned long long> dist(0, 2 * n);

            for (unsigned long long i = 0; i < n; ++i)
                A[i] = dist(gen);

            string filename = "data_" + to_string(n) + ".bin";
            ofstream fout(filename, ios::binary);

            if (!fout) {
                cerr << "  ❌ Could not open " << filename << " for writing.\n\n";
                continue;
            }

            fout.write(reinterpret_cast<const char*>(A.data()), A.size() * sizeof(unsigned long long));
            fout.close();

            cout << "  ✅ Wrote " << filename << " (" << (A.size() * sizeof(unsigned long long) / 1e6)
                 << " MB). First few values: ";
            for (unsigned long long i = 0; i < min(n, 10ULL); ++i)
                cout << A[i] << " ";
            cout << "\n\n";
        }
        catch (const bad_alloc&) {
            cerr << "  ❌ Skipping size " << n << " — not enough memory.\n\n";
        }
    }

    cout << "All done!\n";
    return 0;
}
