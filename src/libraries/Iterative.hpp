template <typename T>
class Iterative : public Fourier<T> {
    private:
        // Input
        // Output
        // Stats
    

    public:
        void compute(const T* input, T* output) override{
            // Implementation of iterative FFT computation
            

        }
        void reverseCompute(const T* input, T* output) override;
        void printStats() override;
};