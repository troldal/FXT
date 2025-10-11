//
// Demo: fxt::tuple_element and fxt::tuple_element_t
//
// This demo shows how to use fxt::tuple_element and fxt::tuple_element_t
// to extract type information from fxt::tuple and fxt::flat_tuple at compile time.
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <type_traits>
#include <typeinfo>

// Helper function to print type names (simplified)
template<typename T>
std::string type_name() {
    if constexpr (std::is_same_v<T, int>) return "int";
    else if constexpr (std::is_same_v<T, double>) return "double";
    else if constexpr (std::is_same_v<T, float>) return "float";
    else if constexpr (std::is_same_v<T, bool>) return "bool";
    else if constexpr (std::is_same_v<T, char>) return "char";
    else if constexpr (std::is_same_v<T, std::string>) return "std::string";
    else if constexpr (std::is_same_v<T, const int>) return "const int";
    else if constexpr (std::is_same_v<T, const double>) return "const double";
    else if constexpr (std::is_same_v<T, const std::string>) return "const std::string";
    else return "unknown";
}

// ============================================================================
// Example 1: Basic Type Extraction with fxt::tuple
// ============================================================================

void example_tuple_basic_extraction() {
    std::cout << "Example 1: Basic Type Extraction with fxt::tuple\n";
    std::cout << "=================================================\n\n";

    using MyTuple = fxt::tuple<int, double, std::string>;

    std::cout << "   Tuple type: fxt::tuple<int, double, std::string>\n\n";

    // Extract types using tuple_element_t
    using Type0 = fxt::tuple_element_t<0, MyTuple>;
    using Type1 = fxt::tuple_element_t<1, MyTuple>;
    using Type2 = fxt::tuple_element_t<2, MyTuple>;

    std::cout << "   Element 0 type: " << type_name<Type0>() << "\n";
    std::cout << "   Element 1 type: " << type_name<Type1>() << "\n";
    std::cout << "   Element 2 type: " << type_name<Type2>() << "\n\n";

    // Verify types at compile time
    static_assert(std::is_same_v<Type0, int>, "Element 0 should be int");
    static_assert(std::is_same_v<Type1, double>, "Element 1 should be double");
    static_assert(std::is_same_v<Type2, std::string>, "Element 2 should be std::string");

    std::cout << "   ✓ All type assertions passed\n\n";
}

// ============================================================================
// Example 2: Using tuple_element with Actual Tuples
// ============================================================================

void example_tuple_with_values() {
    std::cout << "Example 2: Using tuple_element with Actual Tuples\n";
    std::cout << "==================================================\n\n";

    // Create a tuple with actual values
    fxt::tuple<int, double, std::string> myTuple{42, 3.14159, "Hello"};

    std::cout << "   Created tuple with values: (42, 3.14159, \"Hello\")\n\n";

    // Use tuple_element_t to declare variables of the correct type
    fxt::tuple_element_t<0, decltype(myTuple)> firstValue = fxt::get<0>(myTuple);
    fxt::tuple_element_t<1, decltype(myTuple)> secondValue = fxt::get<1>(myTuple);
    fxt::tuple_element_t<2, decltype(myTuple)> thirdValue = fxt::get<2>(myTuple);

    std::cout << "   Extracted values using tuple_element_t:\n";
    std::cout << "   - First value (int): " << firstValue << "\n";
    std::cout << "   - Second value (double): " << secondValue << "\n";
    std::cout << "   - Third value (std::string): " << thirdValue << "\n\n";
}

// ============================================================================
// Example 3: Basic Type Extraction with fxt::flat_tuple
// ============================================================================

void example_flat_tuple_basic_extraction() {
    std::cout << "Example 3: Basic Type Extraction with fxt::flat_tuple\n";
    std::cout << "======================================================\n\n";

    using MyFlatTuple = fxt::flat_tuple<double, int, bool>;

    std::cout << "   Flat tuple type: fxt::flat_tuple<double, int, bool>\n\n";

    // Extract types using tuple_element_t
    using Type0 = fxt::tuple_element_t<0, MyFlatTuple>;
    using Type1 = fxt::tuple_element_t<1, MyFlatTuple>;
    using Type2 = fxt::tuple_element_t<2, MyFlatTuple>;

    std::cout << "   Element 0 type: " << type_name<Type0>() << "\n";
    std::cout << "   Element 1 type: " << type_name<Type1>() << "\n";
    std::cout << "   Element 2 type: " << type_name<Type2>() << "\n\n";

    // Verify types at compile time
    static_assert(std::is_same_v<Type0, double>, "Element should be double");
    static_assert(std::is_same_v<Type1, int>, "Element should be int");
    static_assert(std::is_same_v<Type2, bool>, "Element should be bool");

    std::cout << "   ✓ All type assertions passed\n\n";
}

// ============================================================================
// Example 4: Using tuple_element with Actual flat_tuples
// ============================================================================

