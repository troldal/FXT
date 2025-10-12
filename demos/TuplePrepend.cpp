//
// Demo: fxt::tuple_prepend
//
// This demo shows how to use fxt::tuple_prepend to prepend elements to
// fxt::tuple and fxt::flat_tuple, both with direct calls and using the pipe operator.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// ============================================================================
// Example 1: Using tuple_prepend with fxt::tuple (std::tuple) - Direct Call
// ============================================================================

void example_tuple_direct() {
    std::cout << "Example 1: Direct Call with fxt::tuple\n";
    std::cout << "=======================================\n\n";

    // Start with a tuple
    auto tuple1 = fxt::tuple{42, std::string{"hello"}};
    std::cout << "   Initial tuple: (42, \"hello\")\n";

    // Prepend a double
    auto tuple2 = fxt::tuple_prepend(tuple1, 3.14);
    std::cout << "   After prepending 3.14: (3.14, 42, \"hello\")\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple2) << ", "
              << fxt::get<1>(tuple2) << ", \"" << fxt::get<2>(tuple2) << "\")\n";

    // Prepend a bool
    auto tuple3 = fxt::tuple_prepend(tuple2, true);
    std::cout << "   After prepending true: (true, 3.14, 42, \"hello\")\n";
    std::cout << "   Values: (" << std::boolalpha << fxt::get<0>(tuple3) << ", "
              << fxt::get<1>(tuple3) << ", " << fxt::get<2>(tuple3)
              << ", \"" << fxt::get<3>(tuple3) << "\")\n\n";
}

// ============================================================================
// Example 2: Using tuple_prepend with fxt::tuple - Pipe Operator
// ============================================================================

void example_tuple_pipe() {
    std::cout << "Example 2: Pipe Operator with fxt::tuple\n";
    std::cout << "=========================================\n\n";

    // Start with a tuple
    auto initial = fxt::tuple{100};
    std::cout << "   Initial tuple: (100)\n";

    // Use pipe operator for fluent chaining
    auto result = initial
        | fxt::tuple_prepend(std::string{"world"})
        | fxt::tuple_prepend(2.71)
        | fxt::tuple_prepend(false);

    std::cout << "   After chaining prepends: (false, 2.71, \"world\", 100)\n";
    std::cout << "   Values: (" << std::boolalpha << fxt::get<0>(result) << ", "
              << fxt::get<1>(result) << ", \"" << fxt::get<2>(result)
              << "\", " << fxt::get<3>(result) << ")\n\n";

    // Single prepend with pipe operator
    auto single = fxt::tuple{1, 2, 3} | fxt::tuple_prepend(4);
    std::cout << "   Single prepend: (1, 2, 3) | prepend(4) = (4, 1, 2, 3)\n";
    std::cout << "   Values: (" << fxt::get<0>(single) << ", "
              << fxt::get<1>(single) << ", " << fxt::get<2>(single)
              << ", " << fxt::get<3>(single) << ")\n\n";
}

// ============================================================================
// Example 3: Using tuple_prepend with fxt::flat_tuple - Direct Call
// ============================================================================

void example_flat_tuple_direct() {
    std::cout << "Example 3: Direct Call with fxt::flat_tuple\n";
    std::cout << "============================================\n\n";

    // Start with a flat_tuple (note: explicit template arguments required)
    auto tuple1 = fxt::flat_tuple<double, double>{1.0, 2.0};
    std::cout << "   Initial flat_tuple: (1.0, 2.0)\n";

    // Prepend a double
    auto tuple2 = fxt::tuple_prepend(tuple1, 3.0);
    std::cout << "   After prepending 3.0: (3.0, 1.0, 2.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple2) << ", "
              << fxt::get<1>(tuple2) << ", " << fxt::get<2>(tuple2) << ")\n";

    // Prepend another double
    auto tuple3 = fxt::tuple_prepend(tuple2, 4.0);
    std::cout << "   After prepending 4.0: (4.0, 3.0, 1.0, 2.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(tuple3) << ", "
              << fxt::get<1>(tuple3) << ", " << fxt::get<2>(tuple3)
              << ", " << fxt::get<3>(tuple3) << ")\n\n";
}

