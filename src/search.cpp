#include "search.h"

#include <algorithm>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <thread>

namespace semsearch {

float dot_product(const float* a, const float* b, uint32_t dim) {
    float dot_prod {0.0};
    for (uint32_t i = 0; i < dim; ++i) {
        dot_prod += a[i] * b[i];
    }
    return dot_prod;
}

Index::Index(const std::string& path) {

    // Open the file
    fd = open(path.c_str(), O_RDONLY);
    if (fd < 0){
        std::puts("Error opening the file!");
        return;
    }

    // Read header
    const auto read_bytes = read(fd, &header, sizeof(header));
    if (read_bytes != sizeof(header)){
        std::puts("Error reading the header of the file!");
        close(fd);
        return;
    }

    // Check header validity
    if (std::memcmp("SEM1", header.magic, 4) != 0 || 1 != header.version){
        std::puts("Header error!");
        close(fd);
        return;
    }

    // Retrieving file dimension
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::puts("Error retrieving file dimension!");
        close(fd);
        return;
    }
    file_size = sb.st_size;
    // mmap
    mapped = mmap(NULL,
                  file_size,         // Size of the file
                  PROT_READ,         // Read
                  MAP_PRIVATE,       // Edit won't be written on disk 
                  fd,                // File descriptor
                  0                  // Start of the mmap
            );
    // Check mmap result
    if (MAP_FAILED == mapped) {
        std::puts("Error during mmap!");
        close(fd);
        return;
    }

    // Start of the data 
    data = reinterpret_cast<const float*>(static_cast<const char*>(mapped) + sizeof(Header)); 
    close(fd);
}

Index::~Index() {
    if (mapped != nullptr && mapped != MAP_FAILED) {
        munmap(mapped, file_size);
    }
}

const float* Index::vector(uint32_t i) const {
    if (i >= header.count){
        return nullptr;
    }
    return data + i * header.dim; 
}

std::vector<SearchResult> Index::search(const float* query, uint32_t k_best_results) const {
    return search_range(query, k_best_results, 0, header.count);
}

std::vector<SearchResult> Index::search_range(const float* query, uint32_t k_best_results, uint32_t start, uint32_t end) const {
  std::vector<SearchResult> results;

    for (uint32_t i = start; i < end; ++i){
        const SearchResult& result = {i, dot_product(query, vector(i), header.dim)};
        // Insert the result in the correct position
        const auto it = std::lower_bound(results.begin(), results.end(), result, 
                                        [](const SearchResult& a, const SearchResult& b) {
                                      return a.score > b.score;
                                    });
        results.insert(it, result);
        if (results.size() > k_best_results) {
            results.pop_back(); 
        }
    }

    return results;
}

std::vector<SearchResult> Index::search_mt(const float* query, uint32_t k_best_results, uint32_t thread_count) const {
    // Split operations on different threads
    uint32_t per_threads = static_cast<uint32_t>(header.count / thread_count);

    // Threads, each one writes into its own slot
    std::vector<std::thread> threads;
    std::vector<std::vector<SearchResult>> partial_results(thread_count);
    for (uint32_t i = 0; i < thread_count; ++i){
        uint32_t start = i * per_threads;
        // Last takes also leftover
        uint32_t end = (i == thread_count - 1) ? header.count : (i + 1) * per_threads;
        threads.push_back(std::thread([this, i, query, k_best_results, start, end, &partial_results]() {
            partial_results[i] = search_range(query, k_best_results, start, end);
        }));
    }

    // Wait all of them
    for (auto& t : threads) {
        t.join();
    }

    // Merge results
    std::vector<SearchResult> merged;
    for (const auto& partial : partial_results) {
        merged.insert(merged.end(), partial.begin(), partial.end());
    }

    std::sort(merged.begin(), merged.end(), [](const SearchResult& a, const SearchResult& b) {
        return a.score > b.score;
    });
    if (merged.size() > k_best_results) {
        merged.resize(k_best_results);
    }

    return merged;
}



} // namespace semsearch
