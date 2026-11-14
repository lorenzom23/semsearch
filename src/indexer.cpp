#include "indexer.h"
#include "index.h"
#include "embedder.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;

namespace semsearch {

const std::vector<std::string> allowed_extensions = {".txt", ".md", ".rst"};

uint32_t constexpr chunk_size    = 80;  // words per chunk
uint32_t constexpr chunk_overlap = 10;  // words of overlap between chunks

// A word with its byte offset in the original text
struct Word {
    std::string text;
    uint64_t    offset;
};

bool is_hidden(const std::filesystem::path& path) {
    const std::string path_string = path.filename().string();
    return path_string.starts_with('.');
}

bool has_allowed_extension(const std::filesystem::path& path) {
    for (const auto& extension : allowed_extensions){
        if (path.extension() == extension){
            return true;
        }
    }
    return false;
}

std::string read_file(const std::filesystem::path& path) {
    std::ifstream file(path.string());
    if (file.is_open()) {
        std::string text((std::istreambuf_iterator<char>(file)),
                               std::istreambuf_iterator<char>());
        std::cout << "File readed (length): " << text.length() << std::endl;
        file.close();
        return text;
    } 
    return "";
}

std::vector<Word> split_words(const std::string& text) {
    std::vector<Word> result;
    uint32_t start{0};
    for (uint32_t i = 0; i < text.size(); i++){
        bool is_last = (i == text.size() - 1);
        if (text[i] == ' ' || is_last){
            uint32_t len = (text[i] == ' ') ? (i - start) : (i - start + 1);
            if (len > 0) {
                result.push_back({text.substr(start, len), start});
            }
            start = i + 1;
        }
    }
    return result;
}

std::vector<Chunk> chunk_text(const std::string& text, const std::string& source_file) {
    auto words = split_words(text);
    std::vector<Chunk> chunks;
    
    if (words.empty()) {
        return chunks;
    }

    uint32_t step = chunk_size - chunk_overlap;
    for (uint32_t start = 0; start < words.size(); start += step) {
        uint32_t end = std::min(start + chunk_size, static_cast<uint32_t>(words.size()));
        std::string body;
        for (uint32_t i = start; i < end; ++i) {
            if (i > start) { // Avoid the space at the beginning of the text
                body += ' ';
            }
            body += words[i].text;
        }

        // Chuncks
        uint64_t start_offset = words[start].offset;
        uint64_t end_offset = words[end - 1].offset + words[end - 1].text.size(); 
        uint64_t length = end_offset - start_offset;
        chunks.push_back({body, source_file, start_offset, length});

        if (end == words.size()) {
            break;
        }
    }

    return chunks;
}

void write_meta(const std::string& meta_path, const std::vector<Chunk>& chunks) {
    std::ofstream file(meta_path);
    nlohmann::json meta = nlohmann::json::array();
    
    for (size_t i = 0; i < chunks.size(); ++i) {
        meta.push_back({
            {"id",     i},
            {"file",   chunks[i].source_file},
            {"offset", chunks[i].offset},
            {"length", chunks[i].length},
        });
    }
    file << meta.dump(2);
}

void write_index(const std::string& index_path, const std::vector<Chunk>& chunks, Embedder& embedder) {
    std::vector<float> vectors;
    vectors.reserve(chunks.size() * dim);    
    for (const auto& chunk : chunks){
        const auto v = embedder.embed(chunk.text);
        vectors.insert(vectors.end(), v.begin(), v.end());
    }
    writeIndex(index_path, vectors, dim);

    // generateRandomIndex(index_path, static_cast<uint32_t>(chunks.size()));
}

std::vector<Chunk> index_directory(const std::string& folder_path, const std::string& index_path, const std::string& meta_path) {
    std::vector<Chunk> all_chunks;
    fs::path fs_path {folder_path};

    try {

        auto end = fs::recursive_directory_iterator();
        for (auto it = fs::recursive_directory_iterator(fs_path); it != end; ++it) {
            auto const& entry = *it;

            if (entry.is_directory() && is_hidden(entry.path())) {
                it.disable_recursion_pending();
                continue;
            }

            if (entry.is_regular_file() && !is_hidden(entry.path()) && has_allowed_extension(entry.path())) {
                // Read the file
                std::string text = read_file(entry.path());
                

                std::vector<Chunk> chunks = chunk_text(text, entry.path().string());
                all_chunks.insert(all_chunks.end(), chunks.begin(), chunks.end());
            }
        }

    } catch (fs::filesystem_error const& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
    }

    // Load model
    Embedder embedder("data/model/model.onnx", "data/model/sentencepiece.bpe.model");

    write_meta(meta_path, all_chunks);
    write_index(index_path, all_chunks, embedder);

    return all_chunks;
}

}// namespace semsearch