// ============================================================================
// Example 4: Using tuple_prepend with fxt::flat_tuple - Pipe Operator
// ============================================================================

void example_flat_tuple_pipe() {
    std::cout << "Example 4: Pipe Operator with fxt::flat_tuple\n";
    std::cout << "==============================================\n\n";

    // Start with a flat_tuple (note: explicit template arguments required)
    auto initial = fxt::flat_tuple<double>{5.0};
    std::cout << "   Initial flat_tuple: (5.0)\n";

    // Use pipe operator for fluent chaining
    auto result = initial
        | fxt::tuple_prepend(10.0)
        | fxt::tuple_prepend(15.0)
        | fxt::tuple_prepend(20.0);

    std::cout << "   After chaining prepends: (20.0, 15.0, 10.0, 5.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(result) << ", "
              << fxt::get<1>(result) << ", " << fxt::get<2>(result)
              << ", " << fxt::get<3>(result) << ")\n\n";

    // Single prepend with pipe operator
    auto single = fxt::flat_tuple<double, double>{1.1, 2.2} | fxt::tuple_prepend(3.3);
    std::cout << "   Single prepend: (1.1, 2.2) | prepend(3.3) = (3.3, 1.1, 2.2)\n";
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
    auto complex = fxt::tuple{std::string{"end"}}
        | fxt::tuple_prepend(true)
        | fxt::tuple_prepend(3.14159)
        | fxt::tuple_prepend(42)
        | fxt::tuple_prepend(std::string{"start"});

    std::cout << "   Complex tuple with mixed types:\n";
    std::cout << "   Type sequence: <string, int, double, bool, string>\n";
    std::cout << "   Values: (\"" << fxt::get<0>(complex) << "\", "
              << fxt::get<1>(complex) << ", " << fxt::get<2>(complex)
              << ", " << std::boolalpha << fxt::get<3>(complex)
              << ", \"" << fxt::get<4>(complex) << "\")\n\n";

    // Build a flat_tuple with integers
    auto int_sequence = fxt::flat_tuple<int>{5}
        | fxt::tuple_prepend(4)
        | fxt::tuple_prepend(3)
        | fxt::tuple_prepend(2)
        | fxt::tuple_prepend(1);

    std::cout << "   Integer sequence flat_tuple: (1, 2, 3, 4, 5)\n";
    std::cout << "   Values: (" << fxt::get<0>(int_sequence) << ", "
              << fxt::get<1>(int_sequence) << ", " << fxt::get<2>(int_sequence)
              << ", " << fxt::get<3>(int_sequence) << ", "
              << fxt::get<4>(int_sequence) << ")\n\n";
}

// ============================================================================
// Example 6: Building Tuples with Headers and Data
// ============================================================================

void example_building_with_headers() {
    std::cout << "Example 6: Building Tuples with Headers and Data\n";
    std::cout << "=================================================\n\n";

    // Start with data and prepend metadata
    auto data_record = fxt::tuple{98.6, 120, true}               // Data values
        | fxt::tuple_prepend(42)                                  // ID
        | fxt::tuple_prepend(std::string{"Patient"});             // Type

    std::cout << "   Medical Record with header:\n";
    std::cout << "   Type: \"" << fxt::get<0>(data_record) << "\"\n";
    std::cout << "   Patient ID: " << fxt::get<1>(data_record) << "\n";
    std::cout << "   Temperature: " << fxt::get<2>(data_record) << "°F\n";
    std::cout << "   Heart Rate: " << fxt::get<3>(data_record) << " bpm\n";
    std::cout << "   Healthy: " << std::boolalpha << fxt::get<4>(data_record) << "\n\n";

    // Build a message with priority
    auto message = fxt::flat_tuple<std::string>{std::string{"System operational"}}
        | fxt::tuple_prepend(1234567890)                          // Timestamp
        | fxt::tuple_prepend(std::string{"INFO"});                // Priority level

    std::cout << "   Log Message:\n";
    std::cout << "   Priority: \"" << fxt::get<0>(message) << "\"\n";
    std::cout << "   Timestamp: " << fxt::get<1>(message) << "\n";
    std::cout << "   Message: \"" << fxt::get<2>(message) << "\"\n\n";
}

