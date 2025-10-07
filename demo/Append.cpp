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
    auto exp2 = exp1 | fxt::append(std::string{"hello"});

    // Append a double value
    auto exp3 = exp2 | fxt::append(3.14);

    if (exp3) {
        auto [i, s, d] = *exp3;
        std::cout << "Success: (" << i << ", \"" << s << "\", " << d << ")" << std::endl;
    } else {
        std::cout << "Error: " << exp3.error() << std::endl;
    }

    // Chain multiple appends in one expression
    std::cout << "\nChaining multiple appends:" << std::endl;
    auto chained = fxt::expected<std::tuple<int>, std::string>{std::tuple{100}}
        | fxt::append(std::string{"world"})
        | fxt::append(2.71)
        | fxt::append(true);

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
        | fxt::append(std::string{"this won't be added"})
        | fxt::append(999);

    if (result_with_error) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error propagated: " << result_with_error.error() << std::endl;
    }

    // Append from another expected
    std::cout << "\nAppending from another expected:" << std::endl;
    auto exp_value = fxt::expected<std::string, std::string>{std::string{"from expected"}};
    auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{777}};

    auto combined = exp_tuple | fxt::append(exp_value);

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
    auto opt2 = opt1 | fxt::append(std::string{"hello"});
    auto opt3 = opt2 | fxt::append(3.14);

    if (opt3) {
        auto [i, s, d] = *opt3;
        std::cout << "Success: (" << i << ", \"" << s << "\", " << d << ")" << std::endl;
    } else {
        std::cout << "No value" << std::endl;
    }

    // Chain multiple appends with optional
    std::cout << "\nChaining multiple appends:" << std::endl;
    auto chained_opt = fxt::optional<std::tuple<int>>{std::tuple{200}}
        | fxt::append(std::string{"optional"})
        | fxt::append(1.41)
        | fxt::append(false);

    if (chained_opt) {
        auto [i, s, d, b] = *chained_opt;
        std::cout << "Result: (" << i << ", \"" << s << "\", " << d << ", "
                  << std::boolalpha << b << ")" << std::endl;
    }

    // Demonstrate empty optional propagation
    std::cout << "\nEmpty optional propagation:" << std::endl;
    auto empty_opt = fxt::optional<std::tuple<int>>{};

    auto result_with_empty = empty_opt
        | fxt::append(std::string{"won't be added"})
        | fxt::append(999);

    if (result_with_empty) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Empty optional propagated correctly" << std::endl;
    }

    // Append from another optional
    std::cout << "\nAppending from another optional:" << std::endl;
    auto opt_value = fxt::optional<std::string>{std::string{"from optional"}};
    auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{888}};

    auto combined_opt = opt_tuple | fxt::append(opt_value);

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

    auto result_empty = tuple_opt | fxt::append(empty_value);

    if (result_empty) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Result is empty (as expected)" << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}

