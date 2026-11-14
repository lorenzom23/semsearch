#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace semsearch {

struct Chunk {
    std::string text;
    std::string source_file;
    uint64_t    offset;
    uint64_t    length;
};

// Gets into folder_path recursively 
// and writes index_path (index.bin) + meta_path (meta.json)
void index_directory(const std::string& folder_path,
                     const std::string& index_path,
                     const std::string& meta_path);

std::vector<Chunk> load_meta(const std::string& meta_path);
std::string read_chunk_text(const Chunk& chunk);

} // namespace semsearch
