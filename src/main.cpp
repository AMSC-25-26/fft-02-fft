/**
 * @file main.cpp
 * @brief Main entry point for the FFT application.
 *
 * This file contains the main function which handles MPI initialization,
 * command-line argument parsing, and the execution of different FFT implementations
 * (Iterative, Recursive, Parallel) based on user input.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include "libraries/Fourier.hpp"
#include "libraries/Iterative.hpp"
#include "libraries/Recursive.hpp"
#include "libraries/Parallel.hpp"

/**
 * @brief Main function to execute FFT algorithms.
 * 
 * Initializes MPI, parses command line arguments to select the FFT method
 * and input file, and executes the chosen algorithm(s).
 * 
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 *             argv[1]: Method selection (1: Iterative, 2: Recursive, 3: Parallel, 4: All).
 *             argv[2]: Input file path.
 * @return int Exit status (0 for success, 1 for error).
 */
int main(int argc, char* argv[]) {
    // Initialization of MPI
    MPI_Init(&argc, &argv);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Check on input arguments
    // 1 -> Select compute method (Iterative / Recursive / Parallel / All)
    // 2 -> Input file name
    std::string methods[4] = {"Iterative", "Recursive", "Parallel", "All"};
    std::string input_file;
    int method;
    if (argc == 3) {
        method = std::stoi(argv[1]);
        input_file = argv[2];
        if (method < 1 || method > 4){
            std::cerr << "Method must be between 1 and 4, use all" << std::endl;
            method = 4;
        }

        std::cerr << "Usage: " << methods[method-1] << " on file "<< input_file << std::endl;
    } else if (argc == 2){
        std::string input_file = argv[1];
        std::cerr << "Usage: All method on file " << input_file << std::endl;
    } else {
        if (rank == 0) std::cerr << "Usage: <method (1-4)> <input_file>" << std::endl; //just one process prints the message
        MPI_Finalize(); // Finalize MPI also if we have an error
        return 1;
    }

    if (rank == 0) std::cout << "Processing file: " << input_file << " with method " << method << std::endl; //just one process prints the message

    Fourier<std::complex<double>>* fft = nullptr;

    if (method == 4) {
        std::cout << "Running all methods..." << std::endl;
        
        Fourier<std::complex<double>>* runners[] = {
            new Iterative<std::complex<double>>(),
            new Recursive<std::complex<double>>(),
            new Parallel<std::complex<double>>()
        };
        std::string names[] = {"Iterative", "Recursive", "Parallel"};

        for(int i=0; i<3; ++i) {
            std::cout << "\n--- " << names[i] << " ---" << std::endl;
            runners[i]->read(argv[2]);
            runners[i]->compute();
            runners[i]->printStats();
            runners[i]->write(("output_" + names[i] + ".txt").c_str());
            delete runners[i];
        }
        return 0;
    }

    switch (method) {
        case 1:
            fft = new Iterative<std::complex<double>>();
            break;
        case 2:
            fft = new Recursive<std::complex<double>>();
            break;
        case 3:
            fft = new Parallel<std::complex<double>>();
            break;
    }

    fft->read(argv[2]);
    fft->compute();
    fft->printStats();
    fft->write("output.txt");

    MPI_Finalize();
}