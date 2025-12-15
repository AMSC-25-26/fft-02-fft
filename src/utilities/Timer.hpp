/**
 * @file Timer.hpp
 * @brief Header file for the Timer utility class.
 */

#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <string>

/**
 * @class Timer
 * @brief A simple timer class for measuring execution time.
 * 
 * This class uses std::chrono::high_resolution_clock to measure elapsed time.
 * It provides methods to stop the timer and return the duration or print it directly.
 */
class Timer {
public:
    /**
     * @brief Constructs a Timer and starts measuring time.
     * 
     * The timer starts immediately upon construction.
     */
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}

    /**
     * @brief Stops the timer and returns the elapsed time in milliseconds.
     * 
     * Calculates the duration from the start time to the current moment.
     * 
     * @return long long The elapsed time in milliseconds.
     */
    long long stop_and_return() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        return duration.count();
    }

    /**
     * @brief Stops the timer and prints the elapsed time to standard output.
     * 
     * @param message The message to print before the duration (default: "Duration").
     */
    void stop_and_print(const std::string& message = "Duration") {
        auto duration = stop_and_return();

        std::cout << message << ": " << duration << " ms" << std::endl;
    }

private:
    /**
     * @brief The starting time point of the timer.
     */
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif // TIMER_HPP