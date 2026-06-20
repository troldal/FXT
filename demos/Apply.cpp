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
        | fxt::mtuple_append(5)
        | fxt::mtuple_append(10)
        | fxt::mtuple_apply_append([](int a, int b) {
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
        | fxt::mtuple_append(3)
        | fxt::mtuple_append(4)
        | fxt::mtuple_apply_append([](int a, int b) {
            std::cout << "  Step 1: " << a << " + " << b << " = " << (a + b) << std::endl;
            return a + b;
        })
        | fxt::mtuple_apply_append([](int a, int b, int sum) {
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
        | fxt::mtuple_append(1)
        | fxt::mtuple_append(2)
        | fxt::mtuple_apply_append([](int a, int b) {
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
        | fxt::mtuple_append(10.0)
        | fxt::mtuple_append(2.0)
        | fxt::mtuple_apply_append([](double a, double b) -> fxt::expected<double, std::string> {
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
        | fxt::mtuple_append(10.0)
        | fxt::mtuple_append(0.0)
        | fxt::mtuple_apply_append([](double a, double b) -> fxt::expected<double, std::string> {
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
        | fxt::mtuple_append(42)
        | fxt::mtuple_append(std::string{"hello"})
        | fxt::mtuple_apply_append([](int x, const std::string& s) {
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
        | fxt::mtuple_append(7)
        | fxt::mtuple_append(8)
        | fxt::mtuple_apply_append([](int a, int b) {
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
        | fxt::mtuple_append(2)
        | fxt::mtuple_append(3)
        | fxt::mtuple_apply_append([](int a, int b) {
            std::cout << "  Step 1: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::mtuple_apply_append([](int a, int b, int product) {
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
        | fxt::mtuple_append(1)
        | fxt::mtuple_append(2)
        | fxt::mtuple_apply_append([](int a, int b) {
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
        | fxt::mtuple_append(16.0)
        | fxt::mtuple_apply_append([](double x) -> fxt::optional<double> {
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
        | fxt::mtuple_append(-16.0)
        | fxt::mtuple_apply_append([](double x) -> fxt::optional<double> {
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
        | fxt::mtuple_append(99)
        | fxt::mtuple_append(std::string{"world"})
        | fxt::mtuple_apply_append([](int x, const std::string& s) {
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
        | fxt::mtuple_append(5)
        | fxt::mtuple_append(3)
        | fxt::mtuple_apply_append([](int a, int b) {
            std::cout << "  Computing sum: " << a << " + " << b << " = " << (a + b) << std::endl;
            return a + b;
        })
        | fxt::mtuple_apply_append([](int a, int b, int sum) {
            std::cout << "  Computing product: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::mtuple_apply_append([](int a, int b, int sum, int product) {
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
        | fxt::mtuple_append(std::string{"Hello"})
        | fxt::mtuple_append(42)
        | fxt::mtuple_append(3.14)
        | fxt::mtuple_apply_append([](const std::string& s, int i, double d) {
            std::cout << "Building message from: \"" << s << "\", " << i << ", " << d << std::endl;
            return s + " " + std::to_string(i) + " " + std::to_string(d);
        });

    if (mixed) {
        auto [s, i, d, message] = *mixed;
        std::cout << "Result: \"" << message << "\"" << std::endl;
    }

    // =========================================================================
    // Part 11: fxt::mtuple_apply with fxt::flat_tuple (in expected)
    // =========================================================================
    std::cout << "\n\nPart 11: fxt::mtuple_apply with fxt::flat_tuple (in expected)" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << "Creating expected with flat_tuple..." << std::endl;
    auto exp_flat = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
        | fxt::mtuple_append(2)
        | fxt::mtuple_append(3)
        | fxt::mtuple_apply_append([](int a, int b) {
            std::cout << "  Computing: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        });

    if (exp_flat) {
        auto product = fxt::get<2>(*exp_flat);
        std::cout << "Result in flat_tuple: " << product << std::endl;
    }

    // Chain multiple operations on flat_tuple
    std::cout << "\nChaining operations on flat_tuple:" << std::endl;
    auto exp_flat_chain = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
        | fxt::mtuple_append(10.0)
        | fxt::mtuple_append(5.0)
        | fxt::mtuple_apply_append([](double a, double b) {
            std::cout << "  Step 1: " << a << " / " << b << " = " << (a / b) << std::endl;
            return a / b;
        })
        | fxt::mtuple_apply_append([](double a, double b, double result) {
            std::cout << "  Step 2: " << result << " + 1.0 = " << (result + 1.0) << std::endl;
            return result + 1.0;
        });

    if (exp_flat_chain) {
        std::cout << "Final values: "
                  << fxt::get<0>(*exp_flat_chain) << ", "
                  << fxt::get<1>(*exp_flat_chain) << ", "
                  << fxt::get<2>(*exp_flat_chain) << ", "
                  << fxt::get<3>(*exp_flat_chain) << std::endl;
    }

    // =========================================================================
    // Part 12: fxt::mtuple_apply with fxt::flat_tuple (in optional)
    // =========================================================================
    std::cout << "\n\nPart 12: fxt::mtuple_apply with fxt::flat_tuple (in optional)" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    std::cout << "Creating optional with flat_tuple..." << std::endl;
    auto opt_flat = fxt::optional<fxt::flat_tuple<>>{fxt::flat_tuple<>{}}
        | fxt::mtuple_append(7)
        | fxt::mtuple_append(8)
        | fxt::mtuple_apply_append([](int a, int b) {
            std::cout << "  Computing: " << a << " + " << b << " = " << (a + b) << std::endl;
            return a + b;
        });

    if (opt_flat) {
        auto sum = fxt::get<2>(*opt_flat);
        std::cout << "Result in flat_tuple: " << sum << std::endl;
    }

    // With void-returning function
    std::cout << "\nVoid-returning function with flat_tuple:" << std::endl;
    auto opt_flat_void = fxt::optional<fxt::flat_tuple<>>{fxt::flat_tuple<>{}}
        | fxt::mtuple_append(42)
        | fxt::mtuple_append(std::string{"test"})
        | fxt::mtuple_apply_append([](int x, const std::string& s) {
            std::cout << "  Side effect: " << x << " and \"" << s << "\"" << std::endl;
        });

    if (opt_flat_void) {
        std::cout << "Flat_tuple preserved: "
                  << fxt::get<0>(*opt_flat_void) << ", \""
                  << fxt::get<1>(*opt_flat_void) << "\"" << std::endl;
    }

    // =========================================================================
    // Part 13: fxt::apply with fxt::tuple (direct call, no pipe)
    // =========================================================================
    std::cout << "\n\nPart 13: fxt::apply with fxt::tuple (direct call, no pipe)" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;

    auto tuple1 = fxt::make_tuple(5, 10, 15);
    std::cout << "Tuple: (5, 10, 15)" << std::endl;

    auto sum1 = fxt::apply([](int a, int b, int c) {
        std::cout << "  Computing sum: " << a << " + " << b << " + " << c << std::endl;
        return a + b + c;
    }, tuple1);
    std::cout << "Result: " << sum1 << std::endl;

    // With different types
    std::cout << "\nWith mixed types:" << std::endl;
    auto tuple2 = fxt::make_tuple(std::string{"Length:"}, 42);
    std::cout << "Tuple: (\"Length:\", 42)" << std::endl;

    auto message1 = fxt::apply([](const std::string& prefix, int value) {
        std::cout << "  Building message..." << std::endl;
        return prefix + " " + std::to_string(value);
    }, tuple2);
    std::cout << "Result: \"" << message1 << "\"" << std::endl;

    // With rvalue tuple
    std::cout << "\nWith rvalue tuple:" << std::endl;
    auto product1 = fxt::apply([](int a, int b) {
        std::cout << "  Computing: " << a << " * " << b << std::endl;
        return a * b;
    }, fxt::make_tuple(6, 7));
    std::cout << "Result: " << product1 << std::endl;

    // =========================================================================
    // Part 14: fxt::apply with fxt::tuple (with pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 14: fxt::apply with fxt::tuple (with pipe operator)" << std::endl;
    std::cout << "--------------------------------------------------------" << std::endl;

    auto tuple3 = fxt::make_tuple(2, 3, 4);
    std::cout << "Tuple: (2, 3, 4)" << std::endl;

    auto product2 = tuple3 | fxt::apply([](int a, int b, int c) {
        std::cout << "  Computing product: " << a << " * " << b << " * " << c << std::endl;
        return a * b * c;
    });
    std::cout << "Result: " << product2 << std::endl;

    // Chaining with other operations
    std::cout << "\nChaining multiple operations:" << std::endl;
    auto tuple4 = fxt::make_tuple(10, 5);
    std::cout << "Starting with tuple: (10, 5)" << std::endl;

    auto chain_result = tuple4
        | fxt::apply([](int a, int b) {
            std::cout << "  Step 1: " << a << " - " << b << " = " << (a - b) << std::endl;
            return a - b;
        });
    std::cout << "Result: " << chain_result << std::endl;

    // With void-returning lambda
    std::cout << "\nWith void-returning lambda:" << std::endl;
    auto tuple5 = fxt::make_tuple(100, std::string{"meters"});
    std::cout << "Tuple: (100, \"meters\")" << std::endl;

    tuple5 | fxt::apply([](int distance, const std::string& unit) {
        std::cout << "  Logging: Distance is " << distance << " " << unit << std::endl;
    });

    // =========================================================================
    // Part 15: fxt::apply with fxt::flat_tuple (direct call, no pipe)
    // =========================================================================
    std::cout << "\n\nPart 15: fxt::apply with fxt::flat_tuple (direct call, no pipe)" << std::endl;
    std::cout << "----------------------------------------------------------------" << std::endl;

    auto ft1 = fxt::make_flat_tuple(3.0, 4.0);
    std::cout << "Flat_tuple: (3.0, 4.0)" << std::endl;

    auto hypotenuse = fxt::apply([](double a, double b) {
        auto result = std::sqrt(a * a + b * b);
        std::cout << "  Computing: sqrt(" << a << "² + " << b << "²) = " << result << std::endl;
        return result;
    }, ft1);
    std::cout << "Result: " << hypotenuse << std::endl;

    // With more elements
    std::cout << "\nWith multiple elements:" << std::endl;
    auto ft2 = fxt::make_flat_tuple(1, 2, 3, 4, 5);
    std::cout << "Flat_tuple: (1, 2, 3, 4, 5)" << std::endl;

    auto sum2 = fxt::apply([](int a, int b, int c, int d, int e) {
        auto result = a + b + c + d + e;
        std::cout << "  Sum: " << a << " + " << b << " + " << c << " + " << d << " + " << e << " = " << result << std::endl;
        return result;
    }, ft2);
    std::cout << "Result: " << sum2 << std::endl;

    // With mixed types
    std::cout << "\nWith mixed types:" << std::endl;
    auto ft3 = fxt::make_flat_tuple(42, 3.14, std::string{"pi"});
    std::cout << "Flat_tuple: (42, 3.14, \"pi\")" << std::endl;

    fxt::apply([](int i, double d, const std::string& s) {
        std::cout << "  Values: " << i << ", " << d << ", \"" << s << "\"" << std::endl;
    }, ft3);

    // =========================================================================
    // Part 16: fxt::apply with fxt::flat_tuple (with pipe operator)
    // =========================================================================
    std::cout << "\n\nPart 16: fxt::apply with fxt::flat_tuple (with pipe operator)" << std::endl;
    std::cout << "--------------------------------------------------------------" << std::endl;

    auto ft4 = fxt::make_flat_tuple(8.0, 2.0);
    std::cout << "Flat_tuple: (8.0, 2.0)" << std::endl;

    auto division_result = ft4 | fxt::apply([](double a, double b) {
        std::cout << "  Computing: " << a << " / " << b << " = " << (a / b) << std::endl;
        return a / b;
    });
    std::cout << "Result: " << division_result << std::endl;

    // With computation
    std::cout << "\nWith complex computation:" << std::endl;
    auto ft5 = fxt::make_flat_tuple(10.0, 20.0, 30.0);
    std::cout << "Flat_tuple: (10.0, 20.0, 30.0)" << std::endl;

    auto average = ft5 | fxt::apply([](double a, double b, double c) {
        auto result = (a + b + c) / 3.0;
        std::cout << "  Average: (" << a << " + " << b << " + " << c << ") / 3 = " << result << std::endl;
        return result;
    });
    std::cout << "Result: " << average << std::endl;

    // Using temporary flat_tuple
    std::cout << "\nUsing temporary flat_tuple:" << std::endl;
    auto temp_result = fxt::make_flat_tuple(5, 6, 7, 8)
        | fxt::apply([](int a, int b, int c, int d) {
            std::cout << "  Product: " << a << " * " << b << " * " << c << " * " << d << " = " << (a * b * c * d) << std::endl;
            return a * b * c * d;
        });
    std::cout << "Result: " << temp_result << std::endl;

    // =========================================================================
    // Part 17: Comparing fxt::tuple vs fxt::flat_tuple with fxt::apply
    // =========================================================================
    std::cout << "\n\nPart 17: Comparing fxt::tuple vs fxt::flat_tuple with fxt::apply" << std::endl;
    std::cout << "-----------------------------------------------------------------" << std::endl;

    std::cout << "Same computation with both tuple types:" << std::endl;

    auto regular_tuple = fxt::make_tuple(12, 4);
    auto flat_tuple = fxt::make_flat_tuple(12, 4);

    std::cout << "\nUsing fxt::tuple:" << std::endl;
    auto result_regular = regular_tuple | fxt::apply([](int a, int b) {
        std::cout << "  Computing: " << a << " + " << b << " = " << (a + b) << std::endl;
        return a + b;
    });
    std::cout << "Result: " << result_regular << std::endl;

    std::cout << "\nUsing fxt::flat_tuple:" << std::endl;
    auto result_flat = flat_tuple | fxt::apply([](int a, int b) {
        std::cout << "  Computing: " << a << " + " << b << " = " << (a + b) << std::endl;
        return a + b;
    });
    std::cout << "Result: " << result_flat << std::endl;

    std::cout << "\nBoth produce the same result!" << std::endl;

    // =========================================================================
    // Part 18: Advanced - combining mapply and apply
    // =========================================================================
    std::cout << "\n\nPart 18: Advanced - combining mapply and apply" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    std::cout << "Using mapply on monadic container, then apply on extracted value:" << std::endl;

    auto monadic_result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
        | fxt::mtuple_append(3)
        | fxt::mtuple_append(4)
        | fxt::mtuple_apply_append([](int a, int b) {
            std::cout << "  In monadic context: Computing " << a << " * " << b << std::endl;
            return a * b;
        });

    if (monadic_result) {
        std::cout << "Extracting flat_tuple from expected..." << std::endl;
        auto extracted = *monadic_result;

        auto final_result = extracted | fxt::apply([](int a, int b, int product) {
            std::cout << "  Direct apply: Sum of all = " << (a + b + product) << std::endl;
            return a + b + product;
        });

        std::cout << "Final result: " << final_result << std::endl;
    }

    // =========================================================================
    // Part 19: Real-world example - coordinate calculations
    // =========================================================================
    std::cout << "\n\nPart 19: Real-world example - coordinate calculations" << std::endl;
    std::cout << "-----------------------------------------------------" << std::endl;

    std::cout << "Computing distance between two 2D points:" << std::endl;

    auto point1 = fxt::make_flat_tuple(0.0, 0.0);
    auto point2 = fxt::make_flat_tuple(3.0, 4.0);

    std::cout << "Point 1: (0.0, 0.0)" << std::endl;
    std::cout << "Point 2: (3.0, 4.0)" << std::endl;

    // Compute distance using point2 directly
    auto distance = point2 | fxt::apply([](double x, double y) {
        auto dist = std::sqrt(x * x + y * y);
        std::cout << "  Distance from origin: sqrt(" << x << "² + " << y << "²) = " << dist << std::endl;
        return dist;
    });
    std::cout << "Distance: " << distance << std::endl;

    // More complex: area of triangle
    std::cout << "\nComputing area of triangle (base, height):" << std::endl;
    auto triangle = fxt::make_tuple(10.0, 5.0);
    std::cout << "Base: 10.0, Height: 5.0" << std::endl;

    auto area = fxt::apply([](double base, double height) {
        auto a = 0.5 * base * height;
        std::cout << "  Area: 0.5 * " << base << " * " << height << " = " << a << std::endl;
        return a;
    }, triangle);
    std::cout << "Area: " << area << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;


    // 1. Basic usage with fxt::optional and regular return type
    std::cout << "1. Optional with regular function:\n";
    auto opt_tuple = std::optional{std::tuple{3, 4}};
    auto result1 = opt_tuple | fxt::mtuple_apply_append([](int a, int b) {
        return a + b;
    });
    std::cout << "   Result: " << (result1 ? std::to_string(fxt::get<2>(*result1)) : "nullopt") << "\n\n";


    return 0;
}



