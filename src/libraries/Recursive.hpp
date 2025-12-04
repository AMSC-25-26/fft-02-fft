template <typename T>
class Recursive : public Fourier<T> {
    private:
        // Input
        // Output
        // Stats
    

    public:
        void compute(const T* input, T* output) override{
            // Implementation of recursive FFT computation
            
        }
        void reverseCompute(const T* input, T* output) override;
        void printStats() override;
};