//
// Demo: fxt::tuple_append
//
// This demo shows how to use fxt::tuple_append to append elements to
// fxt::tuple and fxt::flat_tuple, both with direct calls and using the pipe operator.
//

#include <fxt/tuple/TupleAppend.hpp>
#include <fxt/tuple/Tuple.hpp>
#include <fxt/tuple/FlatTuple.hpp>
#include <iostream>
#include <string>

// ============================================================================
// Example 1: Using tuple_append with fxt::tuple (std::tuple) - Direct Call
// ============================================================================

void example_tuple_direct() {
    std::cout << "Example 1: Direct Call with fxt::tuple\n";
    std::cout << "=======================================\n\n";

    // Start with a tuple
    auto tuple1 = fxt::tuple{42, std::string{"hello"}};
    std::cout << "   Initial tuple: (42, \"hello\")\n";

    // Append a double
    auto tuple2 = fxt::tuple_append(tuple1, 3.14);
    std::cout << "   After appending 3.14: (42, \"hello\", 3.14)\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple2) << ", \""
              << fxt::get<1>(tuple2) << "\", " << fxt::get<2>(tuple2) << ")\n";

    // Append a bool
    auto tuple3 = fxt::tuple_append(tuple2, true);
    std::cout << "   After appending true: (42, \"hello\", 3.14, true)\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple3) << ", \""
              << fxt::get<1>(tuple3) << "\", " << fxt::get<2>(tuple3)
              << ", " << std::boolalpha << fxt::get<3>(tuple3) << ")\n\n";
}

// ============================================================================
// Example 2: Using tuple_append with fxt::tuple - Pipe Operator
// ============================================================================

void example_tuple_pipe() {
    std::cout << "Example 2: Pipe Operator with fxt::tuple\n";
    std::cout << "=========================================\n\n";

    // Start with a tuple
    auto initial = fxt::tuple{100};
    std::cout << "   Initial tuple: (100)\n";

    // Use pipe operator for fluent chaining
    auto result = initial
        | fxt::tuple_append(std::string{"world"})
        | fxt::tuple_append(2.71)
        | fxt::tuple_append(false);

    std::cout << "   After chaining appends: (100, \"world\", 2.71, false)\n";
    std::cout << "   Values: (" << fxt::get<0>(result) << ", \""
              << fxt::get<1>(result) << "\", " << fxt::get<2>(result)
              << ", " << std::boolalpha << fxt::get<3>(result) << ")\n\n";

    // Single append with pipe operator
    auto single = fxt::tuple{1, 2, 3} | fxt::tuple_append(4);
    std::cout << "   Single append: (1, 2, 3) | append(4) = (1, 2, 3, 4)\n";
    std::cout << "   Values: (" << fxt::get<0>(single) << ", "
              << fxt::get<1>(single) << ", " << fxt::get<2>(single)
              << ", " << fxt::get<3>(single) << ")\n\n";
}

// ============================================================================
// Example 3: Using tuple_append with fxt::flat_tuple - Direct Call
// ============================================================================

void example_flat_tuple_direct() {
    std::cout << "Example 3: Direct Call with fxt::flat_tuple\n";
    std::cout << "============================================\n\n";

    // Start with a flat_tuple (note: explicit template arguments required)
    auto tuple1 = fxt::flat_tuple<double, double>{1.0, 2.0};
    std::cout << "   Initial flat_tuple: (1.0, 2.0)\n";

    // Append a double
    auto tuple2 = fxt::tuple_append(tuple1, 3.0);
    std::cout << "   After appending 3.0: (1.0, 2.0, 3.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple2) << ", "
              << fxt::get<1>(tuple2) << ", " << fxt::get<2>(tuple2) << ")\n";

    // Append another double
    auto tuple3 = fxt::tuple_append(tuple2, 4.0);
    std::cout << "   After appending 4.0: (1.0, 2.0, 3.0, 4.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple3) << ", "
              << fxt::get<1>(tuple3) << ", " << fxt::get<2>(tuple3)
              << ", " << fxt::get<3>(tuple3) << ")\n\n";
}

// ============================================================================
// Example 4: Using tuple_append with fxt::flat_tuple - Pipe Operator
// ============================================================================

