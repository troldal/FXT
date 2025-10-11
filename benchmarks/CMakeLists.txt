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

# =============================================================================
# Compile-time benchmarks for tuple vs flat_tuple
# =============================================================================
if(UNIX AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # Helper function to create compile-time benchmark target
    function(add_compile_time_benchmark TARGET_NAME SOURCE_FILE)
        add_custom_target(${TARGET_NAME}
            COMMAND ${CMAKE_COMMAND} -E echo "Compiling ${SOURCE_FILE}..."
            COMMAND ${CMAKE_COMMAND} -E time ${CMAKE_CXX_COMPILER}
                -std=c++23
                -O2
                -I${CMAKE_SOURCE_DIR}/include
                -I${CMAKE_BINARY_DIR}/_deps/expected-src/include
                -I${CMAKE_BINARY_DIR}/_deps/optional-src/include
                -c ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE}
                -o ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}.o
            COMMAND ${CMAKE_COMMAND} -E echo "Finished compiling ${SOURCE_FILE}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Measuring compile time for ${SOURCE_FILE}"
            VERBATIM
        )
    endfunction()

    # Create compile-time benchmark targets
    add_compile_time_benchmark(compile_time_tuple CompileTimeTuple.cpp)
    add_compile_time_benchmark(compile_time_flat_tuple CompileTimeFlatTuple.cpp)

    # Create a target that runs both and compares
    add_custom_target(compile_time_comparison
        COMMAND ${CMAKE_COMMAND} -E echo "============================================"
        COMMAND ${CMAKE_COMMAND} -E echo "Compile-time comparison: fxt::tuple vs fxt::flat_tuple"
        COMMAND ${CMAKE_COMMAND} -E echo "============================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Testing fxt::tuple:"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compile_time_tuple
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "Testing fxt::flat_tuple:"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compile_time_flat_tuple
        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "============================================"
        COMMAND ${CMAKE_COMMAND} -E echo "Comparison complete!"
        COMMAND ${CMAKE_COMMAND} -E echo "============================================"
        COMMENT "Running compile-time comparison"
        VERBATIM
    )

    message(STATUS "Compile-time benchmarks enabled. Run 'make compile_time_comparison' or 'cmake --build . --target compile_time_comparison'")
endif()
