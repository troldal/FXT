//
// Created by AI Assistant on 10-10-2025.
//

#include <fxt.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

// ============================================================================
// Example 1: Basic Construction and Element Access
// ============================================================================

void example_basic_usage() {
    std::cout << "Example 1: Basic Construction and Element Access\n";
    std::cout << "================================================\n\n";

    // Create a flat_tuple with mixed types
    fxt::flat_tuple<int, std::string, double> tuple(42, "Hello", 3.14);

    // Access elements using fxt::get<Index>() free function (similar to std::tuple)
    std::cout << "   Element 0 (int): " << fxt::get<0>(tuple) << "\n";
    std::cout << "   Element 1 (string): " << fxt::get<1>(tuple) << "\n";
    std::cout << "   Element 2 (double): " << fxt::get<2>(tuple) << "\n";
    std::cout << "   Tuple size: " << tuple.size() << "\n\n";
}

// ============================================================================
// Example 2: Modifying Elements
// ============================================================================

void example_modification() {
    std::cout << "Example 2: Modifying Elements\n";
    std::cout << "==============================\n\n";

    fxt::flat_tuple<int, std::string, bool> tuple(10, "Original", false);

    std::cout << "   Before modification:\n";
    std::cout << "      int: " << fxt::get<0>(tuple) << "\n";
    std::cout << "      string: " << fxt::get<1>(tuple) << "\n";
    std::cout << "      bool: " << std::boolalpha << fxt::get<2>(tuple) << "\n\n";

    // Modify elements through non-const get()
    fxt::get<0>(tuple) = 99;
    fxt::get<1>(tuple) = "Modified";
    fxt::get<2>(tuple) = true;

    std::cout << "   After modification:\n";
    std::cout << "      int: " << fxt::get<0>(tuple) << "\n";
    std::cout << "      string: " << fxt::get<1>(tuple) << "\n";
    std::cout << "      bool: " << std::boolalpha << fxt::get<2>(tuple) << "\n\n";
}

// ============================================================================
// Example 3: Using visit_all to Process All Elements
// ============================================================================

// void example_visit_all() {
//     std::cout << "Example 3: Using visit_all\n";
//     std::cout << "==========================\n\n";
//
//     fxt::flat_tuple<int, double, std::string> tuple(100, 2.718, "World");
//
//     std::cout << "   Visiting all elements with a generic lambda:\n";
//     tuple.visit_all([](auto& element) {
//         std::cout << "      Value: " << element
//                   << " (type size: " << sizeof(element) << " bytes)\n";
//     });
//     std::cout << "\n";
// }

// ============================================================================
// Example 4: Working with Complex Types
// ============================================================================

void example_complex_types() {
    std::cout << "Example 4: Working with Complex Types\n";
    std::cout << "======================================\n\n";

    // Create a flat_tuple with std::vector and std::unique_ptr
    fxt::flat_tuple<std::vector<int>, std::string, std::unique_ptr<int>> tuple(
        std::vector<int>{1, 2, 3, 4, 5},
        "Complex",
        std::make_unique<int>(777)
    );

    std::cout << "   Vector elements: ";
    for (const auto& val : fxt::get<0>(tuple)) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    std::cout << "   String: " << fxt::get<1>(tuple) << "\n";
    std::cout << "   Unique pointer value: " << *fxt::get<2>(tuple) << "\n\n";

    // Modify the vector through the tuple
    fxt::get<0>(tuple).push_back(6);
    std::cout << "   After adding element to vector: ";
    for (const auto& val : fxt::get<0>(tuple)) {
        std::cout << val << " ";
    }
    std::cout << "\n\n";
}

// ============================================================================
// Example 5: Const Correctness
// ============================================================================

void print_const_tuple(const fxt::flat_tuple<int, std::string, double>& tuple) {
    std::cout << "   Reading from const tuple:\n";
    std::cout << "      Element 0: " << fxt::get<0>(tuple) << "\n";
    std::cout << "      Element 1: " << fxt::get<1>(tuple) << "\n";
    std::cout << "      Element 2: " << fxt::get<2>(tuple) << "\n";
}

void example_const_correctness() {
    std::cout << "Example 5: Const Correctness\n";
    std::cout << "============================\n\n";

    const fxt::flat_tuple<int, std::string, double> const_tuple(42, "Immutable", 3.14159);
    print_const_tuple(const_tuple);
    std::cout << "\n";
}

