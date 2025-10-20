#include <fxt.hpp>
#include <iostream>
#include <string>

int main()
{
    // ========================================================================
    // Basic apply_replace with fxt::tuple
    // ========================================================================
    auto t1 = fxt::tuple{3, 4};
    auto result1 = fxt::apply_replace([](int a, int b) { return a + b; }, t1);
    // result1 is fxt::tuple{7} - original values replaced with result
    std::cout << "Basic apply_replace: " << fxt::get<0>(result1) << "\n";

    // ========================================================================
    // Pipeline usage with pipe operator
    // ========================================================================
    auto result2 = fxt::tuple{2, 3}
        | fxt::apply_replace([](int a, int b) { return a * b; })
        | fxt::apply_replace([](int product) { return product * 2; });
    // result2 is fxt::tuple{12}
    std::cout << "Chained apply_replace: " << fxt::get<0>(result2) << "\n";

    // ========================================================================
    // With flat_tuple
    // ========================================================================
    auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
    auto result3 = ft | fxt::apply_replace([](double a, double b) {
        return a / b;
    });
    // result3 is fxt::flat_tuple<double>{0.666...}
    std::cout << "flat_tuple apply_replace: " << fxt::get<0>(result3) << "\n";

    // ========================================================================
    // Multiple return values
    // ========================================================================
    auto t2 = fxt::tuple{10, 5};
    auto result4 = t2 | fxt::apply_replace([](int a, int b) {
        return fxt::tuple{a / b, a % b};
    });
    // result4 is fxt::tuple{fxt::tuple{2, 0}} (nested tuple)
    std::cout << "Multiple results: quotient=" << fxt::get<0>(fxt::get<0>(result4))
              << ", remainder=" << fxt::get<1>(fxt::get<0>(result4)) << "\n";

    // ========================================================================
    // Monadic apply_replace with fxt::expected
    // ========================================================================
    auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::tuple{10, 5}
    };
    auto result5 = exp | fxt::mapply_replace([](int a, int b) {
        return a / b;
    });
    // result5 is fxt::expected<fxt::tuple<int>, std::string>
    if (result5) {
        std::cout << "Expected mapply_replace: " << fxt::get<0>(result5.value()) << "\n";
    }

    // Error case propagation
    auto exp_err = fxt::expected<fxt::tuple<int, int>, std::string>{
        std::unexpected("division error")
    };
    auto result6 = exp_err | fxt::mapply_replace([](int a, int b) {
        return a + b;
    });
    if (!result6) {
        std::cout << "Error propagated: " << result6.error() << "\n";
    }

    // ========================================================================
    // Monadic apply_replace with fxt::optional
    // ========================================================================
    auto opt = fxt::optional<fxt::tuple<int, int>>{
        fxt::tuple{7, 8}
    };
    auto result7 = opt | fxt::mapply_replace([](int a, int b) {
        return a - b;
    });
    // result7 is fxt::optional<fxt::tuple<int>>
    if (result7) {
        std::cout << "Optional mapply_replace: " << fxt::get<0>(*result7) << "\n";
    }

    // Nullopt case propagation
    auto opt_null = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
    auto result8 = opt_null | fxt::mapply_replace([](int a, int b) {
        return a * b;
    });
    if (!result8) {
        std::cout << "Nullopt propagated\n";
    }

    // ========================================================================
    // Chaining monadic operations
    // ========================================================================
    auto result9 = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::tuple{2, 3}
    }
        | fxt::mapply_replace([](int a, int b) {
            return fxt::tuple{a + b, a * b};
        })
        | fxt::mapply_replace([](fxt::tuple<int, int> t) {
            return fxt::get<0>(t) + fxt::get<1>(t);
        });
    // result9 is fxt::expected<fxt::tuple<int>, std::string>
    if (result9) {
        std::cout << "Chained mapply_replace: " << fxt::get<0>(result9.value()) << "\n";
    }

    // ========================================================================
    // With void-returning function (empty tuple result)
    // ========================================================================
    int side_effect = 0;
    auto result10 = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{5, 10}}
        | fxt::mapply_replace([&](int a, int b) {
            side_effect = a + b;
        });
    // result10 is fxt::optional<fxt::tuple<>>
    std::cout << "Side effect value: " << side_effect << "\n";
    if (result10) {
        std::cout << "Result tuple size: " << fxt::tuple_size_v<std::remove_cvref_t<decltype(*result10)>> << "\n";
    }

    // ========================================================================
    // Type transformation example
    // ========================================================================
    auto t3 = fxt::tuple{10, 20};
    auto result11 = t3 | fxt::apply_replace([](int a, int b) {
        return std::to_string(a) + "+" + std::to_string(b);
    });
    // result11 is fxt::tuple<std::string>
    std::cout << "Type transformation: " << fxt::get<0>(result11) << "\n";

    return 0;
}