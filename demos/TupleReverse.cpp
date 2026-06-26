//
// Demo: fxt::tuple_reverse
//
// This demo shows how to use fxt::tuple_reverse to reverse the order of elements in
// fxt::tuple and fxt::flat_tuple, both with direct calls and using the pipe operator.
// It also demonstrates fxt::mtuple_reverse for monadic tuple reversal.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// ============================================================================
// Example 1: Using tuple_reverse with fxt::tuple - Direct Call
// ============================================================================

void example_tuple_direct() {
    std::cout << "Example 1: Direct Call with fxt::tuple\n";
    std::cout << "=======================================\n\n";

    // Reverse a simple tuple
    auto tuple1 = fxt::make_tuple(1, 2, 3);
    std::cout << "   Initial tuple: (1, 2, 3)\n";

    auto reversed1 = fxt::tuple_reverse(tuple1);
    std::cout << "   After reverse: (3, 2, 1)\n";
    std::cout << "   Values: (" << fxt::get<0>(reversed1) << ", "
              << fxt::get<1>(reversed1) << ", " << fxt::get<2>(reversed1) << ")\n\n";

    // Reverse a tuple with mixed types
    auto tuple2 = fxt::make_tuple(42, std::string{"hello"}, 3.14);
    std::cout << "   Initial tuple with mixed types: (42, \"hello\", 3.14)\n";

    auto reversed2 = fxt::tuple_reverse(tuple2);
    std::cout << "   After reverse: (3.14, \"hello\", 42)\n";
    std::cout << "   Values: (" << fxt::get<0>(reversed2) << ", \""
              << fxt::get<1>(reversed2) << "\", " << fxt::get<2>(reversed2) << ")\n\n";

    // Reverse a single-element tuple (no change)
    auto tuple3 = fxt::make_tuple(std::string{"only"});
    std::cout << "   Single-element tuple: (\"only\")\n";

    auto reversed3 = fxt::tuple_reverse(tuple3);
    std::cout << "   After reverse: (\"only\")\n";
    std::cout << "   Value: \"" << fxt::get<0>(reversed3) << "\"\n\n";
}

// ============================================================================
// Example 2: Using tuple_reverse with fxt::tuple - Pipe Operator
// ============================================================================

void example_tuple_pipe() {
    std::cout << "Example 2: Pipe Operator with fxt::tuple\n";
    std::cout << "=========================================\n\n";

    // Simple pipe usage
    auto tuple1 = fxt::make_tuple(1, 2, 3, 4);
    std::cout << "   Initial tuple: (1, 2, 3, 4)\n";

    auto result1 = tuple1 | fxt::tuple_reverse();
    std::cout << "   After | tuple_reverse(): (4, 3, 2, 1)\n";
    std::cout << "   Values: (" << fxt::get<0>(result1) << ", "
              << fxt::get<1>(result1) << ", " << fxt::get<2>(result1)
              << ", " << fxt::get<3>(result1) << ")\n\n";

    // Chaining with other operations
    auto tuple2 = fxt::make_tuple(10, 20, 30, 40, 50);
    std::cout << "   Chaining operations:\n";
    std::cout << "   Initial: (10, 20, 30, 40, 50)\n";

    auto result2 = tuple2
        | fxt::tuple_take<3>()
        | fxt::tuple_reverse();

    std::cout << "   After | take<3>() | tuple_reverse(): (30, 20, 10)\n";
    std::cout << "   Values: (" << fxt::get<0>(result2) << ", "
              << fxt::get<1>(result2) << ", " << fxt::get<2>(result2) << ")\n\n";

    // Reverse twice (returns to original)
    auto tuple3 = fxt::make_tuple(std::string{"a"}, std::string{"b"}, std::string{"c"});
    std::cout << "   Double reverse:\n";
    std::cout << "   Initial: (\"a\", \"b\", \"c\")\n";

    auto result3 = tuple3
        | fxt::tuple_reverse()
        | fxt::tuple_reverse();

    std::cout << "   After | tuple_reverse() | tuple_reverse(): (\"a\", \"b\", \"c\")\n";
    std::cout << "   Values: (\"" << fxt::get<0>(result3) << "\", \""
              << fxt::get<1>(result3) << "\", \"" << fxt::get<2>(result3) << "\")\n\n";
}

