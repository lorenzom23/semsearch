#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

namespace semsearch {

struct Chunk {
    std::string text;
    std::string source_file;
    uint64_t    offset = 0;
    uint64_t    length = 0;
};

// Gets into folder_path recursively 
// and writes index_path (index.bin) + meta_path (meta.json)
void index_directory(const std::filesystem::path& folder_path,
                     const std::filesystem::path& index_path,
                     const std::filesystem::path& meta_path);

std::vector<Chunk> load_meta(const std::filesystem::path& meta_path);
std::string read_chunk_text(const Chunk& chunk);

} // namespace semsearch
