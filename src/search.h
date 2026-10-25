#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "index.h"

namespace semsearch {

struct SearchResult {
    uint32_t id;
    float score;
};

// Dot products of the passed vectors
float dot_product(const float* a, const float* b, uint32_t dim);

class Index {
public:
    explicit Index(const std::string& path);
    ~Index();

    Index(const Index&) = delete;
    Index& operator=(const Index&) = delete;

    // Gets the vector at index i
    const float* vector(uint32_t i) const;

    // Gets the best k results
    std::vector<SearchResult> search(const float* query, uint32_t k) const;

private:
    Header header{};

    void* mapped      = nullptr;
    int fd            = -1;
    size_t file_size  = 0;
    const float* data = nullptr; 
};

} // namespace semsearch