// ============================================================================
// Example 3: Using tuple_reverse with fxt::flat_tuple - Direct Call
// ============================================================================

void example_flat_tuple_direct() {
    std::cout << "Example 3: Direct Call with fxt::flat_tuple\n";
    std::cout << "============================================\n\n";

    // Reverse a flat_tuple of doubles
    auto flat1 = fxt::flat_tuple<double, double, double>{1.0, 2.0, 3.0};
    std::cout << "   Initial flat_tuple: (1.0, 2.0, 3.0)\n";

    auto reversed1 = fxt::tuple_reverse(flat1);
    std::cout << "   After reverse: (3.0, 2.0, 1.0)\n";
    std::cout << "   Values: (" << fxt::get<0>(reversed1) << ", "
              << fxt::get<1>(reversed1) << ", " << fxt::get<2>(reversed1) << ")\n\n";

    // Reverse a flat_tuple of integers
    auto flat2 = fxt::flat_tuple<int, int, int, int, int>{10, 20, 30, 40, 50};
    std::cout << "   Initial flat_tuple: (10, 20, 30, 40, 50)\n";

    auto reversed2 = fxt::tuple_reverse(flat2);
    std::cout << "   After reverse: (50, 40, 30, 20, 10)\n";
    std::cout << "   Values: (" << fxt::get<0>(reversed2) << ", "
              << fxt::get<1>(reversed2) << ", " << fxt::get<2>(reversed2)
              << ", " << fxt::get<3>(reversed2) << ", " << fxt::get<4>(reversed2) << ")\n\n";
}

// ============================================================================
// Example 4: Using tuple_reverse with fxt::flat_tuple - Pipe Operator
// ============================================================================

void example_flat_tuple_pipe() {
    std::cout << "Example 4: Pipe Operator with fxt::flat_tuple\n";
    std::cout << "==============================================\n\n";

    // Simple pipe usage
    auto flat1 = fxt::flat_tuple<double, double, double, double>{1.5, 2.5, 3.5, 4.5};
    std::cout << "   Initial flat_tuple: (1.5, 2.5, 3.5, 4.5)\n";

    auto result1 = flat1 | fxt::tuple_reverse();
    std::cout << "   After | tuple_reverse(): (4.5, 3.5, 2.5, 1.5)\n";
    std::cout << "   Values: (" << fxt::get<0>(result1) << ", "
              << fxt::get<1>(result1) << ", " << fxt::get<2>(result1)
              << ", " << fxt::get<3>(result1) << ")\n\n";

    // Chaining with other operations
    auto flat2 = fxt::flat_tuple<int, int, int, int, int, int>{1, 2, 3, 4, 5, 6};
    std::cout << "   Chaining operations:\n";
    std::cout << "   Initial: (1, 2, 3, 4, 5, 6)\n";

    auto result2 = flat2
        | fxt::drop<2>()
        | fxt::tuple_reverse();

    std::cout << "   After | drop<2>() | tuple_reverse(): (6, 5, 4, 3)\n";
    std::cout << "   Values: (" << fxt::get<0>(result2) << ", "
              << fxt::get<1>(result2) << ", " << fxt::get<2>(result2)
              << ", " << fxt::get<3>(result2) << ")\n\n";
}

// ============================================================================
// Example 5: Complex Chaining with Mixed Operations
// ============================================================================