void example_flat_tuple_with_values() {
    std::cout << "Example 4: Using tuple_element with Actual flat_tuples\n";
    std::cout << "=======================================================\n\n";

    // Create a flat_tuple with actual values
    fxt::flat_tuple<int, int, int> myFlatTuple{10, 20, 30};

    std::cout << "   Created flat_tuple with values: (10, 20, 30)\n\n";

    // Use tuple_element_t to declare variables of the correct type
    fxt::tuple_element_t<0, decltype(myFlatTuple)> firstValue = fxt::get<0>(myFlatTuple);
    fxt::tuple_element_t<1, decltype(myFlatTuple)> secondValue = fxt::get<1>(myFlatTuple);
    fxt::tuple_element_t<2, decltype(myFlatTuple)> thirdValue = fxt::get<2>(myFlatTuple);

    std::cout << "   Extracted values using tuple_element_t:\n";
    std::cout << "   - First value (int): " << firstValue << "\n";
    std::cout << "   - Second value (int): " << secondValue << "\n";
    std::cout << "   - Third value (int): " << thirdValue << "\n\n";
}

// ============================================================================
// Example 5: Working with CV-Qualified Tuples
// ============================================================================

void example_cv_qualified_tuples() {
    std::cout << "Example 5: Working with CV-Qualified Tuples\n";
    std::cout << "============================================\n\n";

    // Const tuple
    using ConstTuple = const fxt::tuple<int, double>;
    using ConstType0 = fxt::tuple_element_t<0, ConstTuple>;
    using ConstType1 = fxt::tuple_element_t<1, ConstTuple>;

    std::cout << "   const fxt::tuple<int, double>:\n";
    std::cout << "   - Element 0 type: " << type_name<ConstType0>() << "\n";
    std::cout << "   - Element 1 type: " << type_name<ConstType1>() << "\n\n";

    static_assert(std::is_same_v<ConstType0, const int>, "Should be const int");
    static_assert(std::is_same_v<ConstType1, const double>, "Should be const double");

    // Const flat_tuple
    using ConstFlatTuple = const fxt::flat_tuple<double, double>;
    using ConstFlatType0 = fxt::tuple_element_t<0, ConstFlatTuple>;
    using ConstFlatType1 = fxt::tuple_element_t<1, ConstFlatTuple>;

    std::cout << "   const fxt::flat_tuple<double, double>:\n";
    std::cout << "   - Element 0 type: " << type_name<ConstFlatType0>() << "\n";
    std::cout << "   - Element 1 type: " << type_name<ConstFlatType1>() << "\n\n";

    static_assert(std::is_same_v<ConstFlatType0, const double>, "Should be const double");
    static_assert(std::is_same_v<ConstFlatType1, const double>, "Should be const double");

    std::cout << "   ✓ CV-qualifiers are preserved correctly\n\n";
}

// ============================================================================
// Example 6: Generic Function Using tuple_element
// ============================================================================

template<typename Tuple>
void print_first_element_type() {
    using FirstType = fxt::tuple_element_t<0, Tuple>;
    std::cout << "   First element type: " << type_name<FirstType>() << "\n";
}

void example_generic_function() {
    std::cout << "Example 6: Generic Function Using tuple_element\n";
    std::cout << "================================================\n\n";

    std::cout << "   Using generic function to extract first element type:\n\n";

    std::cout << "   For fxt::tuple<int, double, std::string>:\n";
    print_first_element_type<fxt::tuple<int, double, std::string>>();

    std::cout << "\n   For fxt::flat_tuple<float, float, float>:\n";
    print_first_element_type<fxt::flat_tuple<float, float, float>>();

    std::cout << "\n   For fxt::tuple<bool, char, int>:\n";
    print_first_element_type<fxt::tuple<bool, char, int>>();

    std::cout << "\n";
}

// ============================================================================
// Example 7: Using Both tuple_element and tuple_element_t
// ============================================================================

void example_both_forms() {
    std::cout << "Example 7: Using Both tuple_element and tuple_element_t\n";
    std::cout << "========================================================\n\n";

    using MyTuple = fxt::tuple<int, double, std::string>;

    std::cout << "   Both forms extract the same type:\n\n";

    // Using the full form
    using Type1_Full = typename fxt::tuple_element<1, MyTuple>::type;

    // Using the convenience alias
    using Type1_Alias = fxt::tuple_element_t<1, MyTuple>;

    std::cout << "   Using typename fxt::tuple_element<1, MyTuple>::type: "
              << type_name<Type1_Full>() << "\n";
    std::cout << "   Using fxt::tuple_element_t<1, MyTuple>: "
              << type_name<Type1_Alias>() << "\n\n";

    static_assert(std::is_same_v<Type1_Full, Type1_Alias>,
                  "Both forms should produce the same type");

    std::cout << "   ✓ Both forms are equivalent\n";
    std::cout << "   → Prefer tuple_element_t for cleaner code\n\n";
}

