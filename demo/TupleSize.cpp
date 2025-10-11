//
// Demo: fxt::tuple_size and fxt::tuple_size_v
//
// This demo shows how to use fxt::tuple_size and fxt::tuple_size_v to query
// the size of tuple types at compile time. These utilities work with both
// fxt::tuple (std::tuple) and fxt::flat_tuple.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <vector>

// ============================================================================
// Example 1: Basic Usage with fxt::tuple
// ============================================================================

void example_basic_fxt_tuple() {
    std::cout << "Example 1: Basic Usage with fxt::tuple\n";
    std::cout << "=======================================\n\n";

    // Query the size of various fxt::tuple types
    using Tuple1 = fxt::tuple<int>;
    using Tuple2 = fxt::tuple<int, std::string>;
    using Tuple3 = fxt::tuple<int, double, std::string>;
    using Tuple4 = fxt::tuple<int, int, int, int, int>;

    std::cout << "   fxt::tuple<int> size: "
              << fxt::tuple_size<Tuple1>::value << "\n";
    std::cout << "   fxt::tuple<int, std::string> size: "
              << fxt::tuple_size<Tuple2>::value << "\n";
    std::cout << "   fxt::tuple<int, double, std::string> size: "
              << fxt::tuple_size<Tuple3>::value << "\n";
    std::cout << "   fxt::tuple<int, int, int, int, int> size: "
              << fxt::tuple_size<Tuple4>::value << "\n\n";
}

// ============================================================================
// Example 2: Using fxt::tuple_size_v with fxt::tuple
// ============================================================================

void example_tuple_size_v_fxt_tuple() {
    std::cout << "Example 2: Using fxt::tuple_size_v with fxt::tuple\n";
    std::cout << "===================================================\n\n";

    // Using the convenient variable template
    std::cout << "   fxt::tuple_size_v<fxt::tuple<char>>: "
              << fxt::tuple_size_v<fxt::tuple<char>> << "\n";
    std::cout << "   fxt::tuple_size_v<fxt::tuple<int, double>>: "
              << fxt::tuple_size_v<fxt::tuple<int, double>> << "\n";
    std::cout << "   fxt::tuple_size_v<fxt::tuple<int, std::string, bool>>: "
              << fxt::tuple_size_v<fxt::tuple<int, std::string, bool>> << "\n";

    // Empty tuple
    std::cout << "   fxt::tuple_size_v<fxt::tuple<>>: "
              << fxt::tuple_size_v<fxt::tuple<>> << "\n\n";
}

// ============================================================================
// Example 3: Basic Usage with fxt::flat_tuple
// ============================================================================

void example_basic_flat_tuple() {
    std::cout << "Example 3: Basic Usage with fxt::flat_tuple\n";
    std::cout << "============================================\n\n";

    // Query the size of various fxt::flat_tuple types
    using FlatTuple1 = fxt::flat_tuple<int>;
    using FlatTuple2 = fxt::flat_tuple<int, std::string>;
    using FlatTuple3 = fxt::flat_tuple<int, double, std::string>;
    using FlatTuple4 = fxt::flat_tuple<int, int, int, int, int>;

    std::cout << "   fxt::flat_tuple<int> size: "
              << fxt::tuple_size<FlatTuple1>::value << "\n";
    std::cout << "   fxt::flat_tuple<int, std::string> size: "
              << fxt::tuple_size<FlatTuple2>::value << "\n";
    std::cout << "   fxt::flat_tuple<int, double, std::string> size: "
              << fxt::tuple_size<FlatTuple3>::value << "\n";
    std::cout << "   fxt::flat_tuple<int, int, int, int, int> size: "
              << fxt::tuple_size<FlatTuple4>::value << "\n\n";
}

// ============================================================================
// Example 4: Using fxt::tuple_size_v with fxt::flat_tuple
// ============================================================================