void example_flat_tuple_pipe() {
    std::cout << "Example 4: Pipe Operator with fxt::flat_tuple\n";
    std::cout << "==============================================\n\n";

    // Start with a flat_tuple (note: explicit template arguments required)
    auto initial = fxt::flat_tuple<double>{5.0};
    std::cout << "   Initial flat_tuple: (5.0)\n";

    // Use pipe operator for fluent chaining
    auto result = initial
        | fxt::tuple_append(10.0)
        | fxt::tuple_append(15.0)
        | fxt::tuple_append(20.0);

    std::cout << "   After chaining appends: (5.0, 10.0, 15.0, 20.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(result) << ", "
              << fxt::get<1>(result) << ", " << fxt::get<2>(result)
              << ", " << fxt::get<3>(result) << ")\n\n";

    // Single append with pipe operator
    auto single = fxt::flat_tuple<double, double>{1.1, 2.2} | fxt::tuple_append(3.3);
    std::cout << "   Single append: (1.1, 2.2) | append(3.3) = (1.1, 2.2, 3.3)\n";
    std::cout << "   Values: (" << fxt::get<0>(single) << ", "
              << fxt::get<1>(single) << ", " << fxt::get<2>(single) << ")\n\n";
}

// ============================================================================
// Example 5: Mixed Types and Complex Chaining
// ============================================================================

void example_mixed_types() {
    std::cout << "Example 5: Mixed Types and Complex Chaining\n";
    std::cout << "============================================\n\n";

    // Build a complex tuple step by step with different types
    auto complex = fxt::tuple{std::string{"start"}}
        | fxt::tuple_append(42)
        | fxt::tuple_append(3.14159)
        | fxt::tuple_append(true)
        | fxt::tuple_append(std::string{"end"});

    std::cout << "   Complex tuple with mixed types:\n";
    std::cout << "   Type sequence: <string, int, double, bool, string>\n";
    std::cout << "   Values: (\"" << fxt::get<0>(complex) << "\", "
              << fxt::get<1>(complex) << ", " << fxt::get<2>(complex)
              << ", " << std::boolalpha << fxt::get<3>(complex)
              << ", \"" << fxt::get<4>(complex) << "\")\n\n";

    // Build a flat_tuple with integers
    auto int_sequence = fxt::flat_tuple<int>{1}
        | fxt::tuple_append(2)
        | fxt::tuple_append(3)
        | fxt::tuple_append(4)
        | fxt::tuple_append(5);

    std::cout << "   Integer sequence flat_tuple: (1, 2, 3, 4, 5)\n";
    std::cout << "   Values: (" << fxt::get<0>(int_sequence) << ", "
              << fxt::get<1>(int_sequence) << ", " << fxt::get<2>(int_sequence)
              << ", " << fxt::get<3>(int_sequence) << ", "
              << fxt::get<4>(int_sequence) << ")\n\n";
}

// ============================================================================
// Example 6: Building Tuples from Scratch
// ============================================================================

void example_building_from_scratch() {
    std::cout << "Example 6: Building Tuples from Scratch\n";
    std::cout << "========================================\n\n";

    // Start with empty-ish tuple and build up
    auto coords = fxt::tuple{std::string{"Point"}}
        | fxt::tuple_append(10.5)   // x coordinate
        | fxt::tuple_append(20.3)   // y coordinate
        | fxt::tuple_append(5.7);   // z coordinate

    std::cout << "   3D Point representation:\n";
    std::cout << "   Label: \"" << fxt::get<0>(coords) << "\"\n";
    std::cout << "   X: " << fxt::get<1>(coords) << "\n";
    std::cout << "   Y: " << fxt::get<2>(coords) << "\n";
    std::cout << "   Z: " << fxt::get<3>(coords) << "\n\n";

    // Create a data record
    auto record = fxt::flat_tuple<int>{42}                   // ID
        | fxt::tuple_append(98.6)                           // Temperature
        | fxt::tuple_append(120)                            // Heart rate
        | fxt::tuple_append(true);                          // Healthy

    std::cout << "   Medical Record:\n";
    std::cout << "   Patient ID: " << fxt::get<0>(record) << "\n";
    std::cout << "   Temperature: " << fxt::get<1>(record) << "°F\n";
    std::cout << "   Heart Rate: " << fxt::get<2>(record) << " bpm\n";
    std::cout << "   Healthy: " << std::boolalpha << fxt::get<3>(record) << "\n\n";
}

// ============================================================================
// Example 7: Appending Multiple Values at Once
// ============================================================================

