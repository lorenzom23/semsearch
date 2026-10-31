# Bench executable
add_executable(benchmark_search
    benchmark.cpp
)

target_link_libraries(benchmark_search
    PRIVATE
        semsearch_lib
)