void example_complex_chaining() {
    std::cout << "Example 5: Complex Chaining with Mixed Operations\n";
    std::cout << "==================================================\n\n";

    // Build and manipulate a tuple
    auto initial = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8);
    std::cout << "   Initial tuple: (1, 2, 3, 4, 5, 6, 7, 8)\n";

    auto result = initial
        | fxt::drop<2>()           // Drop first 2: (3, 4, 5, 6, 7, 8)
        | fxt::tuple_take<4>()           // Take first 4: (3, 4, 5, 6)
        | fxt::tuple_reverse();    // Reverse: (6, 5, 4, 3)

    std::cout << "   After | drop<2>() | take<4>() | tuple_reverse():\n";
    std::cout << "   Result: (6, 5, 4, 3)\n";
    std::cout << "   Values: (" << fxt::get<0>(result) << ", "
              << fxt::get<1>(result) << ", " << fxt::get<2>(result)
              << ", " << fxt::get<3>(result) << ")\n\n";

    // Reverse then transform
    auto tuple2 = fxt::make_tuple(1, 2, 3);
    std::cout << "   Reverse then transform:\n";
    std::cout << "   Initial: (1, 2, 3)\n";

    auto result2 = tuple2
        | fxt::tuple_reverse()
        | fxt::tuple_transform([](auto x) { return x * 10; });

    std::cout << "   After | tuple_reverse() | transform(*10): (30, 20, 10)\n";
    std::cout << "   Values: (" << fxt::get<0>(result2) << ", "
              << fxt::get<1>(result2) << ", " << fxt::get<2>(result2) << ")\n\n";
}

// ============================================================================
// Example 6: Palindrome Check Using Double Reverse
// ============================================================================

void example_palindrome_check() {
    std::cout << "Example 6: Palindrome Check Using Double Reverse\n";
    std::cout << "=================================================\n\n";

    // A palindromic tuple
    auto palindrome = fxt::make_tuple(1, 2, 3, 2, 1);
    std::cout << "   Palindromic tuple: (1, 2, 3, 2, 1)\n";

    auto reversed = palindrome | fxt::tuple_reverse();
    std::cout << "   After reverse: (" << fxt::get<0>(reversed) << ", "
              << fxt::get<1>(reversed) << ", " << fxt::get<2>(reversed) << ", "
              << fxt::get<3>(reversed) << ", " << fxt::get<4>(reversed) << ")\n";
    std::cout << "   (Same as original - it's a palindrome!)\n\n";

    // A non-palindromic tuple
    auto non_palindrome = fxt::make_tuple(1, 2, 3, 4, 5);
    std::cout << "   Non-palindromic tuple: (1, 2, 3, 4, 5)\n";

    auto reversed2 = non_palindrome | fxt::tuple_reverse();
    std::cout << "   After reverse: (" << fxt::get<0>(reversed2) << ", "
              << fxt::get<1>(reversed2) << ", " << fxt::get<2>(reversed2) << ", "
              << fxt::get<3>(reversed2) << ", " << fxt::get<4>(reversed2) << ")\n";
    std::cout << "   (Different from original - not a palindrome)\n\n";
}

// ============================================================================
// Example 7: Monadic Tuple Reverse with fxt::expected - Direct Call
// ============================================================================

void example_mtuple_reverse_expected_direct() {
    std::cout << "Example 7: Monadic Tuple Reverse with fxt::expected - Direct Call\n";
    std::cout << "===================================================================\n\n";

    // Reverse a tuple inside expected
    auto exp1 = fxt::expected<fxt::tuple<int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3)
    };
    std::cout << "   Initial: expected<tuple<int, int, int>> containing (1, 2, 3)\n";

    auto exp2 = fxt::mtuple_reverse(exp1);
    std::cout << "   After mtuple_reverse(exp1):\n";
    if (exp2) {
        std::cout << "   Result: (" << fxt::get<0>(*exp2) << ", "
                  << fxt::get<1>(*exp2) << ", " << fxt::get<2>(*exp2) << ")\n\n";
    }

    // Error propagation
    auto exp_err = fxt::expected<fxt::tuple<int, int, int>, std::string>{
        fxt::unexpected{"Error occurred"}
    };
    std::cout << "   Error propagation test:\n";
    std::cout << "   Initial: expected with error \"Error occurred\"\n";

    auto exp_err_result = fxt::mtuple_reverse(exp_err);
    if (!exp_err_result) {
        std::cout << "   After mtuple_reverse(exp_err): Error: "
                  << exp_err_result.error() << "\n\n";
    }
}