// ============================================================================
// Example 7: Prepending Multiple Values at Once
// ============================================================================

void example_multiple_values() {
    std::cout << "Example 7: Prepending Multiple Values at Once\n";
    std::cout << "==============================================\n\n";

    // Direct call with multiple values - fxt::tuple
    std::cout << "   Direct call with fxt::tuple:\n";
    auto tuple1 = fxt::tuple{1, 2};
    auto tuple2 = fxt::tuple_prepend(tuple1, 3, 4, 5);
    std::cout << "   Start: (1, 2)\n";
    std::cout << "   After prepend(3, 4, 5): (" << fxt::get<0>(tuple2) << ", "
              << fxt::get<1>(tuple2) << ", " << fxt::get<2>(tuple2) << ", "
              << fxt::get<3>(tuple2) << ", " << fxt::get<4>(tuple2) << ")\n\n";

    // Pipe operator with multiple values - fxt::tuple
    std::cout << "   Pipe operator with fxt::tuple:\n";
    auto result = fxt::tuple{std::string{"Hello"}}
        | fxt::tuple_prepend(42, 3.14, true);
    std::cout << "   (\"Hello\") | prepend(42, 3.14, true)\n";
    std::cout << "   Result: (" << fxt::get<0>(result) << ", "
              << fxt::get<1>(result) << ", " << std::boolalpha << fxt::get<2>(result)
              << ", \"" << fxt::get<3>(result) << "\")\n\n";

    // Direct call with multiple values - fxt::flat_tuple
    std::cout << "   Direct call with fxt::flat_tuple:\n";
    auto flat1 = fxt::flat_tuple<double, double>{1.0, 2.0};
    auto flat2 = fxt::tuple_prepend(flat1, 3.0, 4.0, 5.0);
    std::cout << "   Start: (1.0, 2.0)\n";
    std::cout << "   After prepend(3.0, 4.0, 5.0): (" << fxt::get<0>(flat2) << ", "
              << fxt::get<1>(flat2) << ", " << fxt::get<2>(flat2) << ", "
              << fxt::get<3>(flat2) << ", " << fxt::get<4>(flat2) << ")\n\n";

    // Pipe operator with multiple values - fxt::flat_tuple
    std::cout << "   Pipe operator with fxt::flat_tuple:\n";
    auto flat_result = fxt::flat_tuple<int>{10}
        | fxt::tuple_prepend(20, 30, 40, 50);
    std::cout << "   (10) | prepend(20, 30, 40, 50)\n";
    std::cout << "   Result: (" << fxt::get<0>(flat_result) << ", "
              << fxt::get<1>(flat_result) << ", " << fxt::get<2>(flat_result)
              << ", " << fxt::get<3>(flat_result) << ", " << fxt::get<4>(flat_result) << ")\n\n";
}

// ============================================================================
// Example 8: Mixing Single and Multiple Prepends
// ============================================================================

void example_mixed_prepends() {
    std::cout << "Example 8: Mixing Single and Multiple Prepends\n";
    std::cout << "===============================================\n\n";

    // Build a tuple with mix of single and multiple prepends
    auto complex = fxt::tuple{std::string{"Data"}}
        | fxt::tuple_prepend(true)                      // Prepend 1 value
        | fxt::tuple_prepend(6.7, 8.9)                  // Prepend 2 values
        | fxt::tuple_prepend(4.5)                       // Prepend 1 value
        | fxt::tuple_prepend(1, 2, 3);                  // Prepend 3 values

    std::cout << "   Building with mixed prepends:\n";
    std::cout << "   Start: (\"Data\")\n";
    std::cout << "   | prepend(true)\n";
    std::cout << "   | prepend(6.7, 8.9)\n";
    std::cout << "   | prepend(4.5)\n";
    std::cout << "   | prepend(1, 2, 3)\n\n";
    std::cout << "   Final result: (" << fxt::get<0>(complex) << ", "
              << fxt::get<1>(complex) << ", " << fxt::get<2>(complex) << ", "
              << fxt::get<3>(complex) << ", " << fxt::get<4>(complex) << ", "
              << fxt::get<5>(complex) << ", " << std::boolalpha << fxt::get<6>(complex)
              << ", \"" << fxt::get<7>(complex) << "\")\n\n";
}

