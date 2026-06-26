//
// Demo: fxt::select
//
// This demo shows how to use fxt::select to extract multiple elements from tuples
// within monadic containers (fxt::expected and fxt::optional) using the pipe operator.
//

#include <iostream>
#include <string>
#include <fxt.hpp>

int main()
{
    std::cout << "=== fxt::select Demo ===" << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Part 1: Using select with fxt::expected - by index
    // =========================================================================
    std::cout << "Part 1: Using select with fxt::expected - by index" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    // Create an expected containing a tuple with 5 elements
    auto exp1 = fxt::expected<std::tuple<int, double, std::string, bool, char>, std::string>{
        std::make_tuple(42, 3.14, "hello", true, 'A')
    };

    std::cout << "Original tuple: (42, 3.14, \"hello\", true, 'A')" << std::endl;

    // Select elements at indices 0 and 2
    auto result1 = exp1 | fxt::mtuple_select<0, 2>();
    if (result1) {
        auto [i, s] = *result1;
        std::cout << "Selected indices 0, 2: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select elements at indices 1, 3, and 4
    auto result2 = exp1 | fxt::mtuple_select<1, 3, 4>();
    if (result2) {
        auto [d, b, c] = *result2;
        std::cout << "Selected indices 1, 3, 4: (" << d << ", " << std::boolalpha << b << ", '" << c << "')" << std::endl;
    }

    // Select all elements in different order
    std::cout << "\nReordering elements:" << std::endl;
    auto reordered = exp1 | fxt::mtuple_select<4, 2, 0>();
    if (reordered) {
        auto [c, s, i] = *reordered;
        std::cout << "Selected indices 4, 2, 0: ('" << c << "', \"" << s << "\", " << i << ")" << std::endl;
    }

    // =========================================================================
    // Part 2: Using select with fxt::expected - by type
    // =========================================================================
    std::cout << "\n\nPart 2: Using select with fxt::expected - by type" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    auto exp2 = fxt::expected<std::tuple<int, double, std::string, bool>, std::string>{
        std::make_tuple(99, 2.71, "world", false)
    };

    std::cout << "Original tuple: (99, 2.71, \"world\", false)" << std::endl;

    // Select elements by type
    auto type_result1 = exp2 | fxt::mtuple_select<int, std::string>();
    if (type_result1) {
        auto [i, s] = *type_result1;
        std::cout << "Selected int, string: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select different types
    auto type_result2 = exp2 | fxt::mtuple_select<double, bool>();
    if (type_result2) {
        auto [d, b] = *type_result2;
        std::cout << "Selected double, bool: (" << d << ", " << std::boolalpha << b << ")" << std::endl;
    }

    // =========================================================================
    // Part 3: Error propagation with fxt::expected
    // =========================================================================
    std::cout << "\n\nPart 3: Error propagation with fxt::expected" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    auto exp_error = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        fxt::unexpected("Error occurred")
    };

    auto error_result = exp_error | fxt::mtuple_select<0, 2>();

    if (error_result) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error propagated: " << error_result.error() << std::endl;
    }

    // =========================================================================
    // Part 4: Using select with fxt::optional - by index
    // =========================================================================
    std::cout << "\n\nPart 4: Using select with fxt::optional - by index" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    // Create an optional containing a tuple
    auto opt1 = fxt::optional<std::tuple<int, double, std::string, bool, char>>{
        std::make_tuple(77, 1.41, "optional", true, 'Z')
    };

    std::cout << "Original tuple: (77, 1.41, \"optional\", true, 'Z')" << std::endl;

    // Select elements at indices 0 and 2
    auto opt_result1 = opt1 | fxt::mtuple_select<0, 2>();
    if (opt_result1) {
        auto [i, s] = *opt_result1;
        std::cout << "Selected indices 0, 2: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select elements at indices 1, 3, and 4
    auto opt_result2 = opt1 | fxt::mtuple_select<1, 3, 4>();
    if (opt_result2) {
        auto [d, b, c] = *opt_result2;
        std::cout << "Selected indices 1, 3, 4: (" << d << ", " << std::boolalpha << b << ", '" << c << "')" << std::endl;
    }

    // Select in reverse order
    std::cout << "\nReversing elements:" << std::endl;
    auto reversed = opt1 | fxt::mtuple_select<4, 3, 2, 1, 0>();
    if (reversed) {
        auto [c, b, s, d, i] = *reversed;
        std::cout << "Selected indices 4, 3, 2, 1, 0: ('" << c << "', " << std::boolalpha << b
                  << ", \"" << s << "\", " << d << ", " << i << ")" << std::endl;
    }

    // =========================================================================
    // Part 5: Using select with fxt::optional - by type
    // =========================================================================
    std::cout << "\n\nPart 5: Using select with fxt::optional - by type" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    auto opt2 = fxt::optional<std::tuple<int, double, std::string, bool>>{
        std::make_tuple(55, 6.28, "type", true)
    };

    std::cout << "Original tuple: (55, 6.28, \"type\", true)" << std::endl;

    // Select elements by type
    auto opt_type_result1 = opt2 | fxt::mtuple_select<int, std::string>();
    if (opt_type_result1) {
        auto [i, s] = *opt_type_result1;
        std::cout << "Selected int, string: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select all types
    auto opt_type_result2 = opt2 | fxt::mtuple_select<double, bool, int>();
    if (opt_type_result2) {
        auto [d, b, i] = *opt_type_result2;
        std::cout << "Selected double, bool, int: (" << d << ", " << std::boolalpha << b << ", " << i << ")" << std::endl;
    }

    // =========================================================================
    // Part 6: Empty propagation with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 6: Empty propagation with fxt::optional" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    auto opt_empty = fxt::optional<std::tuple<int, double, std::string>>{};

    auto empty_result = opt_empty | fxt::mtuple_select<0, 2>();

    if (empty_result) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Empty optional propagated correctly" << std::endl;
    }

    // =========================================================================
    // Part 7: Building tuples with append and selecting elements
    // =========================================================================
    std::cout << "\n\nPart 7: Building tuples with append and selecting elements" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;

    auto built = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mtuple_append(100)
        | fxt::mtuple_append(200)
        | fxt::mtuple_append(300)
        | fxt::mtuple_append(400)
        | fxt::mtuple_append(500);

    std::cout << "Built tuple: (100, 200, 300, 400, 500)" << std::endl;

    // Select subset
    auto subset = built | fxt::mtuple_select<0, 2, 4>();
    if (subset) {
        auto [a, c, e] = *subset;
        std::cout << "Selected indices 0, 2, 4: (" << a << ", " << c << ", " << e << ")" << std::endl;
    }

    // =========================================================================
    // Part 8: Combining select with apply
    // =========================================================================
    std::cout << "\n\nPart 8: Combining select with apply" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    auto computed = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mtuple_append(5)
        | fxt::mtuple_append(10)
        | fxt::mtuple_append(15)
        | fxt::mtuple_apply_append([](int a, int b, int c) { return a + b + c; })
        | fxt::mtuple_apply_append([](int a, int b, int c, int sum) { return a * b * c; });

    std::cout << "Computed tuple: (5, 10, 15, 30, 750)" << std::endl;

    // Select specific computed values
    auto selected_computed = computed | fxt::mtuple_select<0, 3, 4>();
    if (selected_computed) {
        auto [first, sum, product] = *selected_computed;
        std::cout << "Selected first, sum, product: (" << first << ", " << sum << ", " << product << ")" << std::endl;
    }

    // =========================================================================
    // Part 9: Chaining select operations
    // =========================================================================
    std::cout << "\n\nPart 9: Chaining select operations" << std::endl;
    std::cout << "----------------------------------" << std::endl;

    auto original = fxt::optional<std::tuple<int, int, int, int, int>>{
        std::make_tuple(1, 2, 3, 4, 5)
    };

    std::cout << "Original tuple: (1, 2, 3, 4, 5)" << std::endl;

    // First selection
    auto first_select = original | fxt::mtuple_select<1, 2, 3>();
    if (first_select) {
        auto [a, b, c] = *first_select;
        std::cout << "After first select<1, 2, 3>: (" << a << ", " << b << ", " << c << ")" << std::endl;
    }

    // Second selection from the result
    auto second_select = first_select | fxt::mtuple_select<0, 2>();
    if (second_select) {
        auto [x, z] = *second_select;
        std::cout << "After second select<0, 2>: (" << x << ", " << z << ")" << std::endl;
    }

    // =========================================================================
    // Part 10: Direct call syntax (without pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 10: Direct call syntax (without pipe operator)" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    auto exp3 = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        std::make_tuple(999, 7.77, "direct")
    };

    // Using direct call syntax
    auto direct_result = fxt::mtuple_select<0, 2>()(exp3);
    if (direct_result) {
        auto [i, s] = *direct_result;
        std::cout << "Direct call select<0, 2>: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // =========================================================================
    // Part 11: Selecting single element (degenerate case)
    // =========================================================================
    std::cout << "\n\nPart 11: Selecting single element" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    auto tuple5 = fxt::optional<std::tuple<int, double, std::string>>{
        std::make_tuple(42, 3.14, "single")
    };

    // Select just one element (returns tuple with one element)
    auto single = tuple5 | fxt::mtuple_select<1>();
    if (single) {
        auto [d] = *single;
        std::cout << "Selected single element at index 1: (" << d << ")" << std::endl;
    }

    // =========================================================================
    // Part 12: Practical example - extracting configuration fields
    // =========================================================================
    std::cout << "\n\nPart 12: Practical example - extracting configuration" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    // Simulate a configuration tuple
    using Config = std::tuple<std::string, int, std::string, bool, int>;
    auto config = fxt::expected<Config, std::string>{
        std::make_tuple("localhost", 8080, "/api/v1", true, 60)
    };

    std::cout << "Configuration: (\"localhost\", 8080, \"/api/v1\", true, 60)" << std::endl;
    std::cout << "  [host, port, endpoint, ssl_enabled, timeout]" << std::endl;

    // Extract network settings (host, port)
    auto network_settings = config | fxt::mtuple_select<0, 1>();
    if (network_settings) {
        auto [host, port] = *network_settings;
        std::cout << "\nNetwork settings: " << host << ":" << port << std::endl;
    }

    // Extract API settings (endpoint, ssl_enabled)
    auto api_settings = config | fxt::mtuple_select<2, 3>();
    if (api_settings) {
        auto [endpoint, ssl] = *api_settings;
        std::cout << "API settings: " << endpoint << " (SSL: " << std::boolalpha << ssl << ")" << std::endl;
    }

    // Extract timeout setting
    auto timeout_setting = config | fxt::mtuple_select<4>();
    if (timeout_setting) {
        auto [timeout] = *timeout_setting;
        std::cout << "Timeout: " << timeout << " seconds" << std::endl;
    }

    // =========================================================================
    // Part 13: Selecting with duplicate indices
    // =========================================================================
    std::cout << "\n\nPart 13: Selecting with duplicate indices" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;

    auto tuple6 = fxt::optional<std::tuple<int, int, int>>{
        std::make_tuple(10, 20, 30)
    };

    std::cout << "Original tuple: (10, 20, 30)" << std::endl;

    // Select same element multiple times
    auto duplicates = tuple6 | fxt::mtuple_select<0, 0, 1, 0>();
    if (duplicates) {
        auto [a, b, c, d] = *duplicates;
        std::cout << "Selected indices 0, 0, 1, 0: (" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl;
    }

    // =========================================================================
    // Part 14: Using tuple_select on fxt::flat_tuple (by index)
    // =========================================================================
    std::cout << "\n\nPart 14: Using tuple_select on fxt::flat_tuple (by index)" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;

    auto ft1 = fxt::make_flat_tuple(100, 200, 300, 400, 500);
    std::cout << "Original flat_tuple: (100, 200, 300, 400, 500)" << std::endl;

    auto ft1_result = ft1 | fxt::tuple_select<0, 2, 4>();
    std::cout << "tuple_select<0, 2, 4>: ("
              << fxt::get<0>(ft1_result) << ", "
              << fxt::get<1>(ft1_result) << ", "
              << fxt::get<2>(ft1_result) << ")" << std::endl;

    auto ft1_result2 = ft1 | fxt::tuple_select<1, 3>();
    std::cout << "tuple_select<1, 3>: ("
              << fxt::get<0>(ft1_result2) << ", "
              << fxt::get<1>(ft1_result2) << ")" << std::endl;

    // =========================================================================
    // Part 15: Using tuple_select on fxt::flat_tuple (by index, mixed types)
    // =========================================================================
    std::cout << "\n\nPart 15: Using tuple_select on fxt::flat_tuple (by index, mixed types)" << std::endl;
    std::cout << "-----------------------------------------------------------------------" << std::endl;

    auto ft2 = fxt::make_flat_tuple(42, 3.14, std::string("hello"), 'X', true);
    std::cout << "Original flat_tuple: (42, 3.14, \"hello\", 'X', true)" << std::endl;

    auto ft2_result = ft2 | fxt::tuple_select<0, 2, 4>();
    std::cout << "tuple_select<0, 2, 4>: ("
              << fxt::get<0>(ft2_result) << ", \""
              << fxt::get<1>(ft2_result) << "\", "
              << std::boolalpha << fxt::get<2>(ft2_result) << ")" << std::endl;

    auto ft2_reordered = ft2 | fxt::tuple_select<4, 3, 2, 1, 0>();
    std::cout << "Reordered (4,3,2,1,0): ("
              << std::boolalpha << fxt::get<0>(ft2_reordered) << ", '"
              << fxt::get<1>(ft2_reordered) << "', \""
              << fxt::get<2>(ft2_reordered) << "\", "
              << fxt::get<3>(ft2_reordered) << ", "
              << fxt::get<4>(ft2_reordered) << ")" << std::endl;

    // =========================================================================
    // Part 16: Using fxt::select on fxt::tuple (direct call)
    // =========================================================================
    std::cout << "\n\nPart 16: Using fxt::select on fxt::tuple (direct call)" << std::endl;
    std::cout << "-------------------------------------------------------" << std::endl;

    auto t1 = fxt::make_tuple(10, 20, 30, 40, 50);
    std::cout << "Original fxt::tuple: (10, 20, 30, 40, 50)" << std::endl;

    // Direct call syntax
    auto t1_result = fxt::tuple_select<0, 2, 4>(t1);
    std::cout << "fxt::tuple_select<0, 2, 4>(t1): ("
              << std::get<0>(t1_result) << ", "
              << std::get<1>(t1_result) << ", "
              << std::get<2>(t1_result) << ")" << std::endl;

    // Select different elements
    auto t1_result2 = fxt::tuple_select<1, 3>(t1);
    std::cout << "fxt::tuple_select<1, 3>(t1): ("
              << std::get<0>(t1_result2) << ", "
              << std::get<1>(t1_result2) << ")" << std::endl;

    // =========================================================================
    // Part 17: Using fxt::select on fxt::tuple (pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 17: Using fxt::select on fxt::tuple (pipe operator)" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    auto t2 = fxt::make_tuple(100, 200, 300, 400, 500, 600);
    std::cout << "Original fxt::tuple: (100, 200, 300, 400, 500, 600)" << std::endl;

    // Using pipe operator
    auto t2_result = t2 | fxt::tuple_select<0, 2, 5>();
    std::cout << "t2 | fxt::tuple_select<0, 2, 5>(): ("
              << std::get<0>(t2_result) << ", "
              << std::get<1>(t2_result) << ", "
              << std::get<2>(t2_result) << ")" << std::endl;

    // Select single element
    auto t2_single = t2 | fxt::tuple_select<3>();
    std::cout << "t2 | fxt::tuple_select<3>(): ("
              << std::get<0>(t2_single) << ")" << std::endl;

    // Select all elements in reverse
    auto t2_reversed = t2 | fxt::tuple_select<5, 4, 3, 2, 1, 0>();
    std::cout << "t2 | fxt::tuple_select<5, 4, 3, 2, 1, 0>(): ("
              << std::get<0>(t2_reversed) << ", "
              << std::get<1>(t2_reversed) << ", "
              << std::get<2>(t2_reversed) << ", "
              << std::get<3>(t2_reversed) << ", "
              << std::get<4>(t2_reversed) << ", "
              << std::get<5>(t2_reversed) << ")" << std::endl;

    // =========================================================================
    // Part 18: Using fxt::select on fxt::tuple with mixed types
    // =========================================================================
    std::cout << "\n\nPart 18: Using fxt::select on fxt::tuple with mixed types" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;

    auto t3 = fxt::make_tuple(99, 7.77, std::string("world"), 'Z', false);
    std::cout << "Original fxt::tuple: (99, 7.77, \"world\", 'Z', false)" << std::endl;

    // Direct call with mixed types
    auto t3_result1 = fxt::tuple_select<0, 2>(t3);
    std::cout << "fxt::tuple_select<0, 2>(t3): ("
              << std::get<0>(t3_result1) << ", \""
              << std::get<1>(t3_result1) << "\")" << std::endl;

    // Pipe operator with mixed types
    auto t3_result2 = t3 | fxt::tuple_select<1, 3, 4>();
    std::cout << "t3 | fxt::tuple_select<1, 3, 4>(): ("
              << std::get<0>(t3_result2) << ", '"
              << std::get<1>(t3_result2) << "', "
              << std::boolalpha << std::get<2>(t3_result2) << ")" << std::endl;

    // =========================================================================
    // Part 19: Using fxt::tuple_select by type (direct call)
    // Works with both fxt::tuple and fxt::flat_tuple; output kind matches input.
    // =========================================================================
    std::cout << "\n\nPart 19: Using fxt::tuple_select by type (direct call)" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;

    auto t4 = fxt::make_tuple(42, 3.14, std::string("type"), 'A');
    std::cout << "fxt::tuple (42, 3.14, \"type\", 'A'):" << std::endl;

    auto t4_result1 = fxt::tuple_select<int, char>(t4);
    std::cout << "  tuple_select<int, char>:         ("
              << std::get<0>(t4_result1) << ", '"
              << std::get<1>(t4_result1) << "')" << std::endl;

    auto t4_result2 = fxt::tuple_select<double, std::string>(t4);
    std::cout << "  tuple_select<double, std::string>: ("
              << std::get<0>(t4_result2) << ", \""
              << std::get<1>(t4_result2) << "\")" << std::endl;

    // Same syntax works on flat_tuple — result is a flat_tuple
    auto ft_t4 = fxt::make_flat_tuple(42, 3.14, std::string("type"), 'A');
    std::cout << "fxt::flat_tuple (42, 3.14, \"type\", 'A'):" << std::endl;

    auto ft_t4_result1 = fxt::tuple_select<int, char>(ft_t4);
    std::cout << "  tuple_select<int, char>:         ("
              << fxt::get<0>(ft_t4_result1) << ", '"
              << fxt::get<1>(ft_t4_result1) << "')  [flat_tuple]" << std::endl;

    auto ft_t4_result2 = fxt::tuple_select<std::string, double>(ft_t4);
    std::cout << "  tuple_select<std::string, double>: (\""
              << fxt::get<0>(ft_t4_result2) << "\", "
              << fxt::get<1>(ft_t4_result2) << ")  [flat_tuple, reordered]" << std::endl;

    // =========================================================================
    // Part 20: Using fxt::tuple_select by type (pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 20: Using fxt::tuple_select by type (pipe operator)" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    auto t5 = fxt::make_tuple(123, 9.99, std::string("pipe"), true);
    std::cout << "fxt::tuple (123, 9.99, \"pipe\", true):" << std::endl;

    auto t5_result1 = t5 | fxt::tuple_select<int, std::string>();
    std::cout << "  | tuple_select<int, std::string>:       ("
              << std::get<0>(t5_result1) << ", \""
              << std::get<1>(t5_result1) << "\")" << std::endl;

    auto t5_result2 = t5 | fxt::tuple_select<bool, double, int>();
    std::cout << "  | tuple_select<bool, double, int>:      ("
              << std::boolalpha << std::get<0>(t5_result2) << ", "
              << std::get<1>(t5_result2) << ", "
              << std::get<2>(t5_result2) << ")  [reordered]" << std::endl;

    // Same syntax works on flat_tuple — result is a flat_tuple
    auto ft_t5 = fxt::make_flat_tuple(123, 9.99, std::string("pipe"), true);
    std::cout << "fxt::flat_tuple (123, 9.99, \"pipe\", true):" << std::endl;

    auto ft_t5_result1 = ft_t5 | fxt::tuple_select<int, std::string>();
    std::cout << "  | tuple_select<int, std::string>:       ("
              << fxt::get<0>(ft_t5_result1) << ", \""
              << fxt::get<1>(ft_t5_result1) << "\")  [flat_tuple]" << std::endl;

    auto ft_t5_result2 = ft_t5 | fxt::tuple_select<std::string, bool, int>();
    std::cout << "  | tuple_select<std::string, bool, int>: (\""
              << fxt::get<0>(ft_t5_result2) << "\", "
              << std::boolalpha << fxt::get<1>(ft_t5_result2) << ", "
              << fxt::get<2>(ft_t5_result2) << ")  [flat_tuple, reordered]" << std::endl;

    // =========================================================================
    // Part 21: Using fxt::select on fxt::flat_tuple (direct call)
    // =========================================================================
    std::cout << "\n\nPart 21: Using fxt::select on fxt::flat_tuple (direct call)" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;

    auto ft3 = fxt::make_flat_tuple(5, 10, 15, 20, 25);
    std::cout << "Original flat_tuple: (5, 10, 15, 20, 25)" << std::endl;

    // Direct call syntax
    auto ft3_result = fxt::tuple_select<0, 2, 4>(ft3);
    std::cout << "fxt::tuple_select<0, 2, 4>(ft3): ("
              << fxt::get<0>(ft3_result) << ", "
              << fxt::get<1>(ft3_result) << ", "
              << fxt::get<2>(ft3_result) << ")" << std::endl;

    // Select consecutive elements
    auto ft3_result2 = fxt::tuple_select<1, 2, 3>(ft3);
    std::cout << "fxt::tuple_select<1, 2, 3>(ft3): ("
              << fxt::get<0>(ft3_result2) << ", "
              << fxt::get<1>(ft3_result2) << ", "
              << fxt::get<2>(ft3_result2) << ")" << std::endl;

    // =========================================================================
    // Part 22: Using fxt::select on fxt::flat_tuple (pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 22: Using fxt::select on fxt::flat_tuple (pipe operator)" << std::endl;
    std::cout << "-------------------------------------------------------------" << std::endl;

    auto ft4 = fxt::make_flat_tuple(11, 22, 33, 44, 55, 66);
    std::cout << "Original flat_tuple: (11, 22, 33, 44, 55, 66)" << std::endl;

    // Using pipe operator
    auto ft4_result = ft4 | fxt::tuple_select<0, 3, 5>();
    std::cout << "ft4 | fxt::tuple_select<0, 3, 5>(): ("
              << fxt::get<0>(ft4_result) << ", "
              << fxt::get<1>(ft4_result) << ", "
              << fxt::get<2>(ft4_result) << ")" << std::endl;

    // Select with duplicates
    auto ft4_dup = ft4 | fxt::tuple_select<0, 0, 1>();
    std::cout << "ft4 | fxt::tuple_select<0, 0, 1>(): ("
              << fxt::get<0>(ft4_dup) << ", "
              << fxt::get<1>(ft4_dup) << ", "
              << fxt::get<2>(ft4_dup) << ")" << std::endl;

    // =========================================================================
    // Part 23: Using fxt::select on fxt::flat_tuple with mixed types
    // =========================================================================
    std::cout << "\n\nPart 23: Using fxt::select on fxt::flat_tuple with mixed types" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;

    auto ft5 = fxt::make_flat_tuple(777, 8.88, std::string("mixed"), 'M', true);
    std::cout << "Original flat_tuple: (777, 8.88, \"mixed\", 'M', true)" << std::endl;

    // Direct call
    auto ft5_result1 = fxt::tuple_select<0, 2, 4>(ft5);
    std::cout << "fxt::tuple_select<0, 2, 4>(ft5): ("
              << fxt::get<0>(ft5_result1) << ", \""
              << fxt::get<1>(ft5_result1) << "\", "
              << std::boolalpha << fxt::get<2>(ft5_result1) << ")" << std::endl;

    // Pipe operator
    auto ft5_result2 = ft5 | fxt::tuple_select<1, 3>();
    std::cout << "ft5 | fxt::tuple_select<1, 3>(): ("
              << fxt::get<0>(ft5_result2) << ", '"
              << fxt::get<1>(ft5_result2) << "')" << std::endl;

    // =========================================================================
    // Part 24: Chaining select with other operations on fxt::tuple
    // =========================================================================
    std::cout << "\n\nPart 24: Chaining select with other operations on fxt::tuple" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    auto t6 = fxt::make_tuple(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    std::cout << "Original fxt::tuple: (1, 2, 3, 4, 5, 6, 7, 8, 9, 10)" << std::endl;

    // Chain drop and select
    auto t6_chain1 = t6 | fxt::tuple_drop<2>() | fxt::tuple_select<0, 2, 4>();
    std::cout << "After drop<2>() | select<0, 2, 4>(): ("
              << std::get<0>(t6_chain1) << ", "
              << std::get<1>(t6_chain1) << ", "
              << std::get<2>(t6_chain1) << ")" << std::endl;

    // Chain select and drop_last
    auto t6_chain2 = t6 | fxt::tuple_select<1, 3, 5, 7, 9>() | fxt::tuple_drop_last<2>();
    std::cout << "After select<1, 3, 5, 7, 9>() | drop_last<2>(): ("
              << std::get<0>(t6_chain2) << ", "
              << std::get<1>(t6_chain2) << ", "
              << std::get<2>(t6_chain2) << ")" << std::endl;

    // =========================================================================
    // Part 25: Chaining select with other operations on fxt::flat_tuple
    // =========================================================================
    std::cout << "\n\nPart 25: Chaining select with other operations on fxt::flat_tuple" << std::endl;
    std::cout << "-----------------------------------------------------------------" << std::endl;

    auto ft6 = fxt::make_flat_tuple(10, 20, 30, 40, 50, 60, 70, 80, 90, 100);
    std::cout << "Original flat_tuple: (10, 20, 30, 40, 50, 60, 70, 80, 90, 100)" << std::endl;

    // Chain drop and select
    auto ft6_chain1 = ft6 | fxt::tuple_drop<3>() | fxt::tuple_select<0, 2, 4>();
    std::cout << "After drop<3>() | select<0, 2, 4>(): ("
              << fxt::get<0>(ft6_chain1) << ", "
              << fxt::get<1>(ft6_chain1) << ", "
              << fxt::get<2>(ft6_chain1) << ")" << std::endl;

    // Chain select and drop
    auto ft6_chain2 = ft6 | fxt::tuple_select<0, 2, 4, 6, 8>() | fxt::tuple_drop<1>();
    std::cout << "After select<0, 2, 4, 6, 8>() | drop<1>(): ("
              << fxt::get<0>(ft6_chain2) << ", "
              << fxt::get<1>(ft6_chain2) << ", "
              << fxt::get<2>(ft6_chain2) << ", "
              << fxt::get<3>(ft6_chain2) << ")" << std::endl;

    // =========================================================================
    // Part 26: Using select on rvalue tuples
    // =========================================================================
    std::cout << "\n\nPart 26: Using select on rvalue tuples" << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    // fxt::tuple rvalue
    auto t7_rvalue = fxt::make_tuple(11, 22, 33, 44, 55) | fxt::tuple_select<0, 2, 4>();
    std::cout << "make_tuple(11,22,33,44,55) | select<0,2,4>(): ("
              << std::get<0>(t7_rvalue) << ", "
              << std::get<1>(t7_rvalue) << ", "
              << std::get<2>(t7_rvalue) << ")" << std::endl;

    // fxt::flat_tuple rvalue
    auto ft7_rvalue = fxt::make_flat_tuple(111, 222, 333, 444) | fxt::tuple_select<1, 3>();
    std::cout << "make_flat_tuple(111,222,333,444) | select<1,3>(): ("
              << fxt::get<0>(ft7_rvalue) << ", "
              << fxt::get<1>(ft7_rvalue) << ")" << std::endl;

    // =========================================================================
    // Part 27: Practical example - selecting data fields from tuples
    // =========================================================================
    std::cout << "\n\nPart 27: Practical example - selecting data fields" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    // Simulate a data record as fxt::tuple
    auto person = fxt::make_tuple(
        std::string("John Doe"),    // 0: name
        35,                          // 1: age
        std::string("Engineer"),     // 2: job
        75000,                       // 3: salary
        std::string("john@example.com") // 4: email
    );

    std::cout << "Person record: (\"John Doe\", 35, \"Engineer\", 75000, \"john@example.com\")" << std::endl;

    // Select basic info (name, age)
    auto basic_info = person | fxt::tuple_select<0, 1>();
    std::cout << "Basic info (name, age): (\""
              << std::get<0>(basic_info) << "\", "
              << std::get<1>(basic_info) << ")" << std::endl;

    // Select professional info (job, salary)
    auto prof_info = person | fxt::tuple_select<2, 3>();
    std::cout << "Professional info (job, salary): (\""
              << std::get<0>(prof_info) << "\", "
              << std::get<1>(prof_info) << ")" << std::endl;

    // Select contact info (name, email)
    auto contact_info = person | fxt::tuple_select<0, 4>();
    std::cout << "Contact info (name, email): (\""
              << std::get<0>(contact_info) << "\", \""
              << std::get<1>(contact_info) << "\")" << std::endl;

    // =========================================================================
    // Part 28: tuple_select vs mtuple_select — separation of plain vs monadic
    // tuple_select  operates on plain tuples directly.
    // mtuple_select operates on expected/optional containing a tuple.
    // =========================================================================
    std::cout << "\n\nPart 28: tuple_select (plain) vs mtuple_select (monadic)" << std::endl;
    std::cout << "---------------------------------------------------------" << std::endl;

    // Plain tuple — use tuple_select
    auto t8 = fxt::make_tuple(100, 200, 300, 400, 500);
    auto t8_plain = t8 | fxt::tuple_select<0, 2, 4>();
    std::cout << "Plain fxt::tuple | tuple_select<0,2,4>: ("
              << std::get<0>(t8_plain) << ", "
              << std::get<1>(t8_plain) << ", "
              << std::get<2>(t8_plain) << ")" << std::endl;

    // Wrapped in expected — use mtuple_select
    auto exp8 = fxt::expected<fxt::tuple<int,int,int,int,int>, std::string>{
        fxt::make_tuple(100, 200, 300, 400, 500)};
    auto exp8_result = exp8 | fxt::mtuple_select<0, 2, 4>();
    std::cout << "expected<tuple> | mtuple_select<0,2,4>: ("
              << std::get<0>(*exp8_result) << ", "
              << std::get<1>(*exp8_result) << ", "
              << std::get<2>(*exp8_result) << ")" << std::endl;

    // =========================================================================
    // Part 29: Type-based flat_tuple selection inside a monad
    // =========================================================================
    std::cout << "\n\nPart 29: mtuple_select by type on flat_tuple inside a monad" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    auto exp9 = fxt::expected<fxt::flat_tuple<int, double, std::string>, std::string>{
        fxt::make_flat_tuple(7, 3.14, std::string("inside"))};
    std::cout << "expected<flat_tuple<int,double,string>>{7, 3.14, \"inside\"}:" << std::endl;

    auto exp9_result = exp9 | fxt::mtuple_select<std::string, int>();
    if (exp9_result) {
        std::cout << "  | mtuple_select<std::string, int>: (\""
                  << fxt::get<0>(*exp9_result) << "\", "
                  << fxt::get<1>(*exp9_result) << ")  [flat_tuple, reordered]" << std::endl;
    }

    auto opt9 = fxt::optional<fxt::flat_tuple<int, double, std::string>>{
        fxt::make_flat_tuple(42, 2.71, std::string("opt"))};
    auto opt9_result = opt9 | fxt::mtuple_select<double, std::string>();
    if (opt9_result) {
        std::cout << "optional<flat_tuple> | mtuple_select<double,string>: ("
                  << fxt::get<0>(*opt9_result) << ", \""
                  << fxt::get<1>(*opt9_result) << "\")" << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}



