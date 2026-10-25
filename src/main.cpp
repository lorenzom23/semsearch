#include <cstdio>

#include "index.h"

int main() {
    semsearch::generateRandomIndex("data/index.bin", 100000, 384);
    std::puts("semsearch: ok");
    return 0;
}
