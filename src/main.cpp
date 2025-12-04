#include <iostream>
#include <vector>
#include <cmath>
#include <string>

int main(int argc, char* argv[]) {
    //Check on input arguments
    // 1 -> Select compute method (Iterative / Recursive / Parallel / All)
    // 2 -> Input file name
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <method> <input_file>" << std::endl;
        return 1;
    }
    int method = std::stoi(argv[1]);

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
    }
    
}