#include "Timer.h"

Timer::Timer(const std::string& name)
    : name_(name), start_(std::chrono::high_resolution_clock::now()) {}

Timer::~Timer() {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start_);
    std::cout << name_ << ": " << duration.count() << " us\n";
}