// ============================================================================
// Example 9: Monadic Tuple Prepend with fxt::expected - Direct Call
// ============================================================================

void example_mtuple_prepend_expected_direct() {
    std::cout << "Example 9: Monadic Tuple Prepend with fxt::expected - Direct Call\n";
    std::cout << "===================================================================\n\n";

    // Prepend a plain value to expected<tuple>
    auto exp1 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{1, 2}};
    std::cout << "   Initial: expected<tuple<int, int>> containing (1, 2)\n";

    auto exp2 = fxt::mtuple_prepend(exp1, 3);
    std::cout << "   After mtuple_prepend(exp1, 3):\n";
    if (exp2) {
        std::cout << "   Result: (" << fxt::get<0>(*exp2) << ", "
                  << fxt::get<1>(*exp2) << ", " << fxt::get<2>(*exp2) << ")\n\n";
    }

    // Prepend multiple plain values
    auto exp3 = fxt::mtuple_prepend(exp1, 3, 4, 5);
    std::cout << "   After mtuple_prepend(exp1, 3, 4, 5):\n";
    if (exp3) {
        std::cout << "   Result: (" << fxt::get<0>(*exp3) << ", "
                  << fxt::get<1>(*exp3) << ", " << fxt::get<2>(*exp3)
                  << ", " << fxt::get<3>(*exp3) << ", " << fxt::get<4>(*exp3) << ")\n\n";
    }

    // Prepend expected<value> to expected<tuple>
    auto exp_value = fxt::expected<int, std::string>{10};
    auto exp4 = fxt::mtuple_prepend(exp1, exp_value);
    std::cout << "   After mtuple_prepend(exp1, expected<int>{10}):\n";
    if (exp4) {
        std::cout << "   Result: (" << fxt::get<0>(*exp4) << ", "
                  << fxt::get<1>(*exp4) << ", " << fxt::get<2>(*exp4) << ")\n\n";
    }

    // Error propagation - tuple contains error
    auto exp_err = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected{"Error in tuple"}};
    auto exp5 = fxt::mtuple_prepend(exp_err, 3);
    std::cout << "   Error propagation - error in tuple:\n";
    if (!exp5) {
        std::cout << "   Error: " << exp5.error() << "\n\n";
    }

    // Error propagation - value contains error
    auto exp_value_err = fxt::expected<int, std::string>{fxt::unexpected{"Error in value"}};
    auto exp6 = fxt::mtuple_prepend(exp1, exp_value_err);
    std::cout << "   Error propagation - error in value:\n";
    if (!exp6) {
        std::cout << "   Error: " << exp6.error() << "\n\n";
    }
}

// ============================================================================
// Example 10: Monadic Tuple Prepend with fxt::expected - Pipe Operator
// ============================================================================

void example_mtuple_prepend_expected_pipe() {
    std::cout << "Example 10: Monadic Tuple Prepend with fxt::expected - Pipe Operator\n";
    std::cout << "======================================================================\n\n";

    // Pipe with single plain value
    auto exp1 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{1, 2}};
    auto result1 = exp1 | fxt::mtuple_prepend(3);
    std::cout << "   expected<(1, 2)> | mtuple_prepend(3):\n";
    if (result1) {
        std::cout << "   Result: (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // Pipe with multiple plain values
    auto result2 = exp1 | fxt::mtuple_prepend(3, 4, 5);
    std::cout << "   expected<(1, 2)> | mtuple_prepend(3, 4, 5):\n";
    if (result2) {
        std::cout << "   Result: (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2)
                  << ", " << fxt::get<3>(*result2) << ", " << fxt::get<4>(*result2) << ")\n\n";
    }

    // Pipe with expected<value>
    auto exp_value = fxt::expected<int, std::string>{10};
    auto result3 = exp1 | fxt::mtuple_prepend(exp_value);
    std::cout << "   expected<(1, 2)> | mtuple_prepend(expected<10>):\n";
    if (result3) {
        std::cout << "   Result: (" << fxt::get<0>(*result3) << ", "
                  << fxt::get<1>(*result3) << ", " << fxt::get<2>(*result3) << ")\n\n";
    }

    // Chaining multiple prepends
    auto result4 = exp1
        | fxt::mtuple_prepend(6)
        | fxt::mtuple_prepend(4, 5)
        | fxt::mtuple_prepend(3);
    std::cout << "   Chaining: expected<(1, 2)> | mtuple_prepend(6) | mtuple_prepend(4, 5) | mtuple_prepend(3):\n";
    if (result4) {
        std::cout << "   Result: (" << fxt::get<0>(*result4) << ", "
                  << fxt::get<1>(*result4) << ", " << fxt::get<2>(*result4)
                  << ", " << fxt::get<3>(*result4) << ", " << fxt::get<4>(*result4)
                  << ", " << fxt::get<5>(*result4) << ")\n\n";
    }
}

