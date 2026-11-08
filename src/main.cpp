#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "indexer.h"
#include "embedder.h"
#include "search.h"

int main() {
    const auto chunks = semsearch::index_directory("search_demo", "data/index.bin", "data/meta.json");

    semsearch::Embedder embedder("data/model/model.onnx", "data/model/sentencepiece.bpe.model");
    semsearch::Index index("data/index.bin");

    const std::vector<std::string> queries = {
        "I need to take money out at the bank",
        "relaxing on the bank watching the water",
    };

    for (const auto& text : queries) {
        std::cout << "Query: " << text << std::endl;
        const auto query = embedder.embed(text);
        const auto results = index.search(query.data(), 3);

        for (const auto& r : results) {
            std::cout << "   id " << r.id
                      << "  score " << r.score << std::endl
                      << "   " << chunks[r.id].source_file << ": " << chunks[r.id].text << std::endl;
        }
    }

    std::puts("semsearch: ok");
    return 0;
}
