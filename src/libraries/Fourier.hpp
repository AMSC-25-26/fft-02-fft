template <typename T>
class Fourier {
    private:
        // Input
        vector<int> 
        // Output
        // Stats
    

    public:
        virtual void compute(const T* input, T* output);
        virtual void reverseCompute(const T* input, T* output);
        virtual void printStats();
};