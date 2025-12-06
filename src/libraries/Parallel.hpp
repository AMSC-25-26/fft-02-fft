/**
 * @file Parallel.hpp
 * @brief Header file for the Parallel FFT implementation.
 */
#ifndef PARALLEL_HPP
#define PARALLEL_HPP

#include "Fourier.hpp"
#include "../utilities/Timer.hpp"
#include <mpi.h>
#include <cmath>
#include <vector>
#include <complex>
#include <algorithm>

/**
 * @class Parallel
 * @brief Implements the Fast Fourier Transform using MPI for distributed computing
 * and OpenMP for local parallelism.
 * @tparam T The data type of the signal.
 */
template <typename T>
class Parallel : public Fourier<T> {
    private:
        // MPI variables
        int rank;
        int size;
        MPI_Comm comm;

        /**
        * @brief Performs one stage of the Cooley-Tukey butterfly operations.
        * @param data The vector to modify (can be local_data or full_data).
        * @param local_n The number of elements to process in this vector.
        * @param len The length of the current stage.
        */
        void butterfly_stage(std::vector<T>& data, size_t local_n, size_t len) {
            double angle = -2.0 * std::acos(-1.0) / len;
            std::complex<double> wlen(std::cos(angle), std::sin(angle));

            #pragma omp parallel for schedule(static)
            for (size_t i = 0; i < local_n; i += len) {
                std::complex<double> w(1.0, 0.0);

                for (size_t j = 0; j < len / 2; j++) {
                    std::complex<double> u = data[i + j];
                    std::complex<double> v = data[i + j + len / 2] * w;

                    // butterfly operation
                    data[i + j] = u + v;
                    data[i + j + len / 2] = u - v;

                    // update w
                    w *= wlen;
                }
            }
        }
    

    public:
        /**
         * @brief Constructs a Parallel FFT object.
         * 
         * Initializes MPI rank and size based on the provided communicator.
         * 
         * @param communicator The MPI communicator to use (default: MPI_COMM_WORLD).
         */
        Parallel(MPI_Comm communicator = MPI_COMM_WORLD) : comm(communicator) {
            MPI_Comm_rank(comm, &rank);
            MPI_Comm_size(comm, &size);
        }
        
        /**
         * @brief Computes the forward Fast Fourier Transform using MPI and OpenMP.
         * 
         * This method performs the FFT in parallel. It handles data distribution,
         * bit-reversal permutation (on rank 0), and butterfly operations.
         * For stages where the butterfly width exceeds local data size, data is gathered
         * to rank 0, processed, and scattered back.
         */
        void compute() override{
            // Implementation of parallel FFT computation
            Timer t;

            // Setup
            int global_n = 0;
            if (rank == 0) {
                global_n = static_cast<int>(this->input->size());
            }
            // Broadcast total size to all processes
            MPI_Bcast(&global_n, 1, MPI_INT, 0, comm);

            // Compute log_n
            size_t log_n = 0;
            while ((size_t(1) << log_n) < static_cast<size_t>(global_n)) log_n++; // ?? casting global_n to size_t

            // Bit reversal permutation
            std::vector<T> permuted_input;
            // rank 0 only (because just the root process handles the initial data permutation)
            if (rank == 0) {
                permuted_input.resize(global_n);

                // "schedule(static)" because the work per iteration is identical
                #pragma omp parallel for schedule(static)
                for (size_t i = 0; i < static_cast<size_t>(global_n); ++i) { //?? casting gloabl_n to use size_t
                    size_t j = 0; // variable to save the reversed index
                    size_t temp_i = i;
                    
                    // Bit reversal computation from the indices i to j
                    for (size_t bit = 0; bit < log_n; ++bit) {
                        j = (j << 1) | (temp_i & 1);
                        temp_i >>= 1;
                    }
                    
                    // Note that no two threads write to same 'j' (so it's safe)
                    permuted_input[j] = (*(this->input))[i];
                }

                // Resize output on rank 0 to hold final result later
                if (this->output == nullptr) {
                    this->output = new std::vector<T>(global_n);
                } else {
                    this->output->resize(global_n);
                }
            }

            // Scatter permuted input to all processes:
            int local_n = global_n / size; // Local partition size (Note: MPI counts must be int)
            std::vector<T> local_data(local_n); // Each process gets a chunk of size local_n

            // Mapping T from std::complex<double> to MPI_DOUBLE_COMPLEX
            MPI_Scatter(rank == 0 ? permuted_input.data() : nullptr, 
                        local_n, MPI_DOUBLE_COMPLEX,
                        local_data.data(), 
                        local_n, MPI_DOUBLE_COMPLEX, 
                        0, comm);


            // Butterfly operations Stages
            // The outer loop goes throw the "Stage" of the FFT (cannot be parallelized due to dependencies)
            for (int len = 2; len <= global_n; len <<= 1) {                
                
                // CASE 1: The butterfly fits entirely inside local memory
                if (len <= local_n) {
                    butterfly_stage (local_data, local_n, len); // ?? local_n is int and len is size_t
                }

                // CASE 2: The butterfly spans multiple processes
                else {
                    // Gather all current local work to Rank 0
                    std::vector<T> full_data;
                    if (rank == 0) full_data.resize(global_n);

                    MPI_Gather(local_data.data(), local_n, MPI_DOUBLE_COMPLEX,
                            rank == 0 ? full_data.data() : nullptr, local_n, MPI_DOUBLE_COMPLEX,
                            0, comm);

                    // Rank 0 performs the butterfly operation on the full data
                    if (rank == 0) {
                        butterfly_stage(full_data, global_n, len); // ?? global_n is int and len is size_t
                    }

                    // Scatter back to processes
                    MPI_Scatter(rank == 0 ? full_data.data() : nullptr, local_n, MPI_DOUBLE_COMPLEX,
                                local_data.data(), local_n, MPI_DOUBLE_COMPLEX,
                                0, comm);
                }

                //Final Gather
                MPI_Gather(local_data.data(), local_n, MPI_DOUBLE_COMPLEX,
                       rank == 0 ? this->output->data() : nullptr, local_n, MPI_DOUBLE_COMPLEX,
                       0, comm);
            }            
            this->duration = t.stop_and_return();
        }

        /**
         * @brief Computes the inverse Fast Fourier Transform.
         * 
         * @note This method is currently not implemented for the Parallel version.
         */
        void reverseCompute() override {
            // Implementation of parallel Inverse FFT computation
        }

        /**
         * @brief Prints the statistics of the Parallel FFT execution.
         * 
         * Outputs the method name and the duration of the last computation.
         */
        void printStats() override {
            std::cout << "Parallel FFT ";
            Fourier<T>::printStats();
        }
};

#endif // PARALLEL_HPP