void example_tuple_size_v_flat_tuple() {
    std::cout << "Example 4: Using fxt::tuple_size_v with fxt::flat_tuple\n";
    std::cout << "========================================================\n\n";

    // Using the convenient variable template
    std::cout << "   fxt::tuple_size_v<fxt::flat_tuple<char>>: "
              << fxt::tuple_size_v<fxt::flat_tuple<char>> << "\n";
    std::cout << "   fxt::tuple_size_v<fxt::flat_tuple<int, double>>: "
              << fxt::tuple_size_v<fxt::flat_tuple<int, double>> << "\n";
    std::cout << "   fxt::tuple_size_v<fxt::flat_tuple<int, std::string, bool>>: "
              << fxt::tuple_size_v<fxt::flat_tuple<int, std::string, bool>> << "\n";

    // Empty tuple
    std::cout << "   fxt::tuple_size_v<fxt::flat_tuple<>>: "
              << fxt::tuple_size_v<fxt::flat_tuple<>> << "\n\n";
}

// ============================================================================
// Example 5: Compile-Time Computation
// ============================================================================

void example_compile_time() {
    std::cout << "Example 5: Compile-Time Computation\n";
    std::cout << "====================================\n\n";

    // All of these are computed at compile time
    constexpr std::size_t size1 = fxt::tuple_size_v<fxt::tuple<int, int>>;
    constexpr std::size_t size2 = fxt::tuple_size_v<fxt::flat_tuple<int, int, int>>;

    static_assert(size1 == 2, "fxt::tuple size should be 2");
    static_assert(size2 == 3, "fxt::flat_tuple size should be 3");

    std::cout << "   constexpr size1 (fxt::tuple<int, int>): " << size1 << "\n";
    std::cout << "   constexpr size2 (fxt::flat_tuple<int, int, int>): " << size2 << "\n";
    std::cout << "   All static_asserts passed!\n\n";
}

// ============================================================================
// Example 6: Using with const and volatile qualifiers
// ============================================================================

void example_cv_qualifiers() {
    std::cout << "Example 6: Using with const and volatile qualifiers\n";
    std::cout << "====================================================\n\n";

    using Tuple = fxt::tuple<int, std::string, double>;
    using FlatTuple = fxt::flat_tuple<int, std::string, double>;

    std::cout << "   fxt::tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<Tuple> << "\n";
    std::cout << "   const fxt::tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<const Tuple> << "\n";
    std::cout << "   volatile fxt::tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<volatile Tuple> << "\n";
    std::cout << "   const volatile fxt::tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<const volatile Tuple> << "\n\n";

    std::cout << "   fxt::flat_tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<FlatTuple> << "\n";
    std::cout << "   const fxt::flat_tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<const FlatTuple> << "\n";
    std::cout << "   volatile fxt::flat_tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<volatile FlatTuple> << "\n";
    std::cout << "   const volatile fxt::flat_tuple<int, std::string, double> size: "
              << fxt::tuple_size_v<const volatile FlatTuple> << "\n\n";
}

// ============================================================================
// Example 7: Template Metaprogramming - Generic Function
// ============================================================================

template<typename Tuple>
void print_tuple_info() {
    constexpr std::size_t size = fxt::tuple_size_v<Tuple>;
    std::cout << "   Tuple type has " << size << " element(s)\n";
}

void example_generic_function() {
    std::cout << "Example 7: Template Metaprogramming - Generic Function\n";
    std::cout << "=======================================================\n\n";

    std::cout << "   Analyzing fxt::tuple<int, std::string>:\n";
    print_tuple_info<fxt::tuple<int, std::string>>();

    std::cout << "   Analyzing fxt::flat_tuple<double, bool, char>:\n";
    print_tuple_info<fxt::flat_tuple<double, bool, char>>();

    std::cout << "   Analyzing fxt::tuple<>:\n";
    print_tuple_info<fxt::tuple<>>();

    std::cout << "\n";
}

// ============================================================================
// Example 8: Conditional Compilation Based on Size
// ============================================================================

