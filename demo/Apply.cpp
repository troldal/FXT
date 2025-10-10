//
// Demo: fxt::apply
//
// This demo shows how to use fxt::apply to apply functions to tuple values
// within monadic containers (fxt::expected and fxt::optional) using the pipe operator.
//

#include <iostream>
#include <string>
#include <cmath>
#include <fxt.hpp>

int main()
{
    std::cout << "=== fxt::apply Demo ===" << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Part 1: Basic usage with fxt::expected
    // =========================================================================
    std::cout << "Part 1: Basic usage with fxt::expected" << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    // Apply a function to compute the sum (using temporary directly)
    auto exp2 = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mappend(5)
        | fxt::mappend(10)
        | fxt::mapply([](int a, int b) {
            std::cout << "Computing: " << a << " + " << b << " = " << (a + b) << std::endl;
            return a + b;
        });

    if (exp2) {
        auto [a, b, sum] = *exp2;
        std::cout << "Result tuple: (" << a << ", " << b << ", " << sum << ")" << std::endl;
    }

    // Chain multiple apply operations
    std::cout << "\nChaining multiple apply operations:" << std::endl;
    auto exp3 = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mappend(3)
        | fxt::mappend(4)
        | fxt::mapply([](int a, int b) {
            std::cout << "  Step 1: " << a << " + " << b << " = " << (a + b) << std::endl;
            return a + b;
        })
        | fxt::mapply([](int a, int b, int sum) {
            std::cout << "  Step 2: " << sum << " * 2 = " << (sum * 2) << std::endl;
            return sum * 2;
        });

    if (exp3) {
        auto [a, b, sum, doubled] = *exp3;
        std::cout << "Final tuple: (" << a << ", " << b << ", " << sum << ", " << doubled << ")" << std::endl;
    }

    // =========================================================================
    // Part 2: Error propagation with fxt::expected
    // =========================================================================
    std::cout << "\n\nPart 2: Error propagation with fxt::expected" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    // Start with an error
    auto result_with_error = fxt::expected<std::tuple<>, std::string>{fxt::unexpected("Initial error")}
        | fxt::mappend(1)
        | fxt::mappend(2)
        | fxt::mapply([](int a, int b) {
            std::cout << "This won't be called" << std::endl;
            return a + b;
        });

    if (result_with_error) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error propagated: " << result_with_error.error() << std::endl;
    }

    // =========================================================================
    // Part 3: Functions returning monadic types (expected)
    // =========================================================================
    std::cout << "\n\nPart 3: Functions returning monadic types (expected)" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    // Apply a function that can fail
    auto exp_monadic = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mappend(10.0)
        | fxt::mappend(2.0)
        | fxt::mapply([](double a, double b) -> fxt::expected<double, std::string> {
            std::cout << "Dividing: " << a << " / " << b << std::endl;
            if (b == 0.0) {
                return fxt::unexpected("Division by zero");
            }
            return a / b;
        });

    if (exp_monadic) {
        auto [a, b, result] = *exp_monadic;
        std::cout << "Success: " << a << " / " << b << " = " << result << std::endl;
    } else {
        std::cout << "Error: " << exp_monadic.error() << std::endl;
    }

    // Example with error in the applied function
    std::cout << "\nWith division by zero:" << std::endl;
    auto exp_error = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mappend(10.0)
        | fxt::mappend(0.0)
        | fxt::mapply([](double a, double b) -> fxt::expected<double, std::string> {
            std::cout << "Attempting: " << a << " / " << b << std::endl;
            if (b == 0.0) {
                return fxt::unexpected("Division by zero");
            }
            return a / b;
        });

    if (exp_error) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Error caught: " << exp_error.error() << std::endl;
    }

    // =========================================================================
    // Part 4: Void-returning functions for side effects (expected)
    // =========================================================================
    std::cout << "\n\nPart 4: Void-returning functions for side effects (expected)" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    auto exp_void = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mappend(42)
        | fxt::mappend(std::string{"hello"})
        | fxt::mapply([](int x, const std::string& s) {
            std::cout << "Side effect: logging " << x << " and '" << s << "'" << std::endl;
        });

    if (exp_void) {
        auto [x, s] = *exp_void;
        std::cout << "Tuple unchanged: (" << x << ", \"" << s << "\")" << std::endl;
    }

    // =========================================================================
    // Part 5: Basic usage with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 5: Basic usage with fxt::optional" << std::endl;
    std::cout << "---------------------------------------" << std::endl;

    // Apply a function to compute the product (using temporary directly)
    auto opt2 = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mappend(7)
        | fxt::mappend(8)
        | fxt::mapply([](int a, int b) {
            std::cout << "Computing: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        });

    if (opt2) {
        auto [a, b, product] = *opt2;
        std::cout << "Result tuple: (" << a << ", " << b << ", " << product << ")" << std::endl;
    }

    // Chain multiple apply operations
    std::cout << "\nChaining multiple apply operations:" << std::endl;
    auto opt3 = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mappend(2)
        | fxt::mappend(3)
        | fxt::mapply([](int a, int b) {
            std::cout << "  Step 1: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::mapply([](int a, int b, int product) {
            std::cout << "  Step 2: " << product << " + 10 = " << (product + 10) << std::endl;
            return product + 10;
        });

    if (opt3) {
        auto [a, b, product, sum] = *opt3;
        std::cout << "Final tuple: (" << a << ", " << b << ", " << product << ", " << sum << ")" << std::endl;
    }

    // =========================================================================
    // Part 6: Empty propagation with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 6: Empty propagation with fxt::optional" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    // Start with an empty optional
    auto result_with_empty = fxt::optional<std::tuple<>>{}
        | fxt::mappend(1)
        | fxt::mappend(2)
        | fxt::mapply([](int a, int b) {
            std::cout << "This won't be called" << std::endl;
            return a + b;
        });

    if (result_with_empty) {
        std::cout << "Unexpected value" << std::endl;
    } else {
        std::cout << "Empty optional propagated correctly" << std::endl;
    }

    // =========================================================================
    // Part 7: Functions returning monadic types (optional)
    // =========================================================================
    std::cout << "\n\nPart 7: Functions returning monadic types (optional)" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    // Apply a function that returns optional
    auto opt_monadic = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mappend(16.0)
        | fxt::mapply([](double x) -> fxt::optional<double> {
            std::cout << "Computing sqrt of " << x << std::endl;
            if (x < 0.0) {
                return fxt::nullopt;
            }
            return std::sqrt(x);
        });

    if (opt_monadic) {
        auto [x, result] = *opt_monadic;
        std::cout << "Success: sqrt(" << x << ") = " << result << std::endl;
    } else {
        std::cout << "No value" << std::endl;
    }

    // Example with failure
    std::cout << "\nWith negative number:" << std::endl;
    auto opt_fail = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mappend(-16.0)
        | fxt::mapply([](double x) -> fxt::optional<double> {
            std::cout << "Attempting sqrt of " << x << std::endl;
            if (x < 0.0) {
                return fxt::nullopt;
            }
            return std::sqrt(x);
        });

    if (opt_fail) {
        std::cout << "Unexpected success" << std::endl;
    } else {
        std::cout << "Empty optional (as expected)" << std::endl;
    }

    // =========================================================================
    // Part 8: Void-returning functions for side effects (optional)
    // =========================================================================
    std::cout << "\n\nPart 8: Void-returning functions for side effects (optional)" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    auto opt_void = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mappend(99)
        | fxt::mappend(std::string{"world"})
        | fxt::mapply([](int x, const std::string& s) {
            std::cout << "Side effect: processing " << x << " and '" << s << "'" << std::endl;
        });

    if (opt_void) {
        auto [x, s] = *opt_void;
        std::cout << "Tuple unchanged: (" << x << ", \"" << s << "\")" << std::endl;
    }

    // =========================================================================
    // Part 9: Complex pipeline example
    // =========================================================================
    std::cout << "\n\nPart 9: Complex pipeline example" << std::endl;
    std::cout << "--------------------------------" << std::endl;

    std::cout << "Building a calculation pipeline..." << std::endl;
    auto complex = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
        | fxt::mappend(5)
        | fxt::mappend(3)
        | fxt::mapply([](int a, int b) {
            std::cout << "  Computing sum: " << a << " + " << b << " = " << (a + b) << std::endl;
            return a + b;
        })
        | fxt::mapply([](int a, int b, int sum) {
            std::cout << "  Computing product: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::mapply([](int a, int b, int sum, int product) {
            std::cout << "  Computing average: (" << sum << " + " << product << ") / 2 = "
                      << ((sum + product) / 2.0) << std::endl;
            return (sum + product) / 2.0;
        });

    if (complex) {
        auto [a, b, sum, product, avg] = *complex;
        std::cout << "Final result:" << std::endl;
        std::cout << "  Inputs: " << a << ", " << b << std::endl;
        std::cout << "  Sum: " << sum << std::endl;
        std::cout << "  Product: " << product << std::endl;
        std::cout << "  Average: " << avg << std::endl;
    }

    // =========================================================================
    // Part 10: Mixed types example
    // =========================================================================
    std::cout << "\n\nPart 10: Mixed types example" << std::endl;
    std::cout << "----------------------------" << std::endl;

    auto mixed = fxt::optional<std::tuple<>>{std::tuple{}}
        | fxt::mappend(std::string{"Hello"})
        | fxt::mappend(42)
        | fxt::mappend(3.14)
        | fxt::mapply([](const std::string& s, int i, double d) {
            std::cout << "Building message from: \"" << s << "\", " << i << ", " << d << std::endl;
            return s + " " + std::to_string(i) + " " + std::to_string(d);
        });

    if (mixed) {
        auto [s, i, d, message] = *mixed;
        std::cout << "Result: \"" << message << "\"" << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}

