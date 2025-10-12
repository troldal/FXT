//
// Demo: fxt::tuple_append
//
// This demo shows how to use fxt::tuple_append to append elements to
// fxt::tuple and fxt::flat_tuple, both with direct calls and using the pipe operator.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// ============================================================================
// Example 1: Using tuple_append with fxt::tuple (std::tuple) - Direct Call
// ============================================================================

void example_tuple_direct() {
    std::cout << "Example 1: Direct Call with fxt::tuple\n";
    std::cout << "=======================================\n\n";

    // Start with a tuple
    auto tuple1 = fxt::make_tuple(42, std::string{"hello"});
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
    auto initial = fxt::make_tuple(100);
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
    auto single = fxt::make_tuple(1, 2, 3) | fxt::tuple_append(4);
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
    auto complex = fxt::make_tuple(std::string{"start"})
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
    auto coords = fxt::make_tuple(std::string{"Point"})
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
    auto tuple1 = fxt::make_tuple(1, 2);
    auto tuple2 = fxt::tuple_append(tuple1, 3, 4, 5);
    std::cout << "   Start: (1, 2)\n";
    std::cout << "   After append(3, 4, 5): (" << fxt::get<0>(tuple2) << ", "
              << fxt::get<1>(tuple2) << ", " << fxt::get<2>(tuple2) << ", "
              << fxt::get<3>(tuple2) << ", " << fxt::get<4>(tuple2) << ")\n\n";

    // Pipe operator with multiple values - fxt::tuple
    std::cout << "   Pipe operator with fxt::tuple:\n";
    auto result = fxt::make_tuple(std::string{"Hello"})
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
    auto complex = fxt::make_tuple(std::string{"Data"})
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
    auto color = fxt::make_tuple(255)                    // Red
        | fxt::tuple_append(128, 64);              // Green, Blue

    std::cout << "   RGB Color: (" << fxt::get<0>(color) << ", "
              << fxt::get<1>(color) << ", " << fxt::get<2>(color) << ")\n\n";
}

// ============================================================================
// Example 9: Monadic Tuple Append with fxt::expected - Direct Call
// ============================================================================

void example_mtuple_append_expected_direct() {
    std::cout << "Example 9: Monadic Tuple Append with fxt::expected - Direct Call\n";
    std::cout << "==================================================================\n\n";

    // Append a plain value to expected<tuple>
    auto exp1 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::make_tuple(1, 2)};
    std::cout << "   Initial: expected<tuple<int, int>> containing (1, 2)\n";

    auto exp2 = fxt::mtuple_append(exp1, 3);
    std::cout << "   After mtuple_append(exp1, 3):\n";
    if (exp2) {
        std::cout << "   Result: (" << fxt::get<0>(*exp2) << ", "
                  << fxt::get<1>(*exp2) << ", " << fxt::get<2>(*exp2) << ")\n\n";
    }

    // Append multiple plain values
    auto exp3 = fxt::mtuple_append(exp1, 3, 4, 5);
    std::cout << "   After mtuple_append(exp1, 3, 4, 5):\n";
    if (exp3) {
        std::cout << "   Result: (" << fxt::get<0>(*exp3) << ", "
                  << fxt::get<1>(*exp3) << ", " << fxt::get<2>(*exp3)
                  << ", " << fxt::get<3>(*exp3) << ", " << fxt::get<4>(*exp3) << ")\n\n";
    }

    // Append expected<value> to expected<tuple>
    auto exp_value = fxt::expected<int, std::string>{10};
    auto exp4 = fxt::mtuple_append(exp1, exp_value);
    std::cout << "   After mtuple_append(exp1, expected<int>{10}):\n";
    if (exp4) {
        std::cout << "   Result: (" << fxt::get<0>(*exp4) << ", "
                  << fxt::get<1>(*exp4) << ", " << fxt::get<2>(*exp4) << ")\n\n";
    }

    // Error propagation - tuple contains error
    auto exp_err = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected{"Error in tuple"}};
    auto exp5 = fxt::mtuple_append(exp_err, 3);
    std::cout << "   Error propagation - error in tuple:\n";
    if (!exp5) {
        std::cout << "   Error: " << exp5.error() << "\n\n";
    }

    // Error propagation - value contains error
    auto exp_value_err = fxt::expected<int, std::string>{fxt::unexpected{"Error in value"}};
    auto exp6 = fxt::mtuple_append(exp1, exp_value_err);
    std::cout << "   Error propagation - error in value:\n";
    if (!exp6) {
        std::cout << "   Error: " << exp6.error() << "\n\n";
    }
}