template<typename Tuple>
void process_tuple() {
    constexpr std::size_t size = fxt::tuple_size_v<Tuple>;

    if constexpr (size == 0) {
        std::cout << "      Empty tuple - nothing to process\n";
    } else if constexpr (size == 1) {
        std::cout << "      Single element tuple\n";
    } else if constexpr (size == 2) {
        std::cout << "      Pair tuple\n";
    } else {
        std::cout << "      Tuple with " << size << " elements\n";
    }
}

void example_conditional_compilation() {
    std::cout << "Example 8: Conditional Compilation Based on Size\n";
    std::cout << "=================================================\n\n";

    std::cout << "   Processing fxt::tuple<>:\n";
    process_tuple<fxt::tuple<>>();

    std::cout << "   Processing fxt::flat_tuple<int>:\n";
    process_tuple<fxt::flat_tuple<int>>();

    std::cout << "   Processing fxt::tuple<int, std::string>:\n";
    process_tuple<fxt::tuple<int, std::string>>();

    std::cout << "   Processing fxt::flat_tuple<int, double, std::string, bool>:\n";
    process_tuple<fxt::flat_tuple<int, double, std::string, bool>>();

    std::cout << "\n";
}

// ============================================================================
// Example 9: Comparison Between fxt::tuple and fxt::flat_tuple
// ============================================================================

void example_comparison() {
    std::cout << "Example 9: Comparison Between fxt::tuple and fxt::flat_tuple\n";
    std::cout << "=============================================================\n\n";

    using StdTupleType = fxt::tuple<int, std::string, double, bool>;
    using FlatTupleType = fxt::flat_tuple<int, std::string, double, bool>;

    constexpr std::size_t std_size = fxt::tuple_size_v<StdTupleType>;
    constexpr std::size_t flat_size = fxt::tuple_size_v<FlatTupleType>;

    std::cout << "   fxt::tuple<int, std::string, double, bool> size: " << std_size << "\n";
    std::cout << "   fxt::flat_tuple<int, std::string, double, bool> size: " << flat_size << "\n";
    std::cout << "   Sizes are equal: " << std::boolalpha << (std_size == flat_size) << "\n\n";

    static_assert(std_size == flat_size, "Both tuple types should have the same size");
    std::cout << "   Static assertion passed: both have size " << std_size << "\n\n";
}

// ============================================================================
// Example 10: Practical Use Case - Array Initialization
// ============================================================================

template<typename Tuple>
auto create_array_from_tuple_size() {
    constexpr std::size_t size = fxt::tuple_size_v<Tuple>;
    std::array<int, size> arr{};

    // Initialize array with indices
    for (std::size_t i = 0; i < size; ++i) {
        arr[i] = static_cast<int>(i);
    }

    return arr;
}

void example_practical_use() {
    std::cout << "Example 10: Practical Use Case - Array Initialization\n";
    std::cout << "======================================================\n\n";

    auto arr1 = create_array_from_tuple_size<fxt::tuple<int, int, int>>();
    std::cout << "   Array from fxt::tuple<int, int, int>: [";
    for (std::size_t i = 0; i < arr1.size(); ++i) {
        std::cout << arr1[i] << (i < arr1.size() - 1 ? ", " : "");
    }
    std::cout << "]\n";

    auto arr2 = create_array_from_tuple_size<fxt::flat_tuple<double, double, double, double, double>>();
    std::cout << "   Array from fxt::flat_tuple<double, double, double, double, double>: [";
    for (std::size_t i = 0; i < arr2.size(); ++i) {
        std::cout << arr2[i] << (i < arr2.size() - 1 ? ", " : "");
    }
    std::cout << "]\n\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  fxt::tuple_size Demonstration\n";
    std::cout << "========================================\n\n";

    example_basic_fxt_tuple();
    example_tuple_size_v_fxt_tuple();
    example_basic_flat_tuple();
    example_tuple_size_v_flat_tuple();
    example_compile_time();
    example_cv_qualifiers();
    example_generic_function();
    example_conditional_compilation();
    example_comparison();
    example_practical_use();

    std::cout << "========================================\n";
    std::cout << "  All examples completed successfully!\n";
    std::cout << "========================================\n\n";

    return 0;
}

