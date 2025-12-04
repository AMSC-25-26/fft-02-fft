template <typename T>
class Fourier {
    private:
        // Input
        // Output
        // Stats
    

    public:
        virtual void compute(const T* input, T* output);
        virtual void reverseCompute(const T* input, T* output);
        virtual void printStats();
};