cmake_minimum_required(VERSION 3.20)
project(semsearch CXX)

# C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build Release
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

include(FetchContent)

# ONNX Runtime
FetchContent_Declare(
    onnxruntime
    URL https://github.com/microsoft/onnxruntime/releases/download/v1.31.1/onnxruntime-linux-x64-1.31.1.tgz
)
FetchContent_MakeAvailable(onnxruntime)
set(ONNXRUNTIME_ROOT ${onnxruntime_SOURCE_DIR})

# SentencePiece
FetchContent_Declare(
    sentencepiece
    GIT_REPOSITORY https://github.com/google/sentencepiece.git
    GIT_TAG        v0.2.2
)
FetchContent_MakeAvailable(sentencepiece)
set(CMAKE_BUILD_RPATH ${ONNXRUNTIME_ROOT}/lib)

# nlohmann/json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Library target (explicit sources, no file(GLOB))
add_library(semsearch_lib
    src/index.cpp
    src/search.cpp
    src/indexer.cpp
    src/embedder.cpp
)
target_include_directories(semsearch_lib PUBLIC
    src
    ${ONNXRUNTIME_ROOT}/include
    ${sentencepiece_SOURCE_DIR}/src
    ${sentencepiece_SOURCE_DIR}
)
target_link_libraries(semsearch_lib PUBLIC
    ${ONNXRUNTIME_ROOT}/lib/libonnxruntime.so
    sentencepiece-static
    nlohmann_json::nlohmann_json
)
target_compile_options(semsearch_lib PRIVATE -Wall -Wextra -Wpedantic)

# CLI executable
add_executable(semsearch src/main.cpp)
target_link_libraries(semsearch PRIVATE semsearch_lib)
target_compile_options(semsearch PRIVATE -Wall -Wextra -Wpedantic)

# Enable testing
enable_testing()
add_subdirectory(test)
add_subdirectory(benchmark)
