CPMAddPackage(
        NAME benchmark
        GITHUB_REPOSITORY google/benchmark
        VERSION 1.9.4
        OPTIONS
        "BENCHMARK_ENABLE_TESTING OFF"
)

if (benchmark_ADDED)
    # patch benchmark target
    set_target_properties(benchmark PROPERTIES CXX_STANDARD 17)
endif()

# Add benchmark executable
add_executable(TupleBenchmark TupleBenchmark.cpp)
target_link_libraries(TupleBenchmark PRIVATE benchmark::benchmark fxt)
target_compile_features(TupleBenchmark PRIVATE cxx_std_20)
