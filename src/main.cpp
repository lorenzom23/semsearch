#include <cstdio>

#include "indexer.h"

int main() {
    semsearch::index_directory("data", "data/index.bin", "data/meta.json");
    std::puts("semsearch: ok");
    return 0;
}