void example_multiple_values() {
    std::cout << "Example 7: Appending Multiple Values at Once\n";
    std::cout << "=============================================\n\n";

    // Direct call with multiple values - fxt::tuple
    std::cout << "   Direct call with fxt::tuple:\n";
    auto tuple1 = fxt::tuple{1, 2};
    auto tuple2 = fxt::tuple_append(tuple1, 3, 4, 5);
    std::cout << "   Start: (1, 2)\n";
    std::cout << "   After append(3, 4, 5): (" << fxt::get<0>(tuple2) << ", "
              << fxt::get<1>(tuple2) << ", " << fxt::get<2>(tuple2) << ", "
              << fxt::get<3>(tuple2) << ", " << fxt::get<4>(tuple2) << ")\n\n";

    // Pipe operator with multiple values - fxt::tuple
    std::cout << "   Pipe operator with fxt::tuple:\n";
    auto result = fxt::tuple{std::string{"Hello"}}
        | fxt::tuple_append(42, 3.14, true);
    std::cout << "   (\"Hello\") | append(42, 3.14, true)\n";
    std::cout << "   Result: (\"" << fxt::get<0>(result) << "\", "
              << fxt::get<1>(result) << ", " << fxt::get<2>(result)
              << ", " << std::boolalpha << fxt::get<3>(result) << ")\n\n";

    // Direct call with multiple values - fxt::flat_tuple
    std::cout << "   Direct call with fxt::flat_tuple:\n";
    auto flat1 = fxt::flat_tuple<double, double>{1.0, 2.0};
    auto flat2 = fxt::tuple_append(flat1, 3.0, 4.0, 5.0);
    std::cout << "   Start: (1.0, 2.0)\n";
    std::cout << "   After append(3.0, 4.0, 5.0): (" << fxt::get<0>(flat2) << ", "
              << fxt::get<1>(flat2) << ", " << fxt::get<2>(flat2) << ", "
              << fxt::get<3>(flat2) << ", " << fxt::get<4>(flat2) << ")\n\n";

    // Pipe operator with multiple values - fxt::flat_tuple
    std::cout << "   Pipe operator with fxt::flat_tuple:\n";
    auto flat_result = fxt::flat_tuple<int>{10}
        | fxt::tuple_append(20, 30, 40, 50);
    std::cout << "   (10) | append(20, 30, 40, 50)\n";
    std::cout << "   Result: (" << fxt::get<0>(flat_result) << ", "
              << fxt::get<1>(flat_result) << ", " << fxt::get<2>(flat_result)
              << ", " << fxt::get<3>(flat_result) << ", " << fxt::get<4>(flat_result) << ")\n\n";
}

// ============================================================================
// Example 8: Mixing Single and Multiple Appends
// ============================================================================

void example_mixed_appends() {
    std::cout << "Example 8: Mixing Single and Multiple Appends\n";
    std::cout << "==============================================\n\n";

    // Build a tuple with mix of single and multiple appends
    auto complex = fxt::tuple{std::string{"Data"}}
        | fxt::tuple_append(1, 2, 3)                    // Append 3 values
        | fxt::tuple_append(4.5)                        // Append 1 value
        | fxt::tuple_append(6.7, 8.9)                   // Append 2 values
        | fxt::tuple_append(true);                      // Append 1 value

    std::cout << "   Building with mixed appends:\n";
    std::cout << "   Start: (\"Data\")\n";
    std::cout << "   | append(1, 2, 3)\n";
    std::cout << "   | append(4.5)\n";
    std::cout << "   | append(6.7, 8.9)\n";
    std::cout << "   | append(true)\n\n";
    std::cout << "   Final result: (\"" << fxt::get<0>(complex) << "\", "
              << fxt::get<1>(complex) << ", " << fxt::get<2>(complex) << ", "
              << fxt::get<3>(complex) << ", " << fxt::get<4>(complex) << ", "
              << fxt::get<5>(complex) << ", " << fxt::get<6>(complex)
              << ", " << std::boolalpha << fxt::get<7>(complex) << ")\n\n";

    // Build a coordinate system with multiple appends
    auto vector3d = fxt::flat_tuple<double>{0.0}
        | fxt::tuple_append(1.0, 2.0);              // Add y and z at once

    std::cout << "   3D Vector: (" << fxt::get<0>(vector3d) << ", "
              << fxt::get<1>(vector3d) << ", " << fxt::get<2>(vector3d) << ")\n\n";

    // Build RGB color from separate R and GB values
    auto color = fxt::tuple{255}                    // Red
        | fxt::tuple_append(128, 64);              // Green, Blue

    std::cout << "   RGB Color: (" << fxt::get<0>(color) << ", "
              << fxt::get<1>(color) << ", " << fxt::get<2>(color) << ")\n\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n=== fxt::tuple_append Demo ===\n\n";

    example_tuple_direct();
    example_tuple_pipe();
    example_flat_tuple_direct();
    example_flat_tuple_pipe();
    example_mixed_types();
    example_building_from_scratch();
    example_multiple_values();
    example_mixed_appends();

    std::cout << "=== Demo Complete ===\n\n";
    return 0;
}