// ============================================================================
// Example 8: Monadic Tuple Reverse with fxt::expected - Pipe Operator
// ============================================================================

void example_mtuple_reverse_expected_pipe() {
    std::cout << "Example 8: Monadic Tuple Reverse with fxt::expected - Pipe Operator\n";
    std::cout << "=====================================================================\n\n";

    // Simple pipe usage
    auto exp1 = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
        fxt::make_tuple(10, 20, 30, 40)
    };
    std::cout << "   Initial: expected<(10, 20, 30, 40)>\n";

    auto result1 = exp1 | fxt::mtuple_reverse();
    std::cout << "   After | mtuple_reverse():\n";
    if (result1) {
        std::cout << "   Result: (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1)
                  << ", " << fxt::get<3>(*result1) << ")\n\n";
    }

    // Chaining with other monadic operations
    auto exp2 = fxt::expected<fxt::tuple<int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3)
    };
    std::cout << "   Chaining monadic operations:\n";
    std::cout << "   Initial: expected<(1, 2, 3)>\n";

    auto result2 = exp2
        | fxt::mtuple_transform([](auto x) { return x * 2; })
        | fxt::mtuple_reverse();

    std::cout << "   After | mtransform_tuple(*2) | mtuple_reverse():\n";
    if (result2) {
        std::cout << "   Result: (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2) << ")\n";
        std::cout << "   (Transform gives (2, 4, 6), then reverse gives (6, 4, 2))\n\n";
    }

    // Complex chaining
    auto exp3 = fxt::expected<fxt::tuple<int, int, int, int, int>, std::string>{
        fxt::make_tuple(5, 4, 3, 2, 1)
    };
    std::cout << "   Complex chaining:\n";
    std::cout << "   Initial: expected<(5, 4, 3, 2, 1)>\n";

    auto result3 = exp3
        | fxt::mtuple_reverse()
        | fxt::mtuple_append(0)
        | fxt::mtuple_transform([](auto x) { return x + 10; });

    std::cout << "   After | mtuple_reverse() | mtuple_append(0) | mtransform_tuple(+10):\n";
    if (result3) {
        std::cout << "   Result: (" << fxt::get<0>(*result3) << ", "
                  << fxt::get<1>(*result3) << ", " << fxt::get<2>(*result3)
                  << ", " << fxt::get<3>(*result3) << ", " << fxt::get<4>(*result3)
                  << ", " << fxt::get<5>(*result3) << ")\n";
        std::cout << "   (Reverse: (1,2,3,4,5), Append: (1,2,3,4,5,0), Transform: (11,12,13,14,15,10))\n\n";
    }
}

// ============================================================================
// Example 9: Monadic Tuple Reverse with fxt::optional - Direct Call
// ============================================================================

