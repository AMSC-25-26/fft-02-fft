#ifndef TIMER_HPP
#define TIMER_HPP

#include <iostream>
#include <chrono>
#include <thread>
#include <string>

class Timer {
public:
    Timer() : start_time(std::chrono::high_resolution_clock::now()) {}

    long long stop_and_return() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

        return duration.count();
    }

    void stop_and_print(const std::string& message = "Duration") {
        auto duration = stop_and_return();

        std::cout << message << ": " << duration << " ms" << std::endl;
    }

private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
};

#endif // TIMER_HPP