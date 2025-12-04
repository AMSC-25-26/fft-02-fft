#include <vector>
#include <string>
#include <fstream>

using namespace std;

template <typename T>
class Fourier {
    protected:
        vector<int> *input;
        vector<int> *output;
        long long duration;

    public:
        Fourier() : input(nullptr), output(nullptr), duration(0) {}

        virtual void compute(const T* input, T* output, size_t n) = 0;
        virtual void reverseCompute(const T* input, T* output, size_t n) = 0;
        virtual void printStats();

        void read(const char* filename) {
            ifstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Could not open file");
            }

            int value;
            input = new vector<int>();
            while (file >> value) {
                input->push_back(value);
            }
            file.close();
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