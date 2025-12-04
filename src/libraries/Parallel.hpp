template <typename T>
class Parallel : public Fourier<T> {
    private:
        // Input
        // Output
        // Stats
    

    public:
        void compute(const T* input, T* output) override{
            // Implementation of parallel FFT computation
            

        }
        void reverseCompute(const T* input, T* output) override;
        void printStats() override;
};