// ============================================================================
// Example 11: Monadic Tuple Prepend with fxt::optional - Direct Call
// ============================================================================

void example_mtuple_prepend_optional_direct() {
    std::cout << "Example 11: Monadic Tuple Prepend with fxt::optional - Direct Call\n";
    std::cout << "===================================================================\n\n";

    // Prepend a plain value to optional<tuple>
    auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{1, 2}};
    std::cout << "   Initial: optional<tuple<int, int>> containing (1, 2)\n";

    auto opt2 = fxt::mtuple_prepend(opt1, 3);
    std::cout << "   After mtuple_prepend(opt1, 3):\n";
    if (opt2) {
        std::cout << "   Result: (" << fxt::get<0>(*opt2) << ", "
                  << fxt::get<1>(*opt2) << ", " << fxt::get<2>(*opt2) << ")\n\n";
    }

    // Prepend multiple plain values
    auto opt3 = fxt::mtuple_prepend(opt1, 3, 4, 5);
    std::cout << "   After mtuple_prepend(opt1, 3, 4, 5):\n";
    if (opt3) {
        std::cout << "   Result: (" << fxt::get<0>(*opt3) << ", "
                  << fxt::get<1>(*opt3) << ", " << fxt::get<2>(*opt3)
                  << ", " << fxt::get<3>(*opt3) << ", " << fxt::get<4>(*opt3) << ")\n\n";
    }

    // Prepend optional<value> to optional<tuple>
    auto opt_value = fxt::optional<int>{10};
    auto opt4 = fxt::mtuple_prepend(opt1, opt_value);
    std::cout << "   After mtuple_prepend(opt1, optional<int>{10}):\n";
    if (opt4) {
        std::cout << "   Result: (" << fxt::get<0>(*opt4) << ", "
                  << fxt::get<1>(*opt4) << ", " << fxt::get<2>(*opt4) << ")\n\n";
    }

    // Nullopt propagation - tuple is nullopt
    auto opt_empty = fxt::optional<fxt::tuple<int, int>>{};
    auto opt5 = fxt::mtuple_prepend(opt_empty, 3);
    std::cout << "   Nullopt propagation - empty tuple:\n";
    if (!opt5) {
        std::cout << "   Result: nullopt\n\n";
    }

    // Nullopt propagation - value is nullopt
    auto opt_value_empty = fxt::optional<int>{};
    auto opt6 = fxt::mtuple_prepend(opt1, opt_value_empty);
    std::cout << "   Nullopt propagation - empty value:\n";
    if (!opt6) {
        std::cout << "   Result: nullopt\n\n";
    }
}

// ============================================================================
// Example 12: Monadic Tuple Prepend with fxt::optional - Pipe Operator
// ============================================================================

