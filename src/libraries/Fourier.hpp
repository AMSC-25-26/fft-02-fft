#ifndef FOURIER_HPP
#define FOURIER_HPP

#include <vector>
#include <string>
#include <fstream>

using namespace std;

template <typename T>
class Fourier {
    protected:
        vector<T> *input;
        vector<T> *output;
        long long duration;

    public:
        Fourier() : input(nullptr), output(nullptr), duration(0) {}

        virtual ~Fourier() {}

        virtual void compute() = 0;
        virtual void reverseCompute() = 0;
        virtual void printStats() {
            cout << "Duration: " << duration << " microseconds" << endl;
        }

        void read(const char* filename) {
            ifstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Could not open file");
            }

            T value;
            input = new vector<T>();
            while (file >> value) {
                input->push_back(value);
            }
            file.close();

            // Pad to next power of 2
            size_t n = input->size();
            if (n > 0 && (n & (n - 1)) != 0) {
                size_t next_pow2 = 1;
                while (next_pow2 < n) next_pow2 <<= 1;
                input->resize(next_pow2, T(0));
                cout << "Warning: Input size " << n << " is not a power of 2. Padded to " << next_pow2 << endl;
            }
        }

        void write(const char* filename) {
            if (output == nullptr) {
                throw runtime_error("Output data is empty");
            }

            ofstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Could not open file");
            }

            for (const auto& value : *output) {
                file << value << endl;
            }
            file.close();
        }
};

#endif // FOURIER_HPP