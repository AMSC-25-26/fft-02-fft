#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "libraries/Fourier.hpp"
#include "libraries/Iterative.hpp"
#include "libraries/Recursive.hpp"

int main(int argc, char* argv[]) {
    //Check on input arguments
    // 1 -> Select compute method (Iterative / Recursive / Parallel / All)
    // 2 -> Input file name
    std::string methods[4] = {"Iterative", "Recursive", "Parallel", "All"};
    int method;
    if (argc == 3) {
        method = std::stoi(argv[1]);
        std::string input_file = argv[2];
        if (method < 1 || method > 4){
            std::cerr << "Method must be between 1 and 4, use all" << std::endl;
            method = 4;
        }

        std::cerr << "Usage: " << methods[method-1] << " on file "<< input_file << std::endl;
    } else if (argc == 2){
        std::string input_file = argv[1];
        std::cerr << "Usage: All method on file " << input_file << std::endl;
    } else {
        std::cerr << "Usage: <method (1-4)> <input_file>" << std::endl;
        return 1;
    }

    Fourier<double>* fft = nullptr;
    switch (method) {
        case 1:
            fft = new Iterative<double>();
            break;
        case 2:
            fft = new Recursive<double>();
            break;
        case 3:
            fft = new Parallel<double>();
            break;
        case 4:
            fft1 = new Iterative<double>();
            fft2 = new Recursive<double>();
            fft3 = new Parallel<double>();
            break;
    }

    fft->read(argv[2]);
    fft->compute();
    fft->printStats();
    fft->write("output.txt");
}