// ============================================================================
// Example 10: Monadic Tuple Append with fxt::expected - Pipe Operator
// ============================================================================

void example_mtuple_append_expected_pipe() {
    std::cout << "Example 10: Monadic Tuple Append with fxt::expected - Pipe Operator\n";
    std::cout << "=====================================================================\n\n";

    // Pipe with single plain value
    auto exp1 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::make_tuple(1, 2)};
    auto result1 = exp1 | fxt::mtuple_append(3);
    std::cout << "   expected<(1, 2)> | mtuple_append(3):\n";
    if (result1) {
        std::cout << "   Result: (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // Pipe with multiple plain values
    auto result2 = exp1 | fxt::mtuple_append(3, 4, 5);
    std::cout << "   expected<(1, 2)> | mtuple_append(3, 4, 5):\n";
    if (result2) {
        std::cout << "   Result: (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2)
                  << ", " << fxt::get<3>(*result2) << ", " << fxt::get<4>(*result2) << ")\n\n";
    }

    // Pipe with expected<value>
    auto exp_value = fxt::expected<int, std::string>{10};
    auto result3 = exp1 | fxt::mtuple_append(exp_value);
    std::cout << "   expected<(1, 2)> | mtuple_append(expected<10>):\n";
    if (result3) {
        std::cout << "   Result: (" << fxt::get<0>(*result3) << ", "
                  << fxt::get<1>(*result3) << ", " << fxt::get<2>(*result3) << ")\n\n";
    }

    // Chaining multiple appends
    auto result4 = exp1
        | fxt::mtuple_append(3)
        | fxt::mtuple_append(4, 5)
        | fxt::mtuple_append(6);
    std::cout << "   Chaining: expected<(1, 2)> | mtuple_append(3) | mtuple_append(4, 5) | mtuple_append(6):\n";
    if (result4) {
        std::cout << "   Result: (" << fxt::get<0>(*result4) << ", "
                  << fxt::get<1>(*result4) << ", " << fxt::get<2>(*result4)
                  << ", " << fxt::get<3>(*result4) << ", " << fxt::get<4>(*result4)
                  << ", " << fxt::get<5>(*result4) << ")\n\n";
    }
}

// ============================================================================
// Example 11: Monadic Tuple Append with fxt::optional - Direct Call
// ============================================================================

void example_mtuple_append_optional_direct() {
    std::cout << "Example 11: Monadic Tuple Append with fxt::optional - Direct Call\n";
    std::cout << "==================================================================\n\n";

    // Append a plain value to optional<tuple>
    auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
    std::cout << "   Initial: optional<tuple<int, int>> containing (1, 2)\n";

    auto opt2 = fxt::mtuple_append(opt1, 3);
    std::cout << "   After mtuple_append(opt1, 3):\n";
    if (opt2) {
        std::cout << "   Result: (" << fxt::get<0>(*opt2) << ", "
                  << fxt::get<1>(*opt2) << ", " << fxt::get<2>(*opt2) << ")\n\n";
    }

    // Append multiple plain values
    auto opt3 = fxt::mtuple_append(opt1, 3, 4, 5);
    std::cout << "   After mtuple_append(opt1, 3, 4, 5):\n";
    if (opt3) {
        std::cout << "   Result: (" << fxt::get<0>(*opt3) << ", "
                  << fxt::get<1>(*opt3) << ", " << fxt::get<2>(*opt3)
                  << ", " << fxt::get<3>(*opt3) << ", " << fxt::get<4>(*opt3) << ")\n\n";
    }

    // Append optional<value> to optional<tuple>
    auto opt_value = fxt::optional<int>{10};
    auto opt4 = fxt::mtuple_append(opt1, opt_value);
    std::cout << "   After mtuple_append(opt1, optional<int>{10}):\n";
    if (opt4) {
        std::cout << "   Result: (" << fxt::get<0>(*opt4) << ", "
                  << fxt::get<1>(*opt4) << ", " << fxt::get<2>(*opt4) << ")\n\n";
    }

    // Nullopt propagation - tuple is nullopt
    auto opt_empty = fxt::optional<fxt::tuple<int, int>>{};
    auto opt5 = fxt::mtuple_append(opt_empty, 3);
    std::cout << "   Nullopt propagation - empty tuple:\n";
    if (!opt5) {
        std::cout << "   Result: nullopt\n\n";
    }

    // Nullopt propagation - value is nullopt
    auto opt_value_empty = fxt::optional<int>{};
    auto opt6 = fxt::mtuple_append(opt1, opt_value_empty);
    std::cout << "   Nullopt propagation - empty value:\n";
    if (!opt6) {
        std::cout << "   Result: nullopt\n\n";
    }
}

