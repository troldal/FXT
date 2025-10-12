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
if(UNIX)
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

    # Create compile-time benchmark targets for appending
    add_compile_time_benchmark(compile_time_append_tuple CompileTimeAppendTuple.cpp)
    add_compile_time_benchmark(compile_time_append_flat_tuple CompileTimeAppendFlatTuple.cpp)

    # Create compile-time benchmark targets for creation
    add_compile_time_benchmark(compile_time_creation_tuple CompileTimeCreationTuple.cpp)
    add_compile_time_benchmark(compile_time_creation_flat_tuple CompileTimeCreationFlatTuple.cpp)

    # Create compile-time benchmark targets for access
    add_compile_time_benchmark(compile_time_access_tuple CompileTimeAccessTuple.cpp)
    add_compile_time_benchmark(compile_time_access_flat_tuple CompileTimeAccessFlatTuple.cpp)

    # Create compile-time benchmark targets for transform
    add_compile_time_benchmark(compile_time_transform_tuple CompileTimeTransformTuple.cpp)
    add_compile_time_benchmark(compile_time_transform_flat_tuple CompileTimeTransformFlatTuple.cpp)

    # Create compile-time benchmark targets for select/take/drop
    add_compile_time_benchmark(compile_time_select_tuple CompileTimeSelectTuple.cpp)
    add_compile_time_benchmark(compile_time_select_flat_tuple CompileTimeSelectFlatTuple.cpp)

    # Create compile-time benchmark targets for apply
    add_compile_time_benchmark(compile_time_apply_tuple CompileTimeApplyTuple.cpp)
    add_compile_time_benchmark(compile_time_apply_flat_tuple CompileTimeApplyFlatTuple.cpp)

    # Create compile-time benchmark targets for combined operations
    add_compile_time_benchmark(compile_time_combined_tuple CompileTimeCombinedTuple.cpp)
    add_compile_time_benchmark(compile_time_combined_flat_tuple CompileTimeCombinedFlatTuple.cpp)

    # Helper function to run a pair of benchmarks and compare
    function(add_benchmark_comparison CATEGORY TUPLE_TARGET FLAT_TUPLE_TARGET)
        add_custom_target(compare_${CATEGORY}
            COMMAND ${CMAKE_COMMAND} -E echo "======================================================"
            COMMAND ${CMAKE_COMMAND} -E echo "Compile-time comparison: ${CATEGORY}"
            COMMAND ${CMAKE_COMMAND} -E echo "======================================================"
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Testing fxt::tuple:"
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${TUPLE_TARGET}
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMAND ${CMAKE_COMMAND} -E echo "Testing fxt::flat_tuple:"
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target ${FLAT_TUPLE_TARGET}
            COMMAND ${CMAKE_COMMAND} -E echo ""
            COMMENT "Running compile-time comparison for ${CATEGORY}"
            VERBATIM
        )
    endfunction()

    # Create comparison targets for each category
    add_benchmark_comparison(append compile_time_append_tuple compile_time_append_flat_tuple)
    add_benchmark_comparison(creation compile_time_creation_tuple compile_time_creation_flat_tuple)
    add_benchmark_comparison(access compile_time_access_tuple compile_time_access_flat_tuple)
    add_benchmark_comparison(transform compile_time_transform_tuple compile_time_transform_flat_tuple)
    add_benchmark_comparison(select compile_time_select_tuple compile_time_select_flat_tuple)
    add_benchmark_comparison(apply compile_time_apply_tuple compile_time_apply_flat_tuple)
    add_benchmark_comparison(combined compile_time_combined_tuple compile_time_combined_flat_tuple)

    # Create a target that runs all comparisons
    add_custom_target(compile_time_comparison_all
        COMMAND ${CMAKE_COMMAND} -E echo "======================================================"
        COMMAND ${CMAKE_COMMAND} -E echo "COMPREHENSIVE COMPILE-TIME COMPARISON"
        COMMAND ${CMAKE_COMMAND} -E echo "fxt::tuple vs fxt::flat_tuple"
        COMMAND ${CMAKE_COMMAND} -E echo "======================================================"
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "1. APPEND OPERATIONS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_append
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "2. CREATION PATTERNS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_creation
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "3. ELEMENT ACCESS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_access
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "4. TRANSFORM OPERATIONS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_transform
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "5. SELECT/TAKE/DROP OPERATIONS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_select
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "6. APPLY OPERATIONS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_apply
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} -E echo "7. COMBINED OPERATIONS"
        COMMAND ${CMAKE_COMMAND} -E echo "========================================"
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target compare_combined
        COMMAND ${CMAKE_COMMAND} -E echo ""

        COMMAND ${CMAKE_COMMAND} -E echo "======================================================"
        COMMAND ${CMAKE_COMMAND} -E echo "ALL COMPARISONS COMPLETE!"
        COMMAND ${CMAKE_COMMAND} -E echo "======================================================"
        COMMENT "Running all compile-time comparisons"
        VERBATIM
    )

    message(STATUS "Compile-time benchmarks enabled.")
    message(STATUS "  - Run individual comparisons: make compare_<category>")
    message(STATUS "    Categories: append, creation, access, transform, select, apply, combined")
    message(STATUS "  - Run all comparisons: make compile_time_comparison_all")
endif()
