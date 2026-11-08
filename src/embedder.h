#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include <onnxruntime_cxx_api.h>
#include <sentencepiece_processor.h>

namespace semsearch {

static constexpr size_t   max_tokens = 126;
static constexpr uint32_t dim        = 384;

class Embedder {
    
public:
    Embedder(const std::string& model_path, const std::string& tokenizer_path);
    std::vector<float> embed(const std::string& text);
    
private:
    std::vector<int64_t> tokenize(const std::string& text) const;
    std::vector<float> run_model(const std::vector<int64_t>& ids);
    std::vector<float> mean_pool(const std::vector<float>& token_vectors, size_t rows) const;
    void normalize(std::vector<float>& v) const;

    sentencepiece::SentencePieceProcessor tokenizer;
    Ort::Env env;   
    Ort::Session session{nullptr};
};

} // namespace semsearch
