#include <complex>
#include <cmath>

template <typename T>
class Iterative : public Fourier<T> {
    private:
        // Input
        // Output
        // Stats
    

    public:
        void compute(const T* input, T* output) override{
            // Implementation of iterative FFT computation

            // Bit reversal permutation on input array
            size_t n = this->input->size();
            size_t log_n = 0;
            while ((size_t(1) << log_n) < n) log_n++;

            for (size_t i = 0; i < n; ++i) {
                size_t j = 0;
                for (size_t bit = 0; bit < log_n; ++bit) {
                    if (i & (size_t(1) << bit)) {
                        j |= (size_t(1) << (log_n - 1 - bit));
                    }
                }
                output[j] = input[i];
            }

            // Butterfly operations
            for (size_t len = 2; len <= n; len <<= 1) {
                double angle = -2.0 * std::acos(-1.0) / len;
                std::complex<double> wlen(std::cos(angle), std::sin(angle));
                for (size_t i = 0; i < n; i += len) {
                    std::complex<double> w(1.0, 0.0);
                    for (size_t j = 0; j < len / 2; j++) {
                        std::complex<double> u = output[i + j];
                        std::complex<double> v = output[i + j + len / 2] * w;
                        output[i + j] = u + v;
                        output[i + j + len / 2] = u - v;
                        w *= wlen;
                    }
                }
            }

        }
        void reverseCompute(const T* input, T* output) override;
        void printStats() override;
};