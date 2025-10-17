#pragma once
#include <vector>
#include <thread>
#include <numeric>
#include <execution>
#include <functional>

template<typename Iterator, typename T, typename BinaryOp>
T parallel_reduce(Iterator first, Iterator last, T init, BinaryOp op, size_t num_threads) {
    auto size = std::distance(first, last);
    if (size == 0) return init;
    if (num_threads == 1) return std::reduce(first, last, init, op);

    std::vector<std::thread> threads;
    std::vector<T> results(num_threads);
    auto block_size = size / num_threads;

    for (size_t i = 0; i < num_threads; ++i) {
        auto start = first + i * block_size;
        auto end = (i == num_threads - 1) ? last : start + block_size;

        threads.emplace_back([start, end, &results, i, init, op]() {
            results[i] = std::reduce(start, end, init, op);
            });
    }

    for (auto& thread : threads) thread.join();

    return std::reduce(results.begin(), results.end(), init, op);
}