// ============================================================================
// Example 12: Monadic Tuple Append with fxt::optional - Pipe Operator
// ============================================================================

void example_mtuple_append_optional_pipe() {
    std::cout << "Example 12: Monadic Tuple Append with fxt::optional - Pipe Operator\n";
    std::cout << "====================================================================\n\n";

    // Pipe with single plain value
    auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
    auto result1 = opt1 | fxt::mtuple_append(3);
    std::cout << "   optional<(1, 2)> | mtuple_append(3):\n";
    if (result1) {
        std::cout << "   Result: (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // Pipe with multiple plain values
    auto result2 = opt1 | fxt::mtuple_append(3, 4, 5);
    std::cout << "   optional<(1, 2)> | mtuple_append(3, 4, 5):\n";
    if (result2) {
        std::cout << "   Result: (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2)
                  << ", " << fxt::get<3>(*result2) << ", " << fxt::get<4>(*result2) << ")\n\n";
    }

    // Pipe with optional<value>
    auto opt_value = fxt::optional<int>{10};
    auto result3 = opt1 | fxt::mtuple_append(opt_value);
    std::cout << "   optional<(1, 2)> | mtuple_append(optional<10>):\n";
    if (result3) {
        std::cout << "   Result: (" << fxt::get<0>(*result3) << ", "
                  << fxt::get<1>(*result3) << ", " << fxt::get<2>(*result3) << ")\n\n";
    }

    // Chaining multiple appends
    auto result4 = opt1
        | fxt::mtuple_append(3)
        | fxt::mtuple_append(4, 5)
        | fxt::mtuple_append(6);
    std::cout << "   Chaining: optional<(1, 2)> | mtuple_append(3) | mtuple_append(4, 5) | mtuple_append(6):\n";
    if (result4) {
        std::cout << "   Result: (" << fxt::get<0>(*result4) << ", "
                  << fxt::get<1>(*result4) << ", " << fxt::get<2>(*result4)
                  << ", " << fxt::get<3>(*result4) << ", " << fxt::get<4>(*result4)
                  << ", " << fxt::get<5>(*result4) << ")\n\n";
    }
}

// ============================================================================
// Example 13: Monadic Tuple Append with fxt::flat_tuple
// ============================================================================

void example_mtuple_append_flat_tuple() {
    std::cout << "Example 13: Monadic Tuple Append with fxt::flat_tuple\n";
    std::cout << "======================================================\n\n";

    // expected<flat_tuple> with direct call
    std::cout << "   Using fxt::expected with fxt::flat_tuple:\n";
    auto exp_flat = fxt::expected<fxt::flat_tuple<double, double>, std::string>{
        fxt::flat_tuple<double, double>{1.0, 2.0}
    };
    auto result1 = fxt::mtuple_append(exp_flat, 3.0);
    if (result1) {
        std::cout << "   mtuple_append(expected<(1.0, 2.0)>, 3.0) = ("
                  << fxt::get<0>(*result1) << ", " << fxt::get<1>(*result1)
                  << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // expected<flat_tuple> with pipe operator
    auto result2 = exp_flat | fxt::mtuple_append(3.0, 4.0);
    if (result2) {
        std::cout << "   expected<(1.0, 2.0)> | mtuple_append(3.0, 4.0) = ("
                  << fxt::get<0>(*result2) << ", " << fxt::get<1>(*result2)
                  << ", " << fxt::get<2>(*result2) << ", " << fxt::get<3>(*result2) << ")\n\n";
    }

    // optional<flat_tuple> with direct call
    std::cout << "   Using fxt::optional with fxt::flat_tuple:\n";
    auto opt_flat = fxt::optional<fxt::flat_tuple<double, double>>{
        fxt::flat_tuple<double, double>{5.0, 6.0}
    };
    auto result3 = fxt::mtuple_append(opt_flat, 7.0);
    if (result3) {
        std::cout << "   mtuple_append(optional<(5.0, 6.0)>, 7.0) = ("
                  << fxt::get<0>(*result3) << ", " << fxt::get<1>(*result3)
                  << ", " << fxt::get<2>(*result3) << ")\n\n";
    }

    // optional<flat_tuple> with pipe operator and chaining
    auto result4 = opt_flat
        | fxt::mtuple_append(7.0)
        | fxt::mtuple_append(8.0, 9.0);
    if (result4) {
        std::cout << "   optional<(5.0, 6.0)> | mtuple_append(7.0) | mtuple_append(8.0, 9.0) = ("
                  << fxt::get<0>(*result4) << ", " << fxt::get<1>(*result4)
                  << ", " << fxt::get<2>(*result4) << ", " << fxt::get<3>(*result4)
                  << ", " << fxt::get<4>(*result4) << ")\n\n";
    }
}

// ============================================================================
// Example 14: Complex Monadic Append Scenarios
// ============================================================================

void example_mtuple_append_complex() {
    std::cout << "Example 14: Complex Monadic Append Scenarios\n";
    std::cout << "=============================================\n\n";

    // Building a data record with monadic operations
    std::cout << "   Building a patient record:\n";
    auto patient_id = fxt::expected<fxt::tuple<int>, std::string>{fxt::make_tuple(12345)};
    auto temperature = fxt::expected<double, std::string>{98.6};
    auto heart_rate = fxt::expected<int, std::string>{72};
    auto blood_pressure = fxt::expected<int, std::string>{120};

    auto record = patient_id
        | fxt::mtuple_append(temperature)
        | fxt::mtuple_append(heart_rate)
        | fxt::mtuple_append(blood_pressure);

    std::cout << "   Patient ID: 12345\n";
    std::cout << "   Temperature: 98.6°F\n";
    std::cout << "   Heart Rate: 72 bpm\n";
    std::cout << "   Blood Pressure: 120 mmHg\n";
    if (record) {
        std::cout << "   Final Record: (" << fxt::get<0>(*record) << ", "
                  << fxt::get<1>(*record) << ", " << fxt::get<2>(*record)
                  << ", " << fxt::get<3>(*record) << ")\n\n";
    }

    // Mixing plain values and monadic values
    std::cout << "   Mixing plain and monadic values:\n";
    auto base = fxt::optional<fxt::tuple<std::string>>{fxt::make_tuple(std::string{"Base"})};
    auto value1 = fxt::optional<int>{42};

    auto mixed = base
        | fxt::mtuple_append(value1)           // Monadic value
        | fxt::mtuple_append(3.14)             // Plain value
        | fxt::mtuple_append(true);            // Plain value

    if (mixed) {
        std::cout << "   Result: (\"" << fxt::get<0>(*mixed) << "\", "
                  << fxt::get<1>(*mixed) << ", " << fxt::get<2>(*mixed)
                  << ", " << std::boolalpha << fxt::get<3>(*mixed) << ")\n\n";
    }

    // Error handling in a computation pipeline
    std::cout << "   Error handling in pipeline:\n";
    auto compute_value = [](int x) -> fxt::expected<int, std::string> {
        if (x < 0) return fxt::unexpected{"Negative value"};
        return x * 2;
    };

    auto pipeline1 = fxt::expected<fxt::tuple<int>, std::string>{fxt::make_tuple(5)}
        | fxt::mtuple_append(compute_value(10))
        | fxt::mtuple_append(compute_value(15));

    if (pipeline1) {
        std::cout << "   Success: (" << fxt::get<0>(*pipeline1) << ", "
                  << fxt::get<1>(*pipeline1) << ", " << fxt::get<2>(*pipeline1) << ")\n";
    }

    auto pipeline2 = fxt::expected<fxt::tuple<int>, std::string>{fxt::make_tuple(5)}
        | fxt::mtuple_append(compute_value(10))
        | fxt::mtuple_append(compute_value(-5));  // This will fail

    if (!pipeline2) {
        std::cout << "   Error: " << pipeline2.error() << "\n\n";
    }
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

    std::cout << "\n=== fxt::mtuple_append Demo ===\n\n";

    example_mtuple_append_expected_direct();
    example_mtuple_append_expected_pipe();
    example_mtuple_append_optional_direct();
    example_mtuple_append_optional_pipe();
    example_mtuple_append_flat_tuple();
    example_mtuple_append_complex();

    std::cout << "=== Demo Complete ===\n\n";
    return 0;
}

