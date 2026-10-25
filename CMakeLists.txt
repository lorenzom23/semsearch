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

# Library target (explicit sources, no file(GLOB)).
add_library(semsearch_lib
    src/index.cpp
    src/search.cpp
)
target_include_directories(semsearch_lib PUBLIC src)
target_compile_options(semsearch_lib PRIVATE -Wall -Wextra -Wpedantic)

# CLI executable.
add_executable(semsearch src/main.cpp)
target_link_libraries(semsearch PRIVATE semsearch_lib)
target_compile_options(semsearch PRIVATE -Wall -Wextra -Wpedantic)
