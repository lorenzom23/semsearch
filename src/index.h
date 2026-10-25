#pragma once

#include <cstdint>
#include <string>

namespace semsearch {

// index.bin header, 32 bytes.
struct Header {
    char magic[4];
    uint32_t version;
    uint32_t dim;         // floats per vector
    uint32_t count;       // number of vectors
    uint32_t reserved[4];
};
static_assert(sizeof(Header) == 32);

void generateRandomIndex(const std::string& path, uint32_t count, uint32_t dim = 384);

} // namespace semsearch
