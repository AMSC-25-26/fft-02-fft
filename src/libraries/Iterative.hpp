/**
 * @file Iterative.hpp
 * @brief Header file for the Iterative FFT implementation.
 */

#ifndef ITERATIVE_HPP
#define ITERATIVE_HPP

#include "Fourier.hpp"
#include "../utilities/Timer.hpp"
#include <complex>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <iostream>

/**
 * @class Iterative
 * @brief Implements the Fast Fourier Transform using an iterative Cooley-Tukey algorithm.
 * @tparam T The data type of the signal (usually std::complex<double>).
 */
template <typename T>
class Iterative : public Fourier<T> {
    public:
        /**
         * @brief Computes the forward Fast Fourier Transform iteratively.
         * 
         * This method performs the FFT using an iterative approach with bit-reversal permutation
         * and butterfly operations. It measures the execution time.
         * 
         * @throws std::invalid_argument If the input size is not a power of 2.
         */
        void compute() override {
            size_t n = this->input->size();
            if (n == 0) return;
            if ((n & (n - 1)) != 0) {
                throw std::invalid_argument("Input size must be a power of 2");
            }

            Timer t;

            // Bit reversal permutation on input array
            size_t log_n = 0;
            while ((size_t(1) << log_n) < n) log_n++;

            // Allocate output if not already allocated
            if (this->output == nullptr) {
                this->output = new std::vector<T>(n);
            } else {
                if (this->output->size() != n) {
                    this->output->resize(n);
                }
            }

            for (size_t i = 0; i < n; ++i) {
                size_t j = 0;
                for (size_t bit = 0; bit < log_n; ++bit) {
                    if (i & (size_t(1) << bit)) {
                        j |= (size_t(1) << (log_n - 1 - bit));
                    }
                }
                (*(this->output))[j] = (*(this->input))[i];
            }

            // Butterfly operations
            for (size_t len = 2; len <= n; len <<= 1) {
                double angle = -2.0 * std::acos(-1.0) / len;
                std::complex<double> wlen(std::cos(angle), std::sin(angle));
                for (size_t i = 0; i < n; i += len) {
                    std::complex<double> w(1.0, 0.0);
                    for (size_t j = 0; j < len / 2; j++) {
                        std::complex<double> u = (*(this->output))[i + j];
                        std::complex<double> v = (*(this->output))[i + j + len / 2] * w;
                        (*(this->output))[i + j] = u + v;
                        (*(this->output))[i + j + len / 2] = u - v;
                        w *= wlen;
                    }
                }
            }
            
            this->duration = t.stop_and_return();
        }

        /**
         * @brief Computes the inverse Fast Fourier Transform iteratively.
         * 
         * This method performs the inverse FFT using an iterative approach. It includes
         * bit-reversal permutation, butterfly operations with inverse angles, and normalization.
         * It measures the execution time.
         * 
         * @throws std::invalid_argument If the input size is not a power of 2.
         */
        void reverseCompute() override {
            size_t n = this->input->size();
            if (n == 0) return;
            if ((n & (n - 1)) != 0) {
                throw std::invalid_argument("Input size must be a power of 2");
            }

            Timer t;

            // Bit reversal permutation on input array
            size_t log_n = 0;
            while ((size_t(1) << log_n) < n) log_n++;

            // Allocate output if not already allocated
            if (this->output == nullptr) {
                this->output = new std::vector<T>(n);
            } else {
                if (this->output->size() != n) {
                    this->output->resize(n);
                }
            }

            for (size_t i = 0; i < n; ++i) {
                size_t j = 0;
                for (size_t bit = 0; bit < log_n; ++bit) {
                    if (i & (size_t(1) << bit)) {
                        j |= (size_t(1) << (log_n - 1 - bit));
                    }
                }
                (*(this->output))[j] = (*(this->input))[i];
            }

            // Butterfly operations
            for (size_t len = 2; len <= n; len <<= 1) {
                double angle = 2.0 * std::acos(-1.0) / len; // Positive angle for inverse
                std::complex<double> wlen(std::cos(angle), std::sin(angle));
                for (size_t i = 0; i < n; i += len) {
                    std::complex<double> w(1.0, 0.0);
                    for (size_t j = 0; j < len / 2; j++) {
                        std::complex<double> u = (*(this->output))[i + j];
                        std::complex<double> v = (*(this->output))[i + j + len / 2] * w;
                        (*(this->output))[i + j] = u + v;
                        (*(this->output))[i + j + len / 2] = u - v;
                        w *= wlen;
                    }
                }
            }

            // Normalization
            for (size_t i = 0; i < n; ++i) {
                (*(this->output))[i] /= static_cast<double>(n);
            }

            this->duration = t.stop_and_return();
        }

         /**
         * @brief Prints the statistics of the Iterative FFT/IFFT execution.
         * @param label "FFT" or "IFFT"
         */
         void printStats(const std::string& label) override {
         std::cout << "Iterative " << label << " Duration: " << this->duration << " ms" << std::endl;
        }
};

#endif // ITERATIVE_HPP