// ============================================================================
// Example 8: Practical Use Case - Type-Safe Element Processing
// ============================================================================

template<typename Tuple, std::size_t I>
void process_element(const Tuple& t) {
    using ElementType = fxt::tuple_element_t<I, Tuple>;

    if constexpr (std::is_same_v<ElementType, int>) {
        std::cout << "   Processing integer: " << fxt::get<I>(t) << "\n";
    } else if constexpr (std::is_same_v<ElementType, double>) {
        std::cout << "   Processing double: " << fxt::get<I>(t) << "\n";
    } else if constexpr (std::is_same_v<ElementType, std::string>) {
        std::cout << "   Processing string: \"" << fxt::get<I>(t) << "\"\n";
    }
}

void example_practical_use_case() {
    std::cout << "Example 8: Practical Use Case - Type-Safe Element Processing\n";
    std::cout << "=============================================================\n\n";

    fxt::tuple<int, double, std::string> data{100, 99.9, "complete"};

    std::cout << "   Processing tuple elements based on their types:\n\n";

    process_element<decltype(data), 0>(data);
    process_element<decltype(data), 1>(data);
    process_element<decltype(data), 2>(data);

    std::cout << "\n   Using flat_tuple:\n\n";

    fxt::flat_tuple<double, double, double> coords{1.5, 2.5, 3.5};

    std::cout << "   Processing coordinate components:\n";
    process_element<decltype(coords), 0>(coords);
    process_element<decltype(coords), 1>(coords);
    process_element<decltype(coords), 2>(coords);

    std::cout << "\n";
}

// ============================================================================
// Example 9: Compile-Time Type Checks
// ============================================================================

void example_compile_time_checks() {
    std::cout << "Example 9: Compile-Time Type Checks\n";
    std::cout << "====================================\n\n";

    using MixedTuple = fxt::tuple<int, float, double, bool, char>;

    std::cout << "   Verifying types at compile time for:\n";
    std::cout << "   fxt::tuple<int, float, double, bool, char>\n\n";

    // All these checks happen at compile time - no runtime cost!
    static_assert(std::is_same_v<fxt::tuple_element_t<0, MixedTuple>, int>);
    static_assert(std::is_same_v<fxt::tuple_element_t<1, MixedTuple>, float>);
    static_assert(std::is_same_v<fxt::tuple_element_t<2, MixedTuple>, double>);
    static_assert(std::is_same_v<fxt::tuple_element_t<3, MixedTuple>, bool>);
    static_assert(std::is_same_v<fxt::tuple_element_t<4, MixedTuple>, char>);

    std::cout << "   ✓ All compile-time type checks passed\n";
    std::cout << "   → Zero runtime overhead for type verification\n\n";
}

// ============================================================================
// Example 10: Compatibility with std::tuple_element
// ============================================================================

void example_std_compatibility() {
    std::cout << "Example 10: Compatibility with std::tuple_element\n";
    std::cout << "==================================================\n\n";

    using FxtTuple = fxt::tuple<int, double, std::string>;
    using StdTuple = std::tuple<int, double, std::string>;

    std::cout << "   fxt::tuple_element works just like std::tuple_element\n";
    std::cout << "   for fxt::tuple (which is an alias for std::tuple)\n\n";

    using FxtType0 = fxt::tuple_element_t<0, FxtTuple>;
    using StdType0 = std::tuple_element_t<0, StdTuple>;

    using FxtType1 = fxt::tuple_element_t<1, FxtTuple>;
    using StdType1 = std::tuple_element_t<1, StdTuple>;

    using FxtType2 = fxt::tuple_element_t<2, FxtTuple>;
    using StdType2 = std::tuple_element_t<2, StdTuple>;

    std::cout << "   Element 0: fxt -> " << type_name<FxtType0>()
              << ", std -> " << type_name<StdType0>() << "\n";
    std::cout << "   Element 1: fxt -> " << type_name<FxtType1>()
              << ", std -> " << type_name<StdType1>() << "\n";
    std::cout << "   Element 2: fxt -> " << type_name<FxtType2>()
              << ", std -> " << type_name<StdType2>() << "\n\n";

    static_assert(std::is_same_v<FxtType0, StdType0>);
    static_assert(std::is_same_v<FxtType1, StdType1>);
    static_assert(std::is_same_v<FxtType2, StdType2>);

    std::cout << "   ✓ fxt::tuple_element is fully compatible with std::tuple_element\n\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n=== fxt::tuple_element and fxt::tuple_element_t Demo ===\n\n";

    example_tuple_basic_extraction();
    example_tuple_with_values();
    example_flat_tuple_basic_extraction();
    example_flat_tuple_with_values();
    example_cv_qualified_tuples();
    example_generic_function();
    example_both_forms();
    example_practical_use_case();
    example_compile_time_checks();
    example_std_compatibility();

    std::cout << "=== Demo Complete ===\n\n";
    return 0;
}