void example_mtuple_prepend_optional_pipe() {
    std::cout << "Example 12: Monadic Tuple Prepend with fxt::optional - Pipe Operator\n";
    std::cout << "=====================================================================\n\n";

    // Pipe with single plain value
    auto opt1 = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{1, 2}};
    auto result1 = opt1 | fxt::mtuple_prepend(3);
    std::cout << "   optional<(1, 2)> | mtuple_prepend(3):\n";
    if (result1) {
        std::cout << "   Result: (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // Pipe with multiple plain values
    auto result2 = opt1 | fxt::mtuple_prepend(3, 4, 5);
    std::cout << "   optional<(1, 2)> | mtuple_prepend(3, 4, 5):\n";
    if (result2) {
        std::cout << "   Result: (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2)
                  << ", " << fxt::get<3>(*result2) << ", " << fxt::get<4>(*result2) << ")\n\n";
    }

    // Pipe with optional<value>
    auto opt_value = fxt::optional<int>{10};
    auto result3 = opt1 | fxt::mtuple_prepend(opt_value);
    std::cout << "   optional<(1, 2)> | mtuple_prepend(optional<10>):\n";
    if (result3) {
        std::cout << "   Result: (" << fxt::get<0>(*result3) << ", "
                  << fxt::get<1>(*result3) << ", " << fxt::get<2>(*result3) << ")\n\n";
    }

    // Chaining multiple prepends
    auto result4 = opt1
        | fxt::mtuple_prepend(6)
        | fxt::mtuple_prepend(4, 5)
        | fxt::mtuple_prepend(3);
    std::cout << "   Chaining: optional<(1, 2)> | mtuple_prepend(6) | mtuple_prepend(4, 5) | mtuple_prepend(3):\n";
    if (result4) {
        std::cout << "   Result: (" << fxt::get<0>(*result4) << ", "
                  << fxt::get<1>(*result4) << ", " << fxt::get<2>(*result4)
                  << ", " << fxt::get<3>(*result4) << ", " << fxt::get<4>(*result4)
                  << ", " << fxt::get<5>(*result4) << ")\n\n";
    }
}

// ============================================================================
// Example 13: Monadic Tuple Prepend with fxt::flat_tuple
// ============================================================================

