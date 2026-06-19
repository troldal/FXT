//
// Demo: fxt::get
//
// This demo shows how to use fxt::get to extract elements from tuples
// within monadic containers (fxt::expected and fxt::optional) using the pipe operator.
//

#include <iostream>
#include <string>
#include <fxt.hpp>

int main()
{
    std::cout << "=== fxt::get Demo ===" << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Part 1: Using get with fxt::expected - by index
    // =========================================================================
    std::cout << "Part 1: Using get with fxt::expected - by index" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    // Create an expected containing a tuple
    auto exp1 = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        std::make_tuple(42, 3.14, "hello")
    };

    // Extract elements by index
    auto result0 = exp1 | fxt::mget<0>();
    auto result1 = exp1 | fxt::mget<1>();
    auto result2 = exp1 | fxt::mget<2>();

    if (result0 && result1 && result2) {
        std::cout << "Element at index 0: " << *result0 << std::endl;
        std::cout << "Element at index 1: " << *result1 << std::endl;
        std::cout << "Element at index 2: " << *result2 << std::endl;
    }

    // Build a tuple incrementally and extract elements
    std::cout << "\nBuilding a tuple with append and extracting elements:" << std::endl;
    auto exp2 = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mtuple_append(100)
        | fxt::mtuple_append(200)
        | fxt::mtuple_append(300);

    auto first = exp2 | fxt::mget<0>();
    auto second = exp2 | fxt::mget<1>();
    auto third = exp2 | fxt::mget<2>();

    if (first && second && third) {
        std::cout << "First: " << *first << ", Second: " << *second << ", Third: " << *third << std::endl;
    }

    // =========================================================================
    // Part 2: Using get with fxt::expected - by type
    // =========================================================================
    std::cout << "\n\nPart 2: Using get with fxt::expected - by type" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    auto exp3 = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        std::make_tuple(99, 2.71, "world")
    };

    // Extract elements by type
    auto int_val = exp3 | fxt::mget<int>();
    auto double_val = exp3 | fxt::mget<double>();
    auto string_val = exp3 | fxt::mget<std::string>();

    if (int_val && double_val && string_val) {
        std::cout << "int value: " << *int_val << std::endl;
        std::cout << "double value: " << *double_val << std::endl;
        std::cout << "string value: " << *string_val << std::endl;
    }

    // =========================================================================
    // Part 3: Error propagation with fxt::expected
    // =========================================================================
    std::cout << "\n\nPart 3: Error propagation with fxt::expected" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    auto exp_error = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        fxt::unexpected("Something went wrong")
    };

    auto result_error = exp_error | fxt::mget<0>();

    if (result_error) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error propagated: " << result_error.error() << std::endl;
    }

    // =========================================================================
    // Part 4: Using get with fxt::optional - by index
    // =========================================================================
    std::cout << "\n\nPart 4: Using get with fxt::optional - by index" << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;

    // Create an optional containing a tuple
    auto opt1 = fxt::optional<std::tuple<int, double, std::string>>{
        std::make_tuple(77, 1.41, "optional")
    };

    // Extract elements by index
    auto opt_result0 = opt1 | fxt::mget<0>();
    auto opt_result1 = opt1 | fxt::mget<1>();
    auto opt_result2 = opt1 | fxt::mget<2>();

    if (opt_result0 && opt_result1 && opt_result2) {
        std::cout << "Element at index 0: " << *opt_result0 << std::endl;
        std::cout << "Element at index 1: " << *opt_result1 << std::endl;
        std::cout << "Element at index 2: " << *opt_result2 << std::endl;
    }

    // Build a tuple incrementally with optional and extract elements
    std::cout << "\nBuilding a tuple with append and extracting elements:" << std::endl;
    auto opt2 = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mtuple_append(10)
        | fxt::mtuple_append(20)
        | fxt::mtuple_append(30);

    auto opt_first = opt2 | fxt::mget<0>();
    auto opt_second = opt2 | fxt::mget<1>();
    auto opt_third = opt2 | fxt::mget<2>();

    if (opt_first && opt_second && opt_third) {
        std::cout << "First: " << *opt_first << ", Second: " << *opt_second << ", Third: " << *opt_third << std::endl;
    }

    // =========================================================================
    // Part 5: Using get with fxt::optional - by type
    // =========================================================================
    std::cout << "\n\nPart 5: Using get with fxt::optional - by type" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    auto opt3 = fxt::optional<std::tuple<int, double, std::string>>{
        std::make_tuple(55, 6.28, "type")
    };

    // Extract elements by type
    auto opt_int_val = opt3 | fxt::mget<int>();
    auto opt_double_val = opt3 | fxt::mget<double>();
    auto opt_string_val = opt3 | fxt::mget<std::string>();

    if (opt_int_val && opt_double_val && opt_string_val) {
        std::cout << "int value: " << *opt_int_val << std::endl;
        std::cout << "double value: " << *opt_double_val << std::endl;
        std::cout << "string value: " << *opt_string_val << std::endl;
    }

    // =========================================================================
    // Part 6: Empty propagation with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 6: Empty propagation with fxt::optional" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    auto opt_empty = fxt::optional<std::tuple<int, double, std::string>>{};

    auto result_empty = opt_empty | fxt::mget<0>();

    if (result_empty) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Empty optional propagated correctly" << std::endl;
    }

    // =========================================================================
    // Part 7: Combining get with apply
    // =========================================================================
    std::cout << "\n\nPart 7: Combining get with apply" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    // Build a tuple with computations, then extract specific elements
    auto computed = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mtuple_append(5)
        | fxt::mtuple_append(10)
        | fxt::mapply([](int a, int b) { return a + b; })
        | fxt::mapply([](int a, int b, int sum) { return a * b; });

    std::cout << "Computed tuple contains: (5, 10, 15, 50)" << std::endl;

    auto sum_result = computed | fxt::mget<2>();
    auto product_result = computed | fxt::mget<3>();

    if (sum_result && product_result) {
        std::cout << "Sum (index 2): " << *sum_result << std::endl;
        std::cout << "Product (index 3): " << *product_result << std::endl;
    }

    // =========================================================================
    // Part 8: Chaining get operations
    // =========================================================================
    std::cout << "\n\nPart 8: Chaining get operations" << std::endl;
    std::cout << "-------------------------------" << std::endl;

    auto exp4 = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        std::make_tuple(123, 4.56, "chain")
    };

    // Extract and transform in a chain
    auto chained_result = exp4
        | fxt::mget<0>()
        | fxt::transform([](int x) { return x * 2; });

    if (chained_result) {
        std::cout << "Original value at index 0: 123" << std::endl;
        std::cout << "After get<0>() | transform(*2): " << *chained_result << std::endl;
    }

    // =========================================================================
    // Part 9: Mixed type extraction
    // =========================================================================
    std::cout << "\n\nPart 9: Mixed type extraction" << std::endl;
    std::cout << "-----------------------------" << std::endl;

    auto mixed = fxt::optional<std::tuple<std::string, int, bool, double>>{
        std::make_tuple("test", 42, true, 9.99)
    };

    std::cout << "Tuple contains: (\"test\", 42, true, 9.99)" << std::endl;

    // Extract by index
    std::cout << "\nBy index:" << std::endl;
    if (auto r0 = mixed | fxt::mget<0>()) std::cout << "  Index 0: " << *r0 << std::endl;
    if (auto r1 = mixed | fxt::mget<1>()) std::cout << "  Index 1: " << *r1 << std::endl;
    if (auto r2 = mixed | fxt::mget<2>()) std::cout << "  Index 2: " << std::boolalpha << *r2 << std::endl;
    if (auto r3 = mixed | fxt::mget<3>()) std::cout << "  Index 3: " << *r3 << std::endl;

    // Extract by type
    std::cout << "\nBy type:" << std::endl;
    if (auto s = mixed | fxt::mget<std::string>()) std::cout << "  string: " << *s << std::endl;
    if (auto i = mixed | fxt::mget<int>()) std::cout << "  int: " << *i << std::endl;
    if (auto b = mixed | fxt::mget<bool>()) std::cout << "  bool: " << std::boolalpha << *b << std::endl;
    if (auto d = mixed | fxt::mget<double>()) std::cout << "  double: " << *d << std::endl;

    // =========================================================================
    // Part 10: Practical example - parsing and extracting data
    // =========================================================================
    std::cout << "\n\nPart 10: Practical example - parsing and extracting data" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    // Simulate parsing some configuration data
    auto parse_config = [](const std::string& input) -> fxt::expected<std::tuple<int, std::string, bool>, std::string> {
        if (input == "valid") {
            return std::make_tuple(8080, "localhost", true);
        }
        return fxt::unexpected("Invalid configuration");
    };

    auto config = parse_config("valid");

    std::cout << "Configuration loaded:" << std::endl;
    if (auto port = config | fxt::mget<0>()) {
        std::cout << "  Port: " << *port << std::endl;
    }
    if (auto host = config | fxt::mget<1>()) {
        std::cout << "  Host: " << *host << std::endl;
    }
    if (auto enabled = config | fxt::mget<2>()) {
        std::cout << "  Enabled: " << std::boolalpha << *enabled << std::endl;
    }

    // Try with invalid config
    std::cout << "\nTrying with invalid configuration:" << std::endl;
    auto bad_config = parse_config("invalid");
    auto port_result = bad_config | fxt::mget<0>();
    if (!port_result) {
        std::cout << "  Error: " << bad_config.error() << std::endl;
    }

    // =========================================================================
    // Part 11: Direct call syntax (without pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 11: Direct call syntax (without pipe operator)" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    auto exp5 = fxt::expected<std::tuple<int, double, std::string>, std::string>{
        std::make_tuple(999, 7.77, "direct")
    };

    // Using direct call syntax
    auto direct_result0 = fxt::mget<0>()(exp5);
    auto direct_result1 = fxt::mget<double>()(exp5);

    if (direct_result0 && direct_result1) {
        std::cout << "Direct call - index 0: " << *direct_result0 << std::endl;
        std::cout << "Direct call - type double: " << *direct_result1 << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}


