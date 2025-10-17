#include "Utils.h"
#include "parallel_reduce.h"
#include "Timer.h"
#include <iostream>
#include <random>
#include <iomanip>
#include <algorithm>

std::vector<int> generate_random_data(size_t size) {
    std::vector<int> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);
    for (auto& element : data)
        element = dist(gen);
    return data;
}

void benchmark_reduce() {
    std::vector<size_t> data_sizes = { 1000, 10000, 100000, 1000000, 10000000 };
    auto max_threads = std::thread::hardware_concurrency();
    std::cout << "Available threads: " << max_threads << "\n\n";

    for (auto size : data_sizes) {
        std::cout << "=== Data size: " << size << " ===\n";
        auto data = generate_random_data(size);

        {
            Timer timer("Library reduce (no policy)");
            auto result = std::reduce(data.begin(), data.end(), 0);
            std::cout << "Result: " << result << "\n";
        }

        {
            Timer timer("Library reduce (seq)");
            auto result = std::reduce(std::execution::seq, data.begin(), data.end(), 0);
        }

        {
            Timer timer("Library reduce (par)");
            auto result = std::reduce(std::execution::par, data.begin(), data.end(), 0);
        }

        {
            Timer timer("Library reduce (par_unseq)");
            auto result = std::reduce(std::execution::par_unseq, data.begin(), data.end(), 0);
        }

        std::cout << "\nCustom parallel reduce:\n";
        std::cout << std::setw(6) << "K" << std::setw(12) << "Time (μs)" << std::setw(15) << "Speedup\n";

        double sequential_time = 0;
        std::vector<std::pair<size_t, long long>> results;

        for (size_t k = 1; k <= 2 * max_threads; ++k) {
            auto data_copy = data;

            auto start = std::chrono::high_resolution_clock::now();
            auto result = parallel_reduce(data_copy.begin(), data_copy.end(), 0, std::plus<>(), k);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            results.emplace_back(k, duration);

            if (k == 1) sequential_time = duration;
            double speedup = sequential_time / duration;

            std::cout << std::setw(6) << k << std::setw(12) << duration
                << std::setw(15) << std::fixed << std::setprecision(2) << speedup << "\n";
        }

        auto best_k = std::min_element(results.begin(), results.end(),
            [](const auto& a, const auto& b) { return a.second < b.second; });

        std::cout << "\nBest K: " << best_k->first
            << " (time: " << best_k->second << " us)\n";
        std::cout << "Ratio to thread count: "
            << static_cast<double>(best_k->first) / max_threads << "\n\n";
    }
}
