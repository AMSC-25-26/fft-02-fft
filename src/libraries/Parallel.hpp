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
        * @param inverse Whether to perform the inverse FFT stage.
        */
        void butterfly_stage(std::vector<T>& data, size_t local_n, size_t len, bool inverse) {
            // If inverse is true (inverse FFT), angle is positive. If false, angle is negative.
            double angle = (inverse ? 2.0 : -2.0) * std::acos(-1.0) / len;
            std::complex<double> wlen(std::cos(angle), std::sin(angle));

            // Heuristic: if the number of outer iterations is large enough, use the efficient recurrence.
            // Otherwise (large len), use collapse(2) with direct calculation to maximize parallelism.
            if (local_n / len >= 32) {
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
            } else {
                #pragma omp parallel for collapse(2) schedule(static)
                for (size_t i = 0; i < local_n; i += len) {
                    for (size_t j = 0; j < len / 2; j++) {
                        std::complex<double> w = std::polar(1.0, angle * j);
                        std::complex<double> u = data[i + j];
                        std::complex<double> v = data[i + j + len / 2] * w;

                        // butterfly operation
                        data[i + j] = u + v;
                        data[i + j + len / 2] = u - v;
                    }
                }
            }
        }

        /**
        * @brief The method that runs both Forward and Inverse Fast Fourier Transform using MPI and OpenMP.
        * 
        * This method performs the FFT in parallel. It handles data distribution,
        * bit-reversal permutation (on rank 0), and butterfly operations.
        * For stages where the butterfly width exceeds local data size, data is gathered
        * to rank 0, processed, and scattered back.
        *
        * @note This implementation gathers data to Rank 0 for stages where the butterfly width exceeds the local partition size.
        *
        * @param inverse If true, uses positive angles and normalizes the result.
        */
        void executeFFT(bool inverse) {
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
            while ((size_t(1) << log_n) < static_cast<size_t>(global_n)) log_n++; // casting global_n to size_t

            // Bit reversal permutation
            std::vector<T> permuted_input;
            // rank 0 only (because just the root process handles the initial data permutation)
            if (rank == 0) {
                permuted_input.resize(global_n);

                // "schedule(static)" because the work per iteration is identical
                #pragma omp parallel for schedule(static)
                for (size_t i = 0; i < static_cast<size_t>(global_n); ++i) { // casting gloabl_n to use size_t
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

                // Prepare Output: resize output on rank 0 to hold final result later
                if (this->output == nullptr) {
                    this->output = make_unique<vector<T>>(global_n);
                } else {
                    this->output->resize(global_n);
                }
            }

            // Scatter permuted input to all processes:
            int local_n = global_n / size;// Local partition size (Note: MPI counts must be int)
            std::vector<T> local_data(local_n);// Each process gets a chunk of size local_n

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
                    butterfly_stage(local_data, local_n, len, inverse); // casting: local_n is int and len is size_t
                    // Pass the inverse flag to handle both FFT and IFFT
                }

                // CASE 2: The butterfly spans multiple processes, distributed calculation
                else {                    
                    size_t half_len = len / 2;
                    // Calculate how many processes are in one "half" of the butterfly
                    // Since len > local_n, the butterfly stride spans across processes.
                    int group_size = static_cast<int>(half_len / local_n); 
                    
                    // Find partner process using XOR (hypercube topology)
                    int partner = rank ^ group_size;

                    std::vector<T> buffer(local_n);

                    // Exchange data with partner
                    MPI_Status status;
                    MPI_Sendrecv(local_data.data(), local_n, MPI_DOUBLE_COMPLEX, partner, 0,
                                 buffer.data(), local_n, MPI_DOUBLE_COMPLEX, partner, 0,
                                 comm, &status);

                    // Determine if I am the "lower" (u) or "upper" (v) part of the butterfly
                    // If the bit corresponding to group_size is 0, I am lower.
                    bool is_lower = (rank & group_size) == 0;

                    // Calculate angle parameters
                    double angle = (inverse ? 2.0 : -2.0) * std::acos(-1.0) / len;
                    
                    // Calculate the global index offset for w calculation
                    // The 'j' index in the butterfly runs 0..half_len-1
                    // My segment of 'j' starts at (rank % group_size) * local_n
                    size_t start_j = (rank % group_size) * local_n;

                    #pragma omp parallel for schedule(static)
                    for (size_t i = 0; i < static_cast<size_t>(local_n); ++i) {
                        // Calculate w for this specific index
                        std::complex<double> w = std::polar(1.0, angle * static_cast<double>(start_j + i));
                        
                        std::complex<double> u, v;
                        if (is_lower) {
                            // I have u, received v
                            u = local_data[i];
                            v = buffer[i];
                            local_data[i] = u + v * w;
                        } else {
                            // I have v, received u
                            u = buffer[i];
                            v = local_data[i];
                            local_data[i] = u - v * w;
                        }
                    }
                }
            }

            //Final Gather
            MPI_Gather(local_data.data(), local_n, MPI_DOUBLE_COMPLEX,
                    rank == 0 ? this->output->data() : nullptr, local_n, MPI_DOUBLE_COMPLEX, 0, comm);

            // Normalization (is needed for the inverse only!)
            if (inverse && rank == 0) {
                #pragma omp parallel for
                for (size_t i = 0; i < static_cast<size_t>(global_n); ++i) {
                    (*(this->output))[i] /= static_cast<double>(global_n);
                }
            }

            this->duration = t.stop_and_return();
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
         */
        void compute() override{
            // Forward FFT computation
            executeFFT(false); // inverse = false
        }

        /**
         * @brief Computes the inverse Fast Fourier Transform using MPI and OpenMP.
         */
        void reverseCompute() override {
            // Inverse FFT computation
            executeFFT(true); // inverse = true
        }

        /**
         * @brief Prints the statistics of the Parallel FFT/IFFT execution.
         * @param label "FFT" or "IFFT"
         */
        void printStats(const std::string& label) override {
            if (rank == 0) {
                std::cout << "Parallel " << label << " Duration: " << this->duration << " ms" << std::endl;
            }
        }
};

#endif // PARALLEL_HPP