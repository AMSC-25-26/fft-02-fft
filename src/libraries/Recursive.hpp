#include "Fourier.hpp"
#include "../utilities/Timer.hpp"

using namespace std;

template <typename T>
class Recursive : public Fourier<T> {
    private:
        bool inverse = false;

        vector<T> recursive(const vector<T> &x) {
            int N = x.size();
            if (N == 1) {
                return x;
            }

            const T w = exp(T((inverse ? 1 : -1) * 2.0 * M_PI) / T(N));
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
        void compute() override {
            inverse = false;
            Timer t;

            // Algorithm
            this->output = recursive(this->input);
            this->duration = t.stop_and_return();
        }
        
        void reverseCompute() override {
            inverse = true;
            Timer t;

            // Algorithm + Normalization
            vector<T> Y = recursive(this->input);
            int N = Y.size();
            for (T &it: Y) {
                it /= N;
            }

            this->output = Y;
            this->duration = t.stop_and_return();
        }

        void printStats() override {
            cout << "Recursive FFT completed in " << this->duration << " ms" << endl;
        }
};