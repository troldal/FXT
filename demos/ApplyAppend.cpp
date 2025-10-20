//
// Demo: fxt::apply_append and fxt::mapply_append
//
// This demo shows how to use fxt::apply_append to apply functions to tuple values
// and append the result to the original tuple.
//

#include <iostream>
#include <string>
#include <fxt.hpp>

int main()
{
    std::cout << "=== fxt::apply_append and fxt::mapply_append Demo ===" << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Part 1: Basic apply_append with fxt::tuple
    // =========================================================================
    std::cout << "Part 1: Basic apply_append with fxt::tuple" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    auto t1 = fxt::tuple{3, 4};
    std::cout << "Original tuple: (" << fxt::get<0>(t1) << ", " << fxt::get<1>(t1) << ")" << std::endl;

    auto result1 = fxt::apply_append([](int a, int b) { return a + b; }, t1);
    std::cout << "After apply_append with sum: ("
              << fxt::get<0>(result1) << ", "
              << fxt::get<1>(result1) << ", "
              << fxt::get<2>(result1) << ")" << std::endl;
    std::cout << "Note: Original elements preserved, sum appended" << std::endl;

    // =========================================================================
    // Part 2: Pipeline usage with pipe operator
    // =========================================================================
    std::cout << "\n\nPart 2: Pipeline usage with pipe operator" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    auto result2 = fxt::tuple{2, 3}
        | fxt::apply_append([](int a, int b) {
            std::cout << "  Step 1: Computing " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::apply_append([](int a, int b, int product) {
            std::cout << "  Step 2: Computing " << a << " + " << b << " + " << product << " = " << (a + b + product) << std::endl;
            return a + b + product;
        });

    std::cout << "Final result: ("
              << fxt::get<0>(result2) << ", "
              << fxt::get<1>(result2) << ", "
              << fxt::get<2>(result2) << ", "
              << fxt::get<3>(result2) << ")" << std::endl;

    // =========================================================================
    // Part 3: With flat_tuple
    // =========================================================================
    std::cout << "\n\nPart 3: With flat_tuple" << std::endl;
    std::cout << "-----------------------" << std::endl;

    auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
    std::cout << "Original flat_tuple: (" << fxt::get<0>(ft) << ", " << fxt::get<1>(ft) << ")" << std::endl;

    auto result3 = ft | fxt::apply_append([](double a, double b) {
        return a / b;
    });
    std::cout << "After appending division result: ("
              << fxt::get<0>(result3) << ", "
              << fxt::get<1>(result3) << ", "
              << fxt::get<2>(result3) << ")" << std::endl;

    // =========================================================================
    // Part 4: Type transformation
    // =========================================================================
    std::cout << "\n\nPart 4: Type transformation" << std::endl;
    std::cout << "----------------------------" << std::endl;

    auto t2 = fxt::tuple{10, 20};
    std::cout << "Original tuple: (" << fxt::get<0>(t2) << ", " << fxt::get<1>(t2) << ")" << std::endl;

    auto result4 = t2 | fxt::apply_append([](int a, int b) {
        return std::to_string(a) + "+" + std::to_string(b) + "=" + std::to_string(a + b);
    });
    std::cout << "After appending formatted string: ("
              << fxt::get<0>(result4) << ", "
              << fxt::get<1>(result4) << ", \""
              << fxt::get<2>(result4) << "\")" << std::endl;
    std::cout << "Note: Tuple now contains mixed types (int, int, string)" << std::endl;

    // =========================================================================
    // Part 5: Monadic apply_append with fxt::expected
    // =========================================================================
    std::cout << "\n\nPart 5: Monadic apply_append with fxt::expected" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 5}};
    std::cout << "Expected with tuple: (" << fxt::get<0>(*exp) << ", " << fxt::get<1>(*exp) << ")" << std::endl;

    auto result5 = exp | fxt::mapply_append([](int a, int b) {
        std::cout << "Computing: " << a << " / " << b << " = " << (a / b) << std::endl;
        return a / b;
    });

    if (result5) {
        std::cout << "Result tuple: ("
                  << fxt::get<0>(result5.value()) << ", "
                  << fxt::get<1>(result5.value()) << ", "
                  << fxt::get<2>(result5.value()) << ")" << std::endl;
    }

    // Error case propagation
    std::cout << "\nError propagation:" << std::endl;
    auto exp_err = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::unexpected("computation error")
    };
    auto result6 = exp_err | fxt::mapply_append([](int a, int b) {
        std::cout << "This won't be called" << std::endl;
        return a + b;
    });
    if (!result6) {
        std::cout << "Error propagated: " << result6.error() << std::endl;
    }

    // =========================================================================
    // Part 6: Chaining multiple mapply_append operations
    // =========================================================================
    std::cout << "\n\nPart 6: Chaining multiple mapply_append operations" << std::endl;
    std::cout << "---------------------------------------------------" << std::endl;

    auto result7 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{2, 3}}
        | fxt::mapply_append([](int a, int b) {
            std::cout << "  Step 1: Appending sum " << (a + b) << std::endl;
            return a + b;
        })
        | fxt::mapply_append([](int a, int b, int sum) {
            std::cout << "  Step 2: Appending product " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::mapply_append([](int a, int b, int sum, int product) {
            std::cout << "  Step 3: Appending total " << (sum + product) << std::endl;
            return sum + product;
        });

    if (result7) {
        auto& val = result7.value();
        std::cout << "Final result: ("
                  << fxt::get<0>(val) << ", "
                  << fxt::get<1>(val) << ", "
                  << fxt::get<2>(val) << ", "
                  << fxt::get<3>(val) << ", "
                  << fxt::get<4>(val) << ")" << std::endl;
        std::cout << "Explanation: (2, 3, 2+3=5, 2*3=6, 5+6=11)" << std::endl;
    }

    // =========================================================================
    // Part 7: Monadic apply_append with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 7: Monadic apply_append with fxt::optional" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{7, 8}};
    std::cout << "Optional with tuple: (" << fxt::get<0>(*opt) << ", " << fxt::get<1>(*opt) << ")" << std::endl;

    auto result8 = opt | fxt::mapply_append([](int a, int b) {
        std::cout << "Computing: " << a << " - " << b << " = " << (a - b) << std::endl;
        return a - b;
    });

    if (result8) {
        std::cout << "Result tuple: ("
                  << fxt::get<0>(*result8) << ", "
                  << fxt::get<1>(*result8) << ", "
                  << fxt::get<2>(*result8) << ")" << std::endl;
    }

    // Nullopt case propagation
    std::cout << "\nNullopt propagation:" << std::endl;
    auto opt_null = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
    auto result9 = opt_null | fxt::mapply_append([](int a, int b) {
        std::cout << "This won't be called" << std::endl;
        return a * b;
    });
    if (!result9) {
        std::cout << "Nullopt propagated successfully" << std::endl;
    }

    // =========================================================================
    // Part 8: Functions returning monadic types
    // =========================================================================
    std::cout << "\n\nPart 8: Functions returning monadic types" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    auto exp2 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 2}};
    auto result10 = exp2 | fxt::mapply_append([](int a, int b) -> fxt::expected<int, std::string> {
        std::cout << "Checking if b is zero..." << std::endl;
        if (b == 0) {
            std::cout << "Error: Division by zero!" << std::endl;
            return fxt::unexpected("division by zero");
        }
        std::cout << "Computing: " << a << " / " << b << " = " << (a / b) << std::endl;
        return a / b;
    });

    if (result10) {
        std::cout << "Result tuple: ("
                  << fxt::get<0>(result10.value()) << ", "
                  << fxt::get<1>(result10.value()) << ", "
                  << fxt::get<2>(result10.value()) << ")" << std::endl;
        std::cout << "Note: Monadic result was automatically unwrapped and appended" << std::endl;
    }

    // Test with division by zero
    std::cout << "\nTesting with division by zero:" << std::endl;
    auto exp3 = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 0}};
    auto result11 = exp3 | fxt::mapply_append([](int a, int b) -> fxt::expected<int, std::string> {
        std::cout << "Checking if b is zero..." << std::endl;
        if (b == 0) {
            std::cout << "Error: Division by zero!" << std::endl;
            return fxt::unexpected("division by zero");
        }
        return a / b;
    });

    if (!result11) {
        std::cout << "Error caught: " << result11.error() << std::endl;
    }

    // =========================================================================
    // Part 9: Void-returning functions
    // =========================================================================
    std::cout << "\n\nPart 9: Void-returning functions" << std::endl;
    std::cout << "---------------------------------" << std::endl;

    int side_effect = 0;
    auto opt2 = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{5, 10}};
    auto result12 = opt2 | fxt::mapply_append([&](int a, int b) {
        side_effect = a + b;
        std::cout << "Side effect: setting variable to " << side_effect << std::endl;
    });

    std::cout << "Side effect value: " << side_effect << std::endl;
    std::cout << "Tuple unchanged (void returns don't append): ("
              << fxt::get<0>(*result12) << ", "
              << fxt::get<1>(*result12) << ")" << std::endl;

    // =========================================================================
    // Part 10: Building complex computation pipelines
    // =========================================================================
    std::cout << "\n\nPart 10: Building complex computation pipelines" << std::endl;
    std::cout << "------------------------------------------------" << std::endl;

    auto computation = fxt::expected<fxt::tuple<double, double>, std::string>{fxt::tuple{3.0, 4.0}}
        | fxt::mapply_append([](double x, double y) {
            auto hypotenuse = std::sqrt(x * x + y * y);
            std::cout << "  Hypotenuse: sqrt(" << x << "² + " << y << "²) = " << hypotenuse << std::endl;
            return hypotenuse;
        })
        | fxt::mapply_append([](double x, double y, double h) {
            auto area = (x * y) / 2.0;
            std::cout << "  Triangle area: (" << x << " * " << y << ") / 2 = " << area << std::endl;
            return area;
        })
        | fxt::mapply_append([](double x, double y, double h, double area) {
            auto perimeter = x + y + h;
            std::cout << "  Perimeter: " << x << " + " << y << " + " << h << " = " << perimeter << std::endl;
            return perimeter;
        });

    if (computation) {
        auto& val = computation.value();
        std::cout << "\nFinal result tuple:" << std::endl;
        std::cout << "  Sides: (" << fxt::get<0>(val) << ", " << fxt::get<1>(val) << ")" << std::endl;
        std::cout << "  Hypotenuse: " << fxt::get<2>(val) << std::endl;
        std::cout << "  Area: " << fxt::get<3>(val) << std::endl;
        std::cout << "  Perimeter: " << fxt::get<4>(val) << std::endl;
    }

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}

