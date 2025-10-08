//
// Created by AI Assistant on 07/10/2025.
//

#include <fxt.hpp>
#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Example 1: Tag Dispatch Pattern
// ============================================================================

// Different implementations based on type tags
void serialize(fxt::Type<int>, int value) {
    std::cout << "   Serializing int: " << value << " -> binary format\n";
}

void serialize(fxt::Type<std::string>, const std::string& value) {
    std::cout << "   Serializing string: \"" << value << "\" -> UTF-8 format\n";
}

void serialize(fxt::Type<double>, double value) {
    std::cout << "   Serializing double: " << value << " -> IEEE 754 format\n";
}

// Generic serialize function that uses tag dispatch
template<typename T>
void serialize_value(const T& value) {
    serialize(fxt::Type<T>{}, value);
}

// ============================================================================
// Example 2: Factory Pattern with Type Tags
// ============================================================================

// Factory function that creates objects based on type tags
template<typename T>
std::unique_ptr<T> create_object(fxt::Type<T>) {
    std::cout << "   Creating generic object\n";
    return std::make_unique<T>();
}

// Specialization for string (implicit conversion from type tag)
std::unique_ptr<std::string> create_object(fxt::Type<std::string>) {
    std::cout << "   Creating string with default content\n";
    return std::make_unique<std::string>("Default String");
}

// Specialization for vector<int>
std::unique_ptr<std::vector<int>> create_object(fxt::Type<std::vector<int>>) {
    std::cout << "   Creating vector with initial values\n";
    auto vec = std::make_unique<std::vector<int>>();
    vec->push_back(1);
    vec->push_back(2);
    vec->push_back(3);
    return vec;
}

// ============================================================================
// Example 3: Type Identity with Value Storage
// ============================================================================

// Process data with different strategies based on type tag
template<typename Strategy, typename Data>
void process_with_strategy(fxt::Type<Strategy, Data> config) {
    std::cout << "   Strategy type: " << typeid(Strategy).name() << "\n";
    std::cout << "   Config value: " << config.value() << "\n";
    std::cout << "   Processing with " << typeid(Strategy).name() << " strategy\n";
}

struct FastStrategy {};
struct AccurateStrategy {};
struct BalancedStrategy {};

// ============================================================================
// Example 4: Compile-time Type Manipulation
// ============================================================================

// Extract type information at compile time
template<typename T>
void print_type_info(T) {
    if constexpr (fxt::is_type_v<T>) {
        using ExtractedType = fxt::type_t<T>;
        std::cout << "   This is a fxt::Type wrapper\n";
        std::cout << "   Wrapped type size: " << sizeof(ExtractedType) << " bytes\n";
        std::cout << "   Type name: " << typeid(ExtractedType).name() << "\n";
    } else {
        std::cout << "   This is NOT a fxt::Type wrapper\n";
        std::cout << "   Type size: " << sizeof(T) << " bytes\n";
    }
}

// ============================================================================
// Example 5: Using Helper Functions
// ============================================================================

template<typename T>
void demonstrate_helper_functions() {
    std::cout << "   Creating type tag with make_type<" << typeid(T).name() << ">()\n";

    auto type_tag = fxt::make_type<T>();
    static_assert(fxt::is_type_v<decltype(type_tag)>, "Should be a Type wrapper");

    std::cout << "   Type tag created successfully\n";
    std::cout << "   Is it a Type? " << std::boolalpha << fxt::is_type_v<decltype(type_tag)> << "\n";
}

// ============================================================================
// Example 6: Type Tag for Overload Resolution
// ============================================================================

// Different algorithms based on container type
template<typename Container>
    requires std::is_same_v<Container, std::vector<int>>
void sort_container(Container& container, fxt::Type<std::vector<int>>) {
    std::cout << "   Using optimized vector<int> sort\n";
    std::sort(container.begin(), container.end());
}

template<typename Container>
    requires (!std::is_same_v<Container, std::vector<int>>)
void sort_container(Container& container, fxt::Type<Container>) {
    std::cout << "   Using generic sort\n";
    std::sort(container.begin(), container.end());
}

// ============================================================================
// Example 7: Comparison and Value Semantics
// ============================================================================

void demonstrate_comparison() {
    std::cout << "   Creating two Type wrappers with values\n";
    fxt::Type<std::string, int> config1(42);
    fxt::Type<std::string, int> config2(42);
    fxt::Type<std::string, int> config3(100);

    std::cout << "   config1 == config2: " << (config1 == config2) << "\n";
    std::cout << "   config1 == config3: " << (config1 == config3) << "\n";
    std::cout << "   config1 < config3: " << (config1 < config3) << "\n";
}

