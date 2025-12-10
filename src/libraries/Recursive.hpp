/**
 * @file Recursive.hpp
 * @brief Header file for the Recursive FFT implementation.
 */
#ifndef RECURSIVE_HPP
#define RECURSIVE_HPP

#include "Fourier.hpp"
#include "../utilities/Timer.hpp"
#include <complex>
#include <cmath>
#include <vector>
#include <stdexcept>

using namespace std;

/**
 * @class Recursive
 * @brief Implements the Fast Fourier Transform using a recursive Cooley-Tukey algorithm.
 * @tparam T The data type of the signal (usually std::complex<double>).
 */
template <typename T>
class Recursive : public Fourier<T> {
    private:
        /**
         * @brief Flag to indicate if the inverse FFT is being computed.
         */
        bool inverse = false;

        /**
         * @brief Recursively computes the FFT of the input vector.
         * 
         * This method implements the Cooley-Tukey algorithm by splitting the input
         * into even and odd indexed elements and recursively solving sub-problems.
         * 
         * @param x The input vector for the current recursion level.
         * @return vector<T> The transformed vector.
         */
        vector<T> recursive(const vector<T> &x) {
            int N = x.size();
            if (N == 1) {
                return x;
            }

            // Calculate w = exp(i * 2 * pi / N)
            // Assuming T is std::complex<double>
            double angle = (inverse ? 1 : -1) * 2.0 * M_PI / N;
            const T w = std::polar(1.0, angle);
            T w_k = 1;

            vector<T> evenX, oddX;
            vector<T> evenY, oddY;
            vector<T> Y(N);
            
            evenX.reserve(N / 2);
            oddX.reserve(N / 2);
            for (int i = 0; i < N; i++) {
                if (i % 2 == 0) {
                    evenX.push_back(x[i]);
                } else {
                    oddX.push_back(x[i]);
                }
            }

            evenY = recursive(evenX);
            oddY = recursive(oddX);

            for (int i = 0; i < (int) N / 2; i++) {
                Y[i] = evenY[i] + w_k * oddY[i];
                Y[i + N/2] = evenY[i] - w_k * oddY[i];
                w_k *= w;
            }

            return Y;
        }

    public:
        /**
         * @brief Computes the forward Fast Fourier Transform.
         * 
         * This method initializes the recursion and measures the execution time.
         */
        void compute() override {
            inverse = false;
            Timer t;

            // Algorithm
            vector<T> result = recursive(*(this->input));
            this->output = new vector<T>(result);
            this->duration = t.stop_and_return();
        }
        
        /**
         * @brief Computes the inverse Fast Fourier Transform.
         * 
         * This method sets the inverse flag, performs the recursive computation,
         * normalizes the result by dividing by N, and measures the execution time.
         */
        void reverseCompute() override {
            inverse = true;
            Timer t;

            // Algorithm + Normalization
            vector<T> Y = recursive(*(this->input));
            int N = Y.size();
            for (T &it: Y) {
                it /= N;
            }

            this->output = new vector<T>(Y);
            this->duration = t.stop_and_return();
        }

        /**
         * @brief Prints the statistics of the Recursive FFT/IFFT execution.
         * @param label "FFT" or "IFFT"
         */
        void printStats(const std::string& label) override {
            cout << "Recursive " << label << " Duration: " << this->duration << " ms" << endl;
        }
};

#endif