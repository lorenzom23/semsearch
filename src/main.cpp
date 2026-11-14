#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "indexer.h"
#include "embedder.h"
#include "search.h"

int main() {
    semsearch::index_directory("search_demo", "data/index.bin", "data/meta.json");

    semsearch::Embedder embedder("data/model/model.onnx", "data/model/sentencepiece.bpe.model");
    semsearch::Index index("data/index.bin");

    const std::vector<std::string> queries = {
        "I need to take money out at the bank",
        "relaxing on the bank watching the water",
    };

    const auto meta = semsearch::load_meta("data/meta.json");

    for (const auto& q : queries) {
        std::cout << "Query: " << q << std::endl;
        const auto query = embedder.embed(q);
        const auto results = index.search(query.data(), 3);

        for (const auto& r : results) {
            const std::string text = semsearch::read_chunk_text(meta[r.id]);
            std::cout << "   id " << r.id
                      << "  score " << r.score << std::endl
                      << "   " << meta[r.id].source_file << ": " << text << std::endl;
        }
    }

    std::puts("semsearch: ok");
    return 0;
}
