# Fetch Catch2
include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.16.0
)
FetchContent_MakeAvailable(Catch2)

# Test executable
add_executable(test_search
    test_search.cpp
)

target_link_libraries(test_search
    PRIVATE
        semsearch_lib
        Catch2::Catch2WithMain
)

# Discover the cast_case in test_search
list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
include(Catch)
catch_discover_tests(test_search WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})