void example_mtuple_reverse_optional_direct() {
    std::cout << "Example 9: Monadic Tuple Reverse with fxt::optional - Direct Call\n";
    std::cout << "==================================================================\n\n";

    // Reverse a tuple inside optional
    auto opt1 = fxt::optional<fxt::tuple<int, int, int>>{
        fxt::make_tuple(1, 2, 3)
    };
    std::cout << "   Initial: optional<tuple<int, int, int>> containing (1, 2, 3)\n";

    auto opt2 = fxt::mtuple_reverse(opt1);
    std::cout << "   After mtuple_reverse(opt1):\n";
    if (opt2) {
        std::cout << "   Result: (" << fxt::get<0>(*opt2) << ", "
                  << fxt::get<1>(*opt2) << ", " << fxt::get<2>(*opt2) << ")\n\n";
    }

    // Nullopt propagation
    auto opt_empty = fxt::optional<fxt::tuple<int, int, int>>{};
    std::cout << "   Nullopt propagation test:\n";
    std::cout << "   Initial: optional with nullopt\n";

    auto opt_empty_result = fxt::mtuple_reverse(opt_empty);
    if (!opt_empty_result) {
        std::cout << "   After mtuple_reverse(opt_empty): nullopt\n\n";
    }
}

// ============================================================================
// Example 10: Monadic Tuple Reverse with fxt::optional - Pipe Operator
// ============================================================================

void example_mtuple_reverse_optional_pipe() {
    std::cout << "Example 10: Monadic Tuple Reverse with fxt::optional - Pipe Operator\n";
    std::cout << "======================================================================\n\n";

    // Simple pipe usage
    auto opt1 = fxt::optional<fxt::tuple<int, int, int, int>>{
        fxt::make_tuple(100, 200, 300, 400)
    };
    std::cout << "   Initial: optional<(100, 200, 300, 400)>\n";

    auto result1 = opt1 | fxt::mtuple_reverse();
    std::cout << "   After | mtuple_reverse():\n";
    if (result1) {
        std::cout << "   Result: (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1)
                  << ", " << fxt::get<3>(*result1) << ")\n\n";
    }

    // Chaining with other monadic operations
    auto opt2 = fxt::optional<fxt::tuple<int, int, int>>{
        fxt::make_tuple(10, 20, 30)
    };
    std::cout << "   Chaining monadic operations:\n";
    std::cout << "   Initial: optional<(10, 20, 30)>\n";

    auto result2 = opt2
        | fxt::mtuple_reverse()
        | fxt::mtuple_transform([](auto x) { return x / 10; });

    std::cout << "   After | mtuple_reverse() | mtransform_tuple(/10):\n";
    if (result2) {
        std::cout << "   Result: (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2) << ")\n";
        std::cout << "   (Reverse gives (30, 20, 10), then divide by 10 gives (3, 2, 1))\n\n";
    }
}

// ============================================================================
// Example 11: Monadic Tuple Reverse with fxt::flat_tuple
// ============================================================================

void example_mtuple_reverse_flat_tuple() {
    std::cout << "Example 11: Monadic Tuple Reverse with fxt::flat_tuple\n";
    std::cout << "=======================================================\n\n";

    // expected<flat_tuple>
    std::cout << "   Using fxt::expected with fxt::flat_tuple:\n";
    auto exp_flat = fxt::expected<fxt::flat_tuple<double, double, double>, std::string>{
        fxt::flat_tuple<double, double, double>{1.1, 2.2, 3.3}
    };
    std::cout << "   Initial: expected<flat_tuple<double, double, double>> containing (1.1, 2.2, 3.3)\n";

    auto result1 = exp_flat | fxt::mtuple_reverse();
    if (result1) {
        std::cout << "   After | mtuple_reverse(): (" << fxt::get<0>(*result1) << ", "
                  << fxt::get<1>(*result1) << ", " << fxt::get<2>(*result1) << ")\n\n";
    }

    // optional<flat_tuple>
    std::cout << "   Using fxt::optional with fxt::flat_tuple:\n";
    auto opt_flat = fxt::optional<fxt::flat_tuple<int, int, int, int>>{
        fxt::flat_tuple<int, int, int, int>{7, 8, 9, 10}
    };
    std::cout << "   Initial: optional<flat_tuple<int, int, int, int>> containing (7, 8, 9, 10)\n";

    auto result2 = opt_flat | fxt::mtuple_reverse();
    if (result2) {
        std::cout << "   After | mtuple_reverse(): (" << fxt::get<0>(*result2) << ", "
                  << fxt::get<1>(*result2) << ", " << fxt::get<2>(*result2)
                  << ", " << fxt::get<3>(*result2) << ")\n\n";
    }
}

