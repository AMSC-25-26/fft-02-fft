/**
 * @file Fourier.hpp
 * @brief Abstract base class for Fast Fourier Transform implementations.
 */

#ifndef FOURIER_HPP
#define FOURIER_HPP

#include <vector>
#include <string>
#include <fstream>
#include <iomanip>

using namespace std;

/**
 * @class Fourier
 * @brief Abstract base class for FFT implementations.
 * 
 * This class defines the interface for computing FFT and Inverse FFT.
 * It handles input/output operations and stores execution statistics.
 * 
 * @tparam T The data type of the signal (usually std::complex<double>).
 */
template <typename T>
class Fourier {
    protected:
        /**
         * @brief Pointer to the input data vector.
         */
        vector<T> *input;

        /**
         * @brief Pointer to the output data vector.
         */
        vector<T> *output;

        /**
         * @brief Duration of the last computation in milliseconds.
         */
        long long duration;

    public:
        /**
         * @brief Default constructor.
         * 
         * Initializes input and output pointers to nullptr and duration to 0.
         */
        Fourier() : input(nullptr), output(nullptr), duration(0) {}

        /**
         * @brief Virtual destructor.
         */
        virtual ~Fourier() {}

        /**
         * @brief Pure virtual method to compute the forward FFT.
         * 
         * Derived classes must implement this method.
         */
        virtual void compute() = 0;

        /**
         * @brief Pure virtual method to compute the inverse FFT.
         * 
         * Derived classes must implement this method.
         */
        virtual void reverseCompute() = 0;

        /**
         * @brief Prints execution statistics.
         * 
         * Prints the duration of the last computation to standard output.
         */
        virtual void printStats() {
            cout << "Duration: " << duration << " ms" << endl;
        }

        /**
         * @brief Reads input data from a file.
         * 
         * Reads values from the specified file into the input vector.
         * If the number of elements is not a power of 2, it pads the input with zeros
         * to the next power of 2.
         * 
         * @param filename The path to the input file.
         * @throws std::runtime_error If the file cannot be opened.
         */
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

        /**
         * @brief Writes output data to a file.
         * 
         * Writes the computed FFT results to the specified file.
         * 
         * @param filename The path to the output file.
         * @throws std::runtime_error If the output data is empty or the file cannot be opened.
         */
        void write(const char* filename) {
            if (output == nullptr) {
                throw runtime_error("Output data is empty");
            }

            ofstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Could not open file");
            }

            file << fixed << setprecision(6);
            for (const auto& value : *output) {
                file << value << endl;
            }
            file.close();
        }

        /**
         * @brief Writes the real part of the output data to a file.
         * 
         * Writes only the real component of the computed results to the specified file.
         * Useful for IFFT output when the original signal was real.
         * 
         * @param filename The path to the output file.
         * @throws std::runtime_error If the output data is empty or the file cannot be opened.
         */
        void writeReal(const char* filename) {
            if (output == nullptr) {
                throw runtime_error("Output data is empty");
            }

            ofstream file(filename);
            if (!file.is_open()) {
                throw runtime_error("Could not open file");
            }

            file << fixed << setprecision(6);
            for (const auto& value : *output) {
                file << value.real() << endl;
            }
            file.close();
        }
};

#endif // FOURIER_HPP