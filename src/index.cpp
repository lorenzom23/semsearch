#include "index.h"

#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>

namespace semsearch {

void generateRandomIndex(const std::string& path, uint32_t count, uint32_t dim) {
    std::filesystem::path p(path);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }

    Header h{};
    std::memcpy(h.magic, "SEM1", 4);
    h.version = 1;
    h.dim = dim;
    h.count = count;

    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(&h), sizeof(h));

    std::mt19937 rng(42);
    std::normal_distribution<double> dist(0.0, 1.0);

    std::vector<double> vec(dim);
    for (uint32_t i = 0; i < count; ++i) {
        double norm_sq = 0.0;
        for (uint32_t j = 0; j < dim; ++j) {
            vec[j] = dist(rng);
            norm_sq += vec[j] * vec[j];
        }
        double norm = std::sqrt(norm_sq);
        for (uint32_t j = 0; j < dim; ++j) {
            vec[j] /= norm;
        }
        std::vector<float> vec_f(vec.begin(), vec.end());
        out.write(reinterpret_cast<const char*>(vec_f.data()), dim * sizeof(float));
    }
}

} // namespace semsearch
