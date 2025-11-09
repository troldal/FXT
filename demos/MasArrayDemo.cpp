/*
    MasArrayDemo - Demonstration of fxt::mas_array functionality
    Shows monadic as_array converting tuples inside monads
*/

#include <fxt.hpp>
#include <iostream>
#include <string>

int main()
{
    std::cout << "=== FXT mas_array Demo ===" << std::endl << std::endl;

    // ==========================================================================
    // Example 1: Optional containing tuple of non-monadic values
    // ==========================================================================
    std::cout << "Example 1: Optional with non-monadic tuple elements" << std::endl;

    std::optional<fxt::tuple<int, double, int>> opt1 = fxt::make_tuple(1, 2.5, 3);
    auto result1 = opt1 | fxt::mas_array<double>();

    std::cout << "Input: optional<tuple<int, double, int>>" << std::endl;
    if (result1) {
        std::cout << "Result array: ";
        for (const auto& val : *result1) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Type: std::optional<std::array<double, 3>>" << std::endl << std::endl;

    // ==========================================================================
    // Example 2: Expected containing tuple of non-monadic values
    // ==========================================================================
    std::cout << "Example 2: Expected with non-monadic tuple elements" << std::endl;

    fxt::expected<fxt::tuple<int, int, int>, std::string> exp1 = fxt::make_tuple(10, 20, 30);
    auto result2 = exp1 | fxt::mas_array<int>();

    std::cout << "Input: expected<tuple<int, int, int>, string>" << std::endl;
    if (result2) {
        std::cout << "Result array: ";
        for (const auto& val : *result2) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Type: fxt::expected<std::array<int, 3>, std::string>" << std::endl << std::endl;

    // ==========================================================================
    // Example 3: Optional containing tuple of optional values (monadic elements)
    // ==========================================================================
    std::cout << "Example 3: Optional with monadic tuple elements" << std::endl;

    using OptInt = std::optional<int>;
    std::optional<fxt::tuple<OptInt, OptInt, OptInt>> opt2 =
        fxt::make_tuple(OptInt{1}, OptInt{2}, OptInt{3});

    auto result3 = opt2 | fxt::mas_array<double>();

    std::cout << "Input: optional<tuple<optional<int>, optional<int>, optional<int>>>" << std::endl;
    if (result3) {
        std::cout << "Result array of optionals: ";
        for (const auto& opt : *result3) {
            if (opt) {
                std::cout << *opt << " ";
            } else {
                std::cout << "nullopt ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Type: std::optional<std::array<std::optional<double>, 3>>" << std::endl << std::endl;

    // ==========================================================================
    // Example 4: Expected containing tuple of expected values (monadic elements)
    // ==========================================================================
    std::cout << "Example 4: Expected with monadic tuple elements" << std::endl;

    using ExpInt = fxt::expected<int, std::string>;
    fxt::expected<fxt::tuple<ExpInt, ExpInt, ExpInt>, std::string> exp2 =
        fxt::make_tuple(ExpInt{100}, ExpInt{200}, ExpInt{300});

    auto result4 = exp2 | fxt::mas_array<double>();

    std::cout << "Input: expected<tuple<expected<int>, expected<int>, expected<int>>, string>" << std::endl;
    if (result4) {
        std::cout << "Result array of expected: ";
        for (const auto& exp : *result4) {
            if (exp) {
                std::cout << *exp << " ";
            } else {
                std::cout << "error ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << "Type: fxt::expected<std::array<fxt::expected<double, std::string>, 3>, std::string>" << std::endl << std::endl;

    // ==========================================================================
    // Example 5: Empty optional (no value case)
    // ==========================================================================
    std::cout << "Example 5: Empty optional (no value)" << std::endl;

    std::optional<fxt::tuple<int, int, int>> opt_empty;
    auto result5 = opt_empty | fxt::mas_array<double>();

    std::cout << "Input: empty optional" << std::endl;
    std::cout << "Has value: " << (result5.has_value() ? "true" : "false") << std::endl << std::endl;

    // ==========================================================================
    // Example 6: Expected with error (error case)
    // ==========================================================================
    std::cout << "Example 6: Expected with error" << std::endl;

    fxt::expected<fxt::tuple<int, int, int>, std::string> exp_error =
        std::unexpected("Computation failed");
    auto result6 = exp_error | fxt::mas_array<double>();

    std::cout << "Input: expected with error" << std::endl;
    std::cout << "Has value: " << (result6.has_value() ? "true" : "false") << std::endl;
    if (!result6) {
        std::cout << "Error: " << result6.error() << std::endl;
    }
    std::cout << std::endl;

    // ==========================================================================
    // Example 7: Using with flat_tuple
    // ==========================================================================
    std::cout << "Example 7: Optional with flat_tuple" << std::endl;

    std::optional<fxt::flat_tuple<double, double, double>> opt_flat =
        fxt::make_flat_tuple(1.1, 2.2, 3.3);
    auto result7 = opt_flat | fxt::mas_array<int>();

    std::cout << "Input: optional<flat_tuple<double, double, double>>" << std::endl;
    if (result7) {
        std::cout << "Result array (truncated): ";
        for (const auto& val : *result7) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
    std::cout << "Type: std::optional<std::array<int, 3>>" << std::endl << std::endl;

    // ==========================================================================
    // Example 8: Chaining with other operations
    // ==========================================================================
    std::cout << "Example 8: Chaining operations" << std::endl;

    auto result8 = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{fxt::make_tuple(1, 2, 3, 4)}
        | fxt::mas_array<double>()
        | fxt::transform([](const auto& arr) {
            double sum = 0;
            for (const auto& val : arr) {
                sum += val;
            }
            return sum;
        });

    if (result8) {
        std::cout << "Sum of array elements: " << *result8 << std::endl;
    }
    std::cout << std::endl;

    return 0;
}

