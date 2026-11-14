#include <cstdlib>
#include <stdexcept>
#include <iostream>
#include <string>
#include <filesystem>

#include "indexer.h"
#include "embedder.h"
#include "search.h"

namespace fs = std::filesystem;

constexpr auto index_filename     = "index.bin";
constexpr auto meta_filename      = "meta.json";
constexpr auto model_filename     = "model.onnx";
constexpr auto tokenizer_filename = "sentencepiece.bpe.model";
constexpr auto usage              = "usage: semsearch index <dir> | semsearch query \"<text>\"";

// Where to store files
fs::path data_dir() {
    const char* folder = std::getenv("XDG_DATA_HOME");

    if (nullptr != folder && *folder != '\0'){
        return fs::path{folder} / "semsearch";
    }

    folder = std::getenv("HOME");
    if (nullptr != folder && *folder != '\0'){
        return fs::path{folder} / ".local/share/semsearch";
    }

    throw std::runtime_error("HOME is not set");
}

void execute_query(const std::string& query, const fs::path& index_path, const fs::path& meta_path, semsearch::Embedder& embedder) {
    const auto meta = semsearch::load_meta(meta_path);
    if (meta.empty()) {
        std::cerr << "No index found, run: semsearch index <dir>" << std::endl;
        return;
    }

    semsearch::Index index(index_path.string());

    const auto query_embedded = embedder.embed(query);
    const auto results = index.search(query_embedded.data(), 3);

    for (const auto& r : results) {
        const std::string text = semsearch::read_chunk_text(meta[r.id]);
        std::cout << "id " << r.id << "  score " << r.score << std::endl
                  << meta[r.id].source_file << ": " << text << std::endl
                  << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3){
        std::cerr << usage << std::endl;
        return 1;
    }

    const std::string command = argv[1];
    if (command != "index" && command != "query") {
        std::cerr << "Unknown command: " << command << std::endl
                  << usage << std::endl;
        return 1;
    }

    try {
        const fs::path dir = data_dir();
        const fs::path model_path = dir / "model";

        if (!fs::exists(model_path / model_filename) ||
            !fs::exists(model_path / tokenizer_filename)) {
            std::cerr << "Model not found in " << model_path << std::endl
                      << "Run: scripts/model.sh" << std::endl;
            return 1;
        }

        semsearch::Embedder embedder((model_path / model_filename).string(),
                                     (model_path / tokenizer_filename).string());

        if (command == "index") {
            semsearch::index_directory(argv[2], dir / index_filename, dir / meta_filename, embedder);
        } else {
            execute_query(argv[2], dir / index_filename, dir / meta_filename, embedder);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
