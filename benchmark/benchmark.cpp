#include "index.h"
#include "search.h"
#include <chrono>
#include <random>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <filesystem>

uint32_t constexpr test_queries = 100;

std::chrono::microseconds get_median(const std::vector<std::chrono::microseconds>& times) {
    if (test_queries % 2 != 0) {
        return times[test_queries / 2];
    } else {
        return (times[(test_queries / 2) - 1] + times[test_queries / 2]) / 2;
    }
}

std::chrono::microseconds get_p95(const std::vector<std::chrono::microseconds>& times) {
    size_t p95_index = static_cast<size_t>(std::round(0.95 * (test_queries - 1)));
    return times[p95_index];
}

void execute_test(const std::string& file_path, uint32_t count, uint32_t dim) {
    // Generates the data
    std::filesystem::path path {file_path};
    if (!std::filesystem::exists(path)){
        semsearch::generateRandomIndex(path.c_str(), count, dim);
    }
    semsearch::Index idx(path.c_str());

    // Query vector
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint32_t> dist(0, count - 1);
    std::vector<const float*> queries;
    for (uint32_t i = 0; i < test_queries; ++i) {
        uint32_t random_index = dist(rng);
        queries.push_back(idx.vector(random_index));
    }

    std::cout << "=== " << count << " vectors ===" << std::endl;

    // Execute query (sequential)
    std::vector<std::chrono::microseconds> times;
    for (const auto& query : queries){
        auto start = std::chrono::steady_clock::now();
        auto result = idx.search(query, 10);
        auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    std::sort(times.begin(), times.end());
    std::cout << "  sequential     : median=" << get_median(times).count()
               << "us  p95=" << get_p95(times).count() << "us" << std::endl;

    // Execute query mt (4 threads)
    times.clear();
    for (const auto& query : queries){
        auto start = std::chrono::steady_clock::now();
        auto result = idx.search_mt(query, 10, 4);
        auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    std::sort(times.begin(), times.end());
    std::cout << "  mt (4 threads) : median=" << get_median(times).count()
               << "us  p95=" << get_p95(times).count() << "us" << std::endl;

    // Execute query mt (8 threads)
    times.clear();
    for (const auto& query : queries){
        auto start = std::chrono::steady_clock::now();
        auto result = idx.search_mt(query, 10, 8);
        auto end = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration_cast<std::chrono::microseconds>(end - start));
    }
    std::sort(times.begin(), times.end());
    std::cout << "  mt (8 threads) : median=" << get_median(times).count()
               << "us  p95=" << get_p95(times).count() << "us" << std::endl;
}

int main(){
    // Execute 100k test
    execute_test("data/index_100k.bin", 100000, 384);

    // Execute 1M test
    execute_test("data/index_1M.bin", 1000000, 384);
}
