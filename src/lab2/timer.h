#pragma once
#include <string>
#include <chrono>
#include <iostream>

class Timer {
public:
    explicit Timer(const std::string& name);
    ~Timer();

private:
    std::string name_;
    std::chrono::time_point<std::chrono::high_resolution_clock> start_;
};
