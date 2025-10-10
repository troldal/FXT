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
    auto result1 = exp1 | fxt::mselect<0, 2>();
    if (result1) {
        auto [i, s] = *result1;
        std::cout << "Selected indices 0, 2: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select elements at indices 1, 3, and 4
    auto result2 = exp1 | fxt::mselect<1, 3, 4>();
    if (result2) {
        auto [d, b, c] = *result2;
        std::cout << "Selected indices 1, 3, 4: (" << d << ", " << std::boolalpha << b << ", '" << c << "')" << std::endl;
    }

    // Select all elements in different order
    std::cout << "\nReordering elements:" << std::endl;
    auto reordered = exp1 | fxt::mselect<4, 2, 0>();
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
    auto type_result1 = exp2 | fxt::mselect<int, std::string>();
    if (type_result1) {
        auto [i, s] = *type_result1;
        std::cout << "Selected int, string: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select different types
    auto type_result2 = exp2 | fxt::mselect<double, bool>();
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

    auto error_result = exp_error | fxt::mselect<0, 2>();

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
    auto opt_result1 = opt1 | fxt::mselect<0, 2>();
    if (opt_result1) {
        auto [i, s] = *opt_result1;
        std::cout << "Selected indices 0, 2: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select elements at indices 1, 3, and 4
    auto opt_result2 = opt1 | fxt::mselect<1, 3, 4>();
    if (opt_result2) {
        auto [d, b, c] = *opt_result2;
        std::cout << "Selected indices 1, 3, 4: (" << d << ", " << std::boolalpha << b << ", '" << c << "')" << std::endl;
    }

    // Select in reverse order
    std::cout << "\nReversing elements:" << std::endl;
    auto reversed = opt1 | fxt::mselect<4, 3, 2, 1, 0>();
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
    auto opt_type_result1 = opt2 | fxt::mselect<int, std::string>();
    if (opt_type_result1) {
        auto [i, s] = *opt_type_result1;
        std::cout << "Selected int, string: (" << i << ", \"" << s << "\")" << std::endl;
    }

    // Select all types
    auto opt_type_result2 = opt2 | fxt::mselect<double, bool, int>();
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

    auto empty_result = opt_empty | fxt::mselect<0, 2>();

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
        | fxt::mappend(100)
        | fxt::mappend(200)
        | fxt::mappend(300)
        | fxt::mappend(400)
        | fxt::mappend(500);

    std::cout << "Built tuple: (100, 200, 300, 400, 500)" << std::endl;

    // Select subset
    auto subset = built | fxt::mselect<0, 2, 4>();
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
        | fxt::mappend(5)
        | fxt::mappend(10)
        | fxt::mappend(15)
        | fxt::mapply([](int a, int b, int c) { return a + b + c; })
        | fxt::mapply([](int a, int b, int c, int sum) { return a * b * c; });

    std::cout << "Computed tuple: (5, 10, 15, 30, 750)" << std::endl;

    // Select specific computed values
    auto selected_computed = computed | fxt::mselect<0, 3, 4>();
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
    auto first_select = original | fxt::mselect<1, 2, 3>();
    if (first_select) {
        auto [a, b, c] = *first_select;
        std::cout << "After first select<1, 2, 3>: (" << a << ", " << b << ", " << c << ")" << std::endl;
    }

    // Second selection from the result
    auto second_select = first_select | fxt::mselect<0, 2>();
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
    auto direct_result = fxt::mselect<0, 2>()(exp3);
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
    auto single = tuple5 | fxt::mselect<1>();
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
    auto network_settings = config | fxt::mselect<0, 1>();
    if (network_settings) {
        auto [host, port] = *network_settings;
        std::cout << "\nNetwork settings: " << host << ":" << port << std::endl;
    }

    // Extract API settings (endpoint, ssl_enabled)
    auto api_settings = config | fxt::mselect<2, 3>();
    if (api_settings) {
        auto [endpoint, ssl] = *api_settings;
        std::cout << "API settings: " << endpoint << " (SSL: " << std::boolalpha << ssl << ")" << std::endl;
    }

    // Extract timeout setting
    auto timeout_setting = config | fxt::mselect<4>();
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
    auto duplicates = tuple6 | fxt::mselect<0, 0, 1, 0>();
    if (duplicates) {
        auto [a, b, c, d] = *duplicates;
        std::cout << "Selected indices 0, 0, 1, 0: (" << a << ", " << b << ", " << c << ", " << d << ")" << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}