// ============================================================================
// Example 8: Move Semantics and Reference Qualifiers
// ============================================================================

void demonstrate_move_semantics() {
    std::cout << "   Creating Type with movable value\n";
    fxt::Type<int, std::string> wrapper(std::string("Hello, World!"));

    // Get reference
    std::cout << "   Getting reference: " << wrapper.get() << "\n";

    // Move value out
    std::cout << "   Moving value out...\n";
    std::string moved = std::move(wrapper).value();
    std::cout << "   Moved value: " << moved << "\n";
}

// ============================================================================
// Main Demo
// ============================================================================

int main() {
    std::cout << "=== fxt::Type Demo ===\n\n";

    // Example 1: Tag Dispatch Pattern
    std::cout << "1. Tag Dispatch Pattern:\n";
    std::cout << "   Using type tags to select different implementations\n";
    serialize_value(42);
    serialize_value(std::string("Hello"));
    serialize_value(3.14159);
    std::cout << "\n";

    // Example 2: Factory Pattern
    std::cout << "2. Factory Pattern with Type Tags:\n";
    auto int_obj = create_object(fxt::Type<int>{});
    auto str_obj = create_object(fxt::Type<std::string>{});
    auto vec_obj = create_object(fxt::Type<std::vector<int>>{});
    std::cout << "   String content: \"" << *str_obj << "\"\n";
    std::cout << "   Vector size: " << vec_obj->size() << "\n";
    std::cout << "\n";

    // Example 3: Type with Value Storage
    std::cout << "3. Type Identity with Value Storage:\n";
    auto fast_config = fxt::Type<FastStrategy, int>(100);
    auto accurate_config = fxt::Type<AccurateStrategy, double>(0.001);
    process_with_strategy(fast_config);
    process_with_strategy(accurate_config);
    std::cout << "\n";

    // Example 4: Compile-time Type Manipulation
    std::cout << "4. Compile-time Type Manipulation:\n";
    print_type_info(fxt::Type<int>{});
    std::cout << "\n";
    print_type_info(42);
    std::cout << "\n";

    // Example 5: Helper Functions
    std::cout << "5. Using Helper Functions:\n";
    demonstrate_helper_functions<int>();
    std::cout << "\n";

    std::cout << "   Creating Type with value using make_type\n";
    auto tagged_value = fxt::make_type<std::string>(42);
    std::cout << "   Tagged value: " << tagged_value.value() << "\n";
    std::cout << "   Type tag: std::string, Value type: int\n";
    std::cout << "\n";

    // Example 6: Overload Resolution
    std::cout << "6. Type Tag for Overload Resolution:\n";
    std::vector<int> vec = {5, 2, 8, 1, 9};
    std::cout << "   Before sort: ";
    for (int v : vec) std::cout << v << " ";
    std::cout << "\n";
    sort_container(vec, fxt::Type<std::vector<int>>{});
    std::cout << "   After sort: ";
    for (int v : vec) std::cout << v << " ";
    std::cout << "\n\n";

    // Example 7: Comparison Operators
    std::cout << "7. Comparison and Value Semantics:\n";
    demonstrate_comparison();
    std::cout << "\n";

    // Example 8: Move Semantics
    std::cout << "8. Move Semantics and Reference Qualifiers:\n";
    demonstrate_move_semantics();
    std::cout << "\n";

    // Example 9: Type Extraction
    std::cout << "9. Type Extraction with type_t:\n";
    using MyType = fxt::Type<std::string, int>;
    using ExtractedType = fxt::type_t<MyType>;  // std::string
    using ExtractedValue = fxt::value_type_t<MyType>;  // int
    std::cout << "   Original wrapper type tag: std::string\n";
    std::cout << "   Original wrapper value type: int\n";
    std::cout << "   Extracted type size: " << sizeof(ExtractedType) << " bytes\n";
    std::cout << "   Extracted value type size: " << sizeof(ExtractedValue) << " bytes\n";
    std::cout << "\n";

    // Example 10: Implicit Conversion
    std::cout << "10. Implicit Conversion for Tag Dispatch:\n";
    std::cout << "    Functions accepting Type<int> can be called with int values\n";
    serialize(42, 123);  // Implicit conversion from int to Type<int>
    serialize(std::string("test"), "Hello");  // Implicit conversion
    std::cout << "\n";

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