// ============================================================================
// Example 12: Practical Use Cases
// ============================================================================

void example_practical_use_cases() {
    std::cout << "Example 12: Practical Use Cases\n";
    std::cout << "================================\n\n";

    // Use case 1: Processing data in reverse order
    std::cout << "   Use Case 1: Processing timestamps in reverse chronological order\n";
    auto timestamps = fxt::make_tuple(
        std::string{"2025-01-01"},
        std::string{"2025-01-02"},
        std::string{"2025-01-03"}
    );
    std::cout << "   Forward order: (\"" << fxt::get<0>(timestamps) << "\", \""
              << fxt::get<1>(timestamps) << "\", \"" << fxt::get<2>(timestamps) << "\")\n";

    auto reverse_chrono = timestamps | fxt::tuple_reverse();
    std::cout << "   Reverse chronological: (\"" << fxt::get<0>(reverse_chrono) << "\", \""
              << fxt::get<1>(reverse_chrono) << "\", \"" << fxt::get<2>(reverse_chrono) << "\")\n\n";

    // Use case 2: Stack-like operations (LIFO)
    std::cout << "   Use Case 2: Stack-like operations (Last In, First Out)\n";
    auto stack = fxt::flat_tuple<int, int, int, int>{1, 2, 3, 4};
    std::cout << "   Stack (top is last): (" << fxt::get<0>(stack) << ", "
              << fxt::get<1>(stack) << ", " << fxt::get<2>(stack)
              << ", " << fxt::get<3>(stack) << ")\n";

    auto reversed_stack = stack | fxt::tuple_reverse();
    std::cout << "   After reverse (top is first): (" << fxt::get<0>(reversed_stack) << ", "
              << fxt::get<1>(reversed_stack) << ", " << fxt::get<2>(reversed_stack)
              << ", " << fxt::get<3>(reversed_stack) << ")\n\n";

    // Use case 3: Mirror/symmetric transformations
    std::cout << "   Use Case 3: Creating mirror-symmetric data\n";
    auto half = fxt::make_tuple(1, 2, 3);
    auto reversed_half = half | fxt::tuple_reverse();
    std::cout << "   Original half: (" << fxt::get<0>(half) << ", "
              << fxt::get<1>(half) << ", " << fxt::get<2>(half) << ")\n";
    std::cout << "   Reversed half: (" << fxt::get<0>(reversed_half) << ", "
              << fxt::get<1>(reversed_half) << ", " << fxt::get<2>(reversed_half) << ")\n";
    std::cout << "   (Could be concatenated to form symmetric structure)\n\n";
}

// ============================================================================
// Main Function
// ============================================================================

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                  FXT Tuple Reverse Demo                        ║\n";
    std::cout << "║                                                                ║\n";
    std::cout << "║  Demonstrates fxt::tuple_reverse and fxt::mtuple_reverse       ║\n";
    std::cout << "║  with fxt::tuple and fxt::flat_tuple                           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════════╝\n";
    std::cout << "\n";

    example_tuple_direct();
    example_tuple_pipe();
    example_flat_tuple_direct();
    example_flat_tuple_pipe();
    example_complex_chaining();
    example_palindrome_check();
    example_mtuple_reverse_expected_direct();
    example_mtuple_reverse_expected_pipe();
    example_mtuple_reverse_optional_direct();
    example_mtuple_reverse_optional_pipe();
    example_mtuple_reverse_flat_tuple();
    example_practical_use_cases();

    std::cout << "═══════════════════════════════════════════════════════════════════\n";
    std::cout << "Demo completed successfully!\n";
    std::cout << "═══════════════════════════════════════════════════════════════════\n\n";

    return 0;
}

