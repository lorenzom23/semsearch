#include "embedder.h"
#include <array>
#include <vector>
#include <numeric>
#include <algorithm>
#include <stdexcept>
#include <format>
#include <cmath>
#include <cstdio>

namespace semsearch {

Embedder::Embedder(const std::string& model_path, const std::string& tokenizer_path) {
    // Tokenizer
    const auto status = tokenizer.Load(tokenizer_path);
    if (!status.ok()){
        throw std::runtime_error( status.ToString() );
    }

    // Ort with default options
    Ort::SessionOptions session_options;
    session = Ort::Session(env, model_path.c_str(), session_options);
}

std::vector<int64_t> Embedder::tokenize(const std::string& text) const {
    std::vector<int> ids;
    auto status = tokenizer.Encode(text, &ids);

    if (!status.ok()){
        throw std::runtime_error( std::format("Error encoding text: {}", status.ToString()) );
    }

    // Leave room for <s> and </s>
    if (ids.size() > max_tokens ) {
        ids.resize(max_tokens);
        std::puts("Tokenized vector resized!");
    }

    // Fix the ids value
    std::vector<int64_t> result;
    result.reserve(ids.size() + 2);
    result.push_back(0); // <s>
    for (int id : ids) {
        result.push_back(id == 0 ? 3 : id + 1); // 0 is <unk>
    }
    result.push_back(2); // </s>

    return result;
}

std::vector<float> Embedder::run_model(const std::vector<int64_t>& ids) {
    const int64_t seq_len = static_cast<int64_t>(ids.size());

    // CreateTensor
    std::vector<int64_t> input_ids(ids);
    std::vector<int64_t> attention_mask(seq_len, 1); 
    std::vector<int64_t> token_type_ids(seq_len, 0); 

    std::array<int64_t, 2> shape{1, seq_len};
    auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

    Ort::Value inputs[] = {
        Ort::Value::CreateTensor<int64_t>(memory_info, input_ids.data(), input_ids.size(), shape.data(), shape.size()),
        Ort::Value::CreateTensor<int64_t>(memory_info, attention_mask.data(), attention_mask.size(), shape.data(), shape.size()),
        Ort::Value::CreateTensor<int64_t>(memory_info, token_type_ids.data(), token_type_ids.size(), shape.data(), shape.size()),
    };
    const char* input_names[]  = {"input_ids", "attention_mask", "token_type_ids"};
    const char* output_names[] = {"last_hidden_state"};

    auto outputs = session.Run(Ort::RunOptions{nullptr}, input_names, inputs, 3, output_names, 1);
    const float* res = outputs[0].GetTensorData<float>();
    return std::vector<float>(res, res + seq_len * dim);
}

std::vector<float> Embedder::mean_pool(const std::vector<float>& token_vectors, size_t rows) const {
    std::vector<float> result(dim, 0.0f);
    for (size_t row = 0; row < rows; ++row) {
        for (size_t col = 0; col < dim; ++col) {
            result[col] += token_vectors[row * dim + col];
        }
    }

    std::transform(result.begin(), result.end(), result.begin(), [rows](float x){
        return x / static_cast<float>(rows);
    });

    return result;
}

void Embedder::normalize(std::vector<float>& v) const {
    float magnitude = sqrt(std::inner_product(v.begin(), v.end(), v.begin(), 0.0f));
    std::transform(v.begin(), v.end(), v.begin(), [magnitude](float x){
        return x / magnitude;
    });
}

std::vector<float> Embedder::embed(const std::string& text) {
    // Tokenize -> run the model
    auto tokenized_text = tokenize(text);
    auto token_vectors = run_model(tokenized_text);

    // Adjust data
    auto result = mean_pool(token_vectors, tokenized_text.size());
    normalize(result);
    return result;
}

} // namespace semsearch