void example_mtuple_prepend_flat_tuple() {
    std::cout << "Example 13: Monadic Tuple Prepend with fxt::flat_tuple\n";
    std::cout << "=======================================================\n\n";

    // expected<flat_tuple> with direct call
    std::cout << "   Using fxt::expected with fxt::flat_tuple:\n";
    auto exp_flat = fxt::expected<fxt::flat_tuple<double, double>, std::string>{
        fxt::flat_tuple<double, double>{1.0, 2.0}
    };
    auto result1 = fxt::mtuple_prepend(exp_flat, 3.0);
    if (result1) {
        std::cout << "   mtuple_prepend(expected<(1.0, 2.0)>, 3.0) = ("
                  << fxt::get<0>(*result1) << ", " << fxt::get<1>(*result1)
                  << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // expected<flat_tuple> with pipe operator
    auto result2 = exp_flat | fxt::mtuple_prepend(3.0, 4.0);
    if (result2) {
        std::cout << "   expected<(1.0, 2.0)> | mtuple_prepend(3.0, 4.0) = ("
                  << fxt::get<0>(*result2) << ", " << fxt::get<1>(*result2)
                  << ", " << fxt::get<2>(*result2) << ", " << fxt::get<3>(*result2) << ")\n\n";
    }

    // optional<flat_tuple> with direct call
    std::cout << "   Using fxt::optional with fxt::flat_tuple:\n";
    auto opt_flat = fxt::optional<fxt::flat_tuple<double, double>>{
        fxt::flat_tuple<double, double>{5.0, 6.0}
    };
    auto result3 = fxt::mtuple_prepend(opt_flat, 7.0);
    if (result3) {
        std::cout << "   mtuple_prepend(optional<(5.0, 6.0)>, 7.0) = ("
                  << fxt::get<0>(*result3) << ", " << fxt::get<1>(*result3)
                  << ", " << fxt::get<2>(*result3) << ")\n\n";
    }

    // optional<flat_tuple> with pipe operator and chaining
    auto result4 = opt_flat
        | fxt::mtuple_prepend(7.0)
        | fxt::mtuple_prepend(8.0, 9.0);
    if (result4) {
        std::cout << "   optional<(5.0, 6.0)> | mtuple_prepend(7.0) | mtuple_prepend(8.0, 9.0) = ("
                  << fxt::get<0>(*result4) << ", " << fxt::get<1>(*result4)
                  << ", " << fxt::get<2>(*result4) << ", " << fxt::get<3>(*result4)
                  << ", " << fxt::get<4>(*result4) << ")\n\n";
    }
}

// ============================================================================
// Example 14: Complex Monadic Prepend Scenarios
// ============================================================================

void example_mtuple_prepend_complex() {
    std::cout << "Example 14: Complex Monadic Prepend Scenarios\n";
    std::cout << "==============================================\n\n";

    // Building a data packet with header information
    std::cout << "   Building a network packet with header:\n";
    auto payload = fxt::expected<fxt::tuple<int, std::string>, std::string>{
        fxt::tuple{200, std::string{"OK"}}
    };
    auto protocol = fxt::expected<std::string, std::string>{std::string{"HTTP/1.1"}};
    auto request_id = fxt::expected<int, std::string>{12345};

    auto packet = payload
        | fxt::mtuple_prepend(protocol)
        | fxt::mtuple_prepend(request_id);

    std::cout << "   Request ID: 12345\n";
    std::cout << "   Protocol: HTTP/1.1\n";
    std::cout << "   Status: 200 OK\n";
    if (packet) {
        std::cout << "   Final Packet: (" << fxt::get<0>(*packet) << ", \""
                  << fxt::get<1>(*packet) << "\", " << fxt::get<2>(*packet)
                  << ", \"" << fxt::get<3>(*packet) << "\")\n\n";
    }

    // Mixing plain values and monadic values
    std::cout << "   Mixing plain and monadic values:\n";
    auto base = fxt::optional<fxt::tuple<std::string>>{fxt::tuple{std::string{"Base"}}};
    auto value1 = fxt::optional<int>{42};

    auto mixed = base
        | fxt::mtuple_prepend(true)            // Plain value
        | fxt::mtuple_prepend(3.14)            // Plain value
        | fxt::mtuple_prepend(value1);         // Monadic value

    if (mixed) {
        std::cout << "   Result: (" << fxt::get<0>(*mixed) << ", "
                  << fxt::get<1>(*mixed) << ", " << std::boolalpha << fxt::get<2>(*mixed)
                  << ", \"" << fxt::get<3>(*mixed) << "\")\n\n";
    }

    // Error handling in a computation pipeline
    std::cout << "   Error handling in pipeline:\n";
    auto compute_value = [](int x) -> fxt::expected<int, std::string> {
        if (x < 0) return fxt::unexpected{"Negative value"};
        return x * 2;
    };

    auto pipeline1 = fxt::expected<fxt::tuple<int>, std::string>{fxt::tuple{5}}
        | fxt::mtuple_prepend(compute_value(15))
        | fxt::mtuple_prepend(compute_value(10));

    if (pipeline1) {
        std::cout << "   Success: (" << fxt::get<0>(*pipeline1) << ", "
                  << fxt::get<1>(*pipeline1) << ", " << fxt::get<2>(*pipeline1) << ")\n";
    }

    auto pipeline2 = fxt::expected<fxt::tuple<int>, std::string>{fxt::tuple{5}}
        | fxt::mtuple_prepend(compute_value(-5))  // This will fail
        | fxt::mtuple_prepend(compute_value(10));

    if (!pipeline2) {
        std::cout << "   Error: " << pipeline2.error() << "\n\n";
    }
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n=== fxt::tuple_prepend Demo ===\n\n";

    example_tuple_direct();
    example_tuple_pipe();
    example_flat_tuple_direct();
    example_flat_tuple_pipe();
    example_mixed_types();
    example_building_with_headers();
    example_multiple_values();
    example_mixed_prepends();

    std::cout << "\n=== fxt::mtuple_prepend Demo ===\n\n";

    example_mtuple_prepend_expected_direct();
    example_mtuple_prepend_expected_pipe();
    example_mtuple_prepend_optional_direct();
    example_mtuple_prepend_optional_pipe();
    example_mtuple_prepend_flat_tuple();
    example_mtuple_prepend_complex();

    std::cout << "=== Demo Complete ===\n\n";
    return 0;
}

