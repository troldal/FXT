//
// Demo: fxt::apply_replace and fxt::mtuple_apply_replace
//
// This demo shows how to use fxt::apply_replace to apply functions to tuple values
// and replace all elements with the result.
//

#include <iostream>
#include <string>
#include <fxt.hpp>

int main()
{
    std::cout << "=== fxt::apply_replace and fxt::mtuple_apply_replace Demo ===" << std::endl;
    std::cout << std::endl;

    // =========================================================================
    // Part 1: Basic apply_replace with fxt::tuple
    // =========================================================================
    std::cout << "Part 1: Basic apply_replace with fxt::tuple" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;

    auto t1 = fxt::tuple{3, 4};
    std::cout << "Original tuple: (" << fxt::get<0>(t1) << ", " << fxt::get<1>(t1) << ")" << std::endl;

    auto result1 = fxt::apply_replace([](int a, int b) { return a + b; }, t1);
    std::cout << "After apply_replace with sum: (" << fxt::get<0>(result1) << ")" << std::endl;
    std::cout << "Note: Original 2 elements replaced with 1 result" << std::endl;

    // =========================================================================
    // Part 2: Pipeline usage with pipe operator
    // =========================================================================
    std::cout << "\n\nPart 2: Pipeline usage with pipe operator" << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    auto result2 = fxt::tuple{2, 3}
        | fxt::apply_replace([](int a, int b) {
            std::cout << "  Step 1: " << a << " * " << b << " = " << (a * b) << std::endl;
            return a * b;
        })
        | fxt::apply_replace([](int product) {
            std::cout << "  Step 2: " << product << " * 2 = " << (product * 2) << std::endl;
            return product * 2;
        });
    std::cout << "Final result: (" << fxt::get<0>(result2) << ")" << std::endl;

    // =========================================================================
    // Part 3: With flat_tuple
    // =========================================================================
    std::cout << "\n\nPart 3: With flat_tuple" << std::endl;
    std::cout << "-----------------------" << std::endl;

    auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
    std::cout << "Original flat_tuple: (" << fxt::get<0>(ft) << ", " << fxt::get<1>(ft) << ")" << std::endl;

    auto result3 = ft | fxt::apply_replace([](double a, double b) {
        return a / b;
    });
    std::cout << "After division: (" << fxt::get<0>(result3) << ")" << std::endl;

    // =========================================================================
    // Part 4: Multiple return values (nested tuple)
    // =========================================================================
    std::cout << "\n\nPart 4: Multiple return values (nested tuple)" << std::endl;
    std::cout << "----------------------------------------------" << std::endl;

    auto t2 = fxt::tuple{10, 5};
    std::cout << "Original tuple: (" << fxt::get<0>(t2) << ", " << fxt::get<1>(t2) << ")" << std::endl;

    auto result4 = t2 | fxt::apply_replace([](int a, int b) {
        return fxt::tuple{a / b, a % b};
    });
    std::cout << "Result (nested tuple): quotient=" << fxt::get<0>(fxt::get<0>(result4))
              << ", remainder=" << fxt::get<1>(fxt::get<0>(result4)) << std::endl;

    // =========================================================================
    // Part 5: Monadic apply_replace with fxt::expected
    // =========================================================================
    std::cout << "\n\nPart 5: Monadic apply_replace with fxt::expected" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::tuple{10, 5}
    };
    std::cout << "Expected with tuple: (" << fxt::get<0>(*exp) << ", " << fxt::get<1>(*exp) << ")" << std::endl;

    auto result5 = exp | fxt::mtuple_apply_replace([](int a, int b) {
        std::cout << "Computing: " << a << " / " << b << " = " << (a / b) << std::endl;
        return a / b;
    });

    if (result5) {
        std::cout << "Result: (" << fxt::get<0>(result5.value()) << ")" << std::endl;
    }

    // Error case propagation
    std::cout << "\nError propagation:" << std::endl;
    auto exp_err = fxt::expected<fxt::tuple<int, int>, std::string>{
        std::unexpected("division error")
    };
    auto result6 = exp_err | fxt::mtuple_apply_replace([](int a, int b) {
        std::cout << "This won't be called" << std::endl;
        return a + b;
    });
    if (!result6) {
        std::cout << "Error propagated: " << result6.error() << std::endl;
    }

    // =========================================================================
    // Part 6: Monadic apply_replace with fxt::optional
    // =========================================================================
    std::cout << "\n\nPart 6: Monadic apply_replace with fxt::optional" << std::endl;
    std::cout << "-------------------------------------------------" << std::endl;

    auto opt = fxt::optional<fxt::tuple<int, int>>{
        fxt::tuple{7, 8}
    };
    std::cout << "Optional with tuple: (" << fxt::get<0>(*opt) << ", " << fxt::get<1>(*opt) << ")" << std::endl;

    auto result7 = opt | fxt::mtuple_apply_replace([](int a, int b) {
        std::cout << "Computing: " << a << " - " << b << " = " << (a - b) << std::endl;
        return a - b;
    });

    if (result7) {
        std::cout << "Result: (" << fxt::get<0>(*result7) << ")" << std::endl;
    }

    // Nullopt case propagation
    std::cout << "\nNullopt propagation:" << std::endl;
    auto opt_null = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
    auto result8 = opt_null | fxt::mtuple_apply_replace([](int a, int b) {
        std::cout << "This won't be called" << std::endl;
        return a * b;
    });
    if (!result8) {
        std::cout << "Nullopt propagated successfully" << std::endl;
    }

    // =========================================================================
    // Part 7: Chaining monadic operations
    // =========================================================================
    std::cout << "\n\nPart 7: Chaining monadic operations" << std::endl;
    std::cout << "------------------------------------" << std::endl;

    auto result9 = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::tuple{2, 3}
    }
        | fxt::mtuple_apply_replace([](int a, int b) {
            std::cout << "  Step 1: Creating tuple(" << a << "+" << b << ", " << a << "*" << b << ")" << std::endl;
            return fxt::tuple{a + b, a * b};
        })
        | fxt::mtuple_apply_replace([](fxt::tuple<int, int> t) {
            auto sum = fxt::get<0>(t);
            auto product = fxt::get<1>(t);
            std::cout << "  Step 2: " << sum << " + " << product << " = " << (sum + product) << std::endl;
            return sum + product;
        });

    if (result9) {
        std::cout << "Final result: (" << fxt::get<0>(result9.value()) << ")" << std::endl;
    }

    // =========================================================================
    // Part 8: With void-returning function
    // =========================================================================
    std::cout << "\n\nPart 8: With void-returning function (empty tuple result)" << std::endl;
    std::cout << "-----------------------------------------------------------" << std::endl;

    int side_effect = 0;
    auto result10 = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{5, 10}}
        | fxt::mtuple_apply_replace([&](int a, int b) {
            side_effect = a + b;
            std::cout << "Side effect: setting variable to " << side_effect << std::endl;
        });

    std::cout << "Side effect value: " << side_effect << std::endl;
    if (result10) {
        std::cout << "Result tuple size: " << fxt::tuple_size_v<std::remove_cvref_t<decltype(*result10)>> << " (empty tuple)" << std::endl;
    }

    // =========================================================================
    // Part 9: Type transformation example
    // =========================================================================
    std::cout << "\n\nPart 9: Type transformation example" << std::endl;
    std::cout << "------------------------------------" << std::endl;

    auto t3 = fxt::tuple{10, 20};
    std::cout << "Original tuple: (" << fxt::get<0>(t3) << ", " << fxt::get<1>(t3) << ")" << std::endl;

    auto result11 = t3 | fxt::apply_replace([](int a, int b) {
        return std::to_string(a) + "+" + std::to_string(b);
    });
    std::cout << "After type transformation: (\"" << fxt::get<0>(result11) << "\")" << std::endl;
    std::cout << "Note: Changed from tuple<int, int> to tuple<string>" << std::endl;

    // =========================================================================
    // Part 10: Comparison with apply_append
    // =========================================================================
    std::cout << "\n\nPart 10: Comparison - apply_replace vs apply_append" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;

    auto base = fxt::tuple{3, 4};
    std::cout << "Base tuple: (" << fxt::get<0>(base) << ", " << fxt::get<1>(base) << ")" << std::endl;

    auto appended = base | fxt::apply_append([](int a, int b) { return a + b; });
    std::cout << "With apply_append: (" << fxt::get<0>(appended) << ", "
              << fxt::get<1>(appended) << ", " << fxt::get<2>(appended) << ")" << std::endl;

    auto replaced = base | fxt::apply_replace([](int a, int b) { return a + b; });
    std::cout << "With apply_replace: (" << fxt::get<0>(replaced) << ")" << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}


