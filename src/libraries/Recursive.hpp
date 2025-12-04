#include "Fourier.hpp"
#include "../utilities/Timer.hpp"

using namespace std;

template <typename T>
class Recursive : public Fourier<T> {
    private:
        vector<T> recursive(const vector<T> x) {
            int N = x->size();
            vector<T> evenX, oddX;
            vector<T> evenY, oddY;

            if (N <= 1) {
                this->output[0] = x[0];
                return;
            }
            
            for (int i = 0; i < N; i++) {
                if (i % 2 == 0) {
                    even.push_back(x[i]);
                } else {
                    odd.push_back(x[i]);
                }
            }
        }

    public:
        void compute() {
            Timer t;
            recursive();
            this->duration = t.stop_and_return();
        }
        
        void reverseCompute(const T* input, T* output) override;
        void printStats() override;
};