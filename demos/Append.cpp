//
// Demo: fxt::append
//
// This demo shows how to use fxt::append to build up tuples within
// monadic containers (fxt::expected and fxt::optional) using the pipe operator.
//

#include <iostream>
#include <string>
#include <fxt.hpp>

int main()
{
    std::cout << "=== fxt::append Demo ===" << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Part 1: Using append with fxt::expected
    // =========================================================================
    std::cout << "Part 1: Using append with fxt::expected" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // Start with an expected containing a tuple with one element
    auto exp1 = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};

    // Append a string value to the tuple
    auto exp2 = exp1 | fxt::mappend(std::string{"hello"});

    // Append a double value
    auto exp3 = exp2 | fxt::mappend(3.14);

    if (exp3) {
        auto [i, s, d] = *exp3;
        std::cout << "Success: (" << i << ", \"" << s << "\", " << d << ")" << std::endl;
    } else {
        std::cout << "Error: " << exp3.error() << std::endl;
    }

    // Chain multiple appends in one expression
    std::cout << "\nChaining multiple appends:" << std::endl;
    auto chained = fxt::expected<std::tuple<int>, std::string>{std::tuple{100}}
        | fxt::mappend(std::string{"world"})
        | fxt::mappend(2.71)
        | fxt::mappend(true);

    if (chained) {
        auto [i, s, d, b] = *chained;
        std::cout << "Result: (" << i << ", \"" << s << "\", " << d << ", "
                  << std::boolalpha << b << ")" << std::endl;
    }

    // Demonstrate error propagation
    std::cout << "\nError propagation:" << std::endl;
    auto error_exp = fxt::expected<std::tuple<int>, std::string>{
        fxt::unexpected<std::string>("Something went wrong")
    };

    auto result_with_error = error_exp
        | fxt::mappend(std::string{"this won't be added"})
        | fxt::mappend(999);

    if (result_with_error) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error propagated: " << result_with_error.error() << std::endl;
    }

    // Append from another expected
    std::cout << "\nAppending from another expected:" << std::endl;
    auto exp_value = fxt::expected<std::string, std::string>{std::string{"from expected"}};
    auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{777}};

    auto combined = exp_tuple | fxt::mappend(exp_value);

    if (combined) {
        auto [i, s] = *combined;
        std::cout << "Combined: (" << i << ", \"" << s << "\")" << std::endl;
    } else {
        std::cout << "Error: " << combined.error() << std::endl;
    }

    // =========================================================================
    // Part 2: Using append with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 2: Using append with fxt::optional" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // Start with an optional containing a tuple
    auto opt1 = fxt::optional<std::tuple<int>>{std::tuple{42}};

    // Append values to build up the tuple
    auto opt2 = opt1 | fxt::mappend(std::string{"hello"});
    auto opt3 = opt2 | fxt::mappend(3.14);

    if (opt3) {
        auto [i, s, d] = *opt3;
        std::cout << "Success: (" << i << ", \"" << s << "\", " << d << ")" << std::endl;
    } else {
        std::cout << "No value" << std::endl;
    }

    // Chain multiple appends with optional
    std::cout << "\nChaining multiple appends:" << std::endl;
    auto chained_opt = fxt::optional<std::tuple<int>>{std::tuple{200}}
        | fxt::mappend(std::string{"optional"})
        | fxt::mappend(1.41)
        | fxt::mappend(false);

    if (chained_opt) {
        auto [i, s, d, b] = *chained_opt;
        std::cout << "Result: (" << i << ", \"" << s << "\", " << d << ", "
                  << std::boolalpha << b << ")" << std::endl;
    }

    // Demonstrate empty optional propagation
    std::cout << "\nEmpty optional propagation:" << std::endl;
    auto empty_opt = fxt::optional<std::tuple<int>>{};

    auto result_with_empty = empty_opt
        | fxt::mappend(std::string{"won't be added"})
        | fxt::mappend(999);

    if (result_with_empty) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Empty optional propagated correctly" << std::endl;
    }

    // Append from another optional
    std::cout << "\nAppending from another optional:" << std::endl;
    auto opt_value = fxt::optional<std::string>{std::string{"from optional"}};
    auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{888}};

    auto combined_opt = opt_tuple | fxt::mappend(opt_value);

    if (combined_opt) {
        auto [i, s] = *combined_opt;
        std::cout << "Combined: (" << i << ", \"" << s << "\")" << std::endl;
    } else {
        std::cout << "No value" << std::endl;
    }

    // Append empty optional to tuple
    std::cout << "\nAppending empty optional:" << std::endl;
    auto empty_value = fxt::optional<std::string>{};
    auto tuple_opt = fxt::optional<std::tuple<int>>{std::tuple{999}};

    auto result_empty = tuple_opt | fxt::mappend(empty_value);

    if (result_empty) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Result is empty (as expected)" << std::endl;
    }

    // =========================================================================
    // Part 3: Using append with fxt::flat_tuple
    // =========================================================================
    std::cout << "\n\nPart 3: Using append with fxt::flat_tuple" << std::endl;
    std::cout << "----------------------------------------" << std::endl;

    // Start with an expected containing a flat_tuple with one element
    auto flat_exp1 = fxt::expected<fxt::flat_tuple<int>, std::string>{fxt::flat_tuple<int>{42}};

    // Append a string value to the flat_tuple
    auto flat_exp2 = flat_exp1 | fxt::mappend(std::string{"hello"});

    // Append a double value
    auto flat_exp3 = flat_exp2 | fxt::mappend(3.14);

    if (flat_exp3) {
        std::cout << "Success: (" << fxt::get<0>(*flat_exp3) << ", \""
                  << fxt::get<1>(*flat_exp3) << "\", " << fxt::get<2>(*flat_exp3) << ")" << std::endl;
    } else {
        std::cout << "Error: " << flat_exp3.error() << std::endl;
    }

    // Chain multiple appends with flat_tuple in one expression
    std::cout << "\nChaining multiple appends with flat_tuple:" << std::endl;
    auto chained_flat = fxt::expected<fxt::flat_tuple<int>, std::string>{fxt::flat_tuple<int>{100}}
        | fxt::mappend(std::string{"flat"})
        | fxt::mappend(2.71)
        | fxt::mappend(true);

    if (chained_flat) {
        std::cout << "Result: (" << fxt::get<0>(*chained_flat) << ", \""
                  << fxt::get<1>(*chained_flat) << "\", " << fxt::get<2>(*chained_flat)
                  << ", " << std::boolalpha << fxt::get<3>(*chained_flat) << ")" << std::endl;
    }

    // Using flat_tuple with optional
    std::cout << "\nUsing flat_tuple with optional:" << std::endl;
    auto flat_opt1 = fxt::optional<fxt::flat_tuple<int>>{fxt::flat_tuple<int>{555}};

    // Append values to build up the flat_tuple
    auto flat_opt2 = flat_opt1 | fxt::mappend(std::string{"flat_optional"});
    auto flat_opt3 = flat_opt2 | fxt::mappend(1.618);

    if (flat_opt3) {
        std::cout << "Success: (" << fxt::get<0>(*flat_opt3) << ", \""
                  << fxt::get<1>(*flat_opt3) << "\", " << fxt::get<2>(*flat_opt3) << ")" << std::endl;
    } else {
        std::cout << "No value" << std::endl;
    }

    // Demonstrate error propagation with flat_tuple
    std::cout << "\nError propagation with flat_tuple:" << std::endl;
    auto error_flat_exp = fxt::expected<fxt::flat_tuple<int>, std::string>{
        fxt::unexpected<std::string>("Flat tuple error")
    };

    auto result_flat_error = error_flat_exp
        | fxt::mappend(std::string{"won't be added"})
        | fxt::mappend(999);

    if (result_flat_error) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error propagated: " << result_flat_error.error() << std::endl;
    }

    // Append from another expected to flat_tuple
    std::cout << "\nAppending from expected to flat_tuple:" << std::endl;
    auto flat_exp_value = fxt::expected<std::string, std::string>{std::string{"from expected"}};
    auto flat_exp_tuple = fxt::expected<fxt::flat_tuple<int>, std::string>{fxt::flat_tuple<int>{777}};

    auto combined_flat = flat_exp_tuple | fxt::mappend(flat_exp_value);

    if (combined_flat) {
        std::cout << "Combined: (" << fxt::get<0>(*combined_flat) << ", \""
                  << fxt::get<1>(*combined_flat) << "\")" << std::endl;
    } else {
        std::cout << "Error: " << combined_flat.error() << std::endl;
    }

    // Mixing std::tuple and flat_tuple behavior comparison
    std::cout << "\nComparison: std::tuple vs flat_tuple:" << std::endl;
    auto std_result = fxt::expected<std::tuple<int>, std::string>{std::tuple{10}}
        | fxt::mappend(20) | fxt::mappend(30);

    auto flat_result = fxt::expected<fxt::flat_tuple<int>, std::string>{fxt::flat_tuple<int>{10}}
        | fxt::mappend(20) | fxt::mappend(30);

    if (std_result && flat_result) {
        auto [a, b, c] = *std_result;
        std::cout << "std::tuple result: (" << a << ", " << b << ", " << c << ")" << std::endl;
        std::cout << "flat_tuple result: (" << fxt::get<0>(*flat_result) << ", "
                  << fxt::get<1>(*flat_result) << ", " << fxt::get<2>(*flat_result) << ")" << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}

