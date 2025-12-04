#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include "Fourier.hpp"
#include <mpi.h>

template <typename T>
class Parallel : public Fourier<T> {
    private:
        // Input
        // Output
        // Stats
    

    public:
        void compute() override{
            // Implementation of parallel FFT computation
            
        }
        void reverseCompute() override {
            // Implementation of parallel Inverse FFT computation
        }
        void printStats() override {
            std::cout << "Parallel FFT ";
            Fourier<T>::printStats();
        }
};

#endif // PARALLEL_HPP