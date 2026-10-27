#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "index.h"
#include "search.h"

TEST_CASE("Search returns the query vector itself as best match", "[search]") {
    // Generates data
    semsearch::generateRandomIndex("data/test_index.bin", 10, 8);
    semsearch::Index idx("data/test_index.bin");

    // Gets the first array as query
    const float* query = idx.vector(0);
    REQUIRE(nullptr != query);

    // Search the best result
    auto results = idx.search(query, 3); 
    REQUIRE(3 == results.size());
    REQUIRE(0 == results[0].id); 
    CHECK(results[0].score == Catch::Approx(1.0f).margin(1e-4));
}