// ============================================================================
// Example 6: Single Element Tuple
// ============================================================================

void example_single_element() {
    std::cout << "Example 6: Single Element Tuple\n";
    std::cout << "================================\n\n";

    fxt::flat_tuple<std::string> single("Only one element");
    std::cout << "   Single element: " << fxt::get<0>(single) << "\n";
    std::cout << "   Size: " << single.size() << "\n\n";
}

// ============================================================================
// Example 7: Empty Tuple
// ============================================================================

void example_empty_tuple() {
    std::cout << "Example 7: Empty Tuple\n";
    std::cout << "======================\n\n";

    fxt::flat_tuple<> empty;
    std::cout << "   Empty tuple size: " << empty.size() << "\n";
    std::cout << "   (No elements to access)\n\n";
}

// ============================================================================
// Example 8: Practical Use Case - Configuration Storage
// ============================================================================

struct Config {
    fxt::flat_tuple<std::string, int, bool, double> settings;

    Config(std::string name, int port, bool enabled, double timeout)
        : settings(std::move(name), port, enabled, timeout) {}

    std::string& name() { return fxt::get<0>(settings); }
    int& port() { return fxt::get<1>(settings); }
    bool& enabled() { return fxt::get<2>(settings); }
    double& timeout() { return fxt::get<3>(settings); }

    const std::string& name() const { return fxt::get<0>(settings); }
    int port() const { return fxt::get<1>(settings); }
    bool enabled() const { return fxt::get<2>(settings); }
    double timeout() const { return fxt::get<3>(settings); }

    void print() const {
        std::cout << "   Configuration:\n";
        std::cout << "      Name: " << name() << "\n";
        std::cout << "      Port: " << port() << "\n";
        std::cout << "      Enabled: " << std::boolalpha << enabled() << "\n";
        std::cout << "      Timeout: " << timeout() << "s\n";
    }
};

void example_practical_use() {
    std::cout << "Example 8: Practical Use Case - Configuration Storage\n";
    std::cout << "======================================================\n\n";

    Config config("MyServer", 8080, true, 30.0);
    config.print();

    std::cout << "\n   Modifying configuration...\n";
    config.port() = 9090;
    config.timeout() = 60.0;

    std::cout << "\n   Updated configuration:\n";
    config.print();
    std::cout << "\n";
}

// ============================================================================
// Example 9: Using visit_all for Aggregation
// ============================================================================

// void example_visit_aggregation() {
//     std::cout << "Example 9: Using visit_all for Aggregation\n";
//     std::cout << "===========================================\n\n";
//
//     fxt::flat_tuple<int, int, int, int> numbers(10, 20, 30, 40);
//
//     int sum = 0;
//     numbers.visit_all([&sum](const auto& value) {
//         sum += value;
//     });
//
//     std::cout << "   Sum of all elements: " << sum << "\n\n";
// }

// ============================================================================
// Example 10: Memory Layout Demonstration
// ============================================================================

void example_memory_layout() {
    std::cout << "Example 10: Memory Layout\n";
    std::cout << "=========================\n\n";

    using TupleType = fxt::flat_tuple<char, int, double, std::string>;
    TupleType tuple('A', 42, 3.14, "Test");

    std::cout << "   Tuple type size: " << sizeof(TupleType) << " bytes\n";
    std::cout << "   Individual element sizes:\n";
    std::cout << "      char: " << sizeof(char) << " bytes\n";
    std::cout << "      int: " << sizeof(int) << " bytes\n";
    std::cout << "      double: " << sizeof(double) << " bytes\n";
    std::cout << "      string: " << sizeof(std::string) << " bytes\n";

    std::cout << "\n   Note: flat_tuple uses std::array of std::variant\n";
    std::cout << "         to achieve a flattened memory layout.\n\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "========================================\n";
    std::cout << "  fxt::flat_tuple Demonstration\n";
    std::cout << "========================================\n\n";

    example_basic_usage();
    example_modification();
    // example_visit_all();
    example_complex_types();
    example_const_correctness();
    example_single_element();
    example_empty_tuple();
    example_practical_use();
    // example_visit_aggregation();
    example_memory_layout();

    std::cout << "========================================\n";
    std::cout << "  All examples completed successfully!\n";
    std::cout << "========================================\n\n";

    return 0;
}
