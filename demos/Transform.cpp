//
// Created by kenne on 04/10/2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// Helper functions for demonstration
fxt::optional<int> parse_int(const std::string& str) {
    try {
        int value = std::stoi(str);
        return fxt::optional<int>{value};
    } catch (...) {
        return fxt::optional<int>{};
    }
}

fxt::expected<int, std::string> safe_divide(const int a, const int b) {
    if (b == 0) {
        return fxt::unexpected<std::string>{"Division by zero"};
    }
    return fxt::expected<int, std::string>{a / b};
}

int main() {
    std::cout << "=== Transform Demonstration ===\n\n";

    // Demonstration with fxt::optional
    std::cout << "--- fxt::optional examples ---\n";

    // Example 1: Transform value in optional
    if (auto opt_result1 = parse_int("42") | fxt::transform([](const int x) { return x * 2; }); opt_result1.has_value()) {
        std::cout << "parse_int(\"42\") | transform(*2) = " << *opt_result1 << std::endl;
    } else {
        std::cout << "parse_int(\"42\") | transform(*2) = nullopt" << std::endl;
    }

    // Example 2: Transform with empty optional
    if (auto opt_result2 = parse_int("invalid") | fxt::transform([](const int x) { return x * 2; }); opt_result2.has_value()) {
        std::cout << "parse_int(\"invalid\") | transform(*2) = " << *opt_result2 << std::endl;
    } else {
        std::cout << "parse_int(\"invalid\") | transform(*2) = nullopt" << std::endl;
    }

    // Example 3: Chain multiple transforms
    auto opt_result3 = parse_int("5")
                     | fxt::transform([](const int x) { return x * 2; })      // 10
                     | fxt::transform([](const int x) { return x + 3; })      // 13
                     | fxt::transform([](const int x) { return x * x; });     // 169
    if (opt_result3.has_value()) {
        std::cout << "parse_int(\"5\") | transform(*2) | transform(+3) | transform(x²) = "
                  << *opt_result3 << std::endl;
    }

    // Example 4: Transform type conversion
    auto opt_result4 = parse_int("123")
                     | fxt::transform([](const int x) { return std::to_string(x) + " units"; });
    if (opt_result4.has_value()) {
        std::cout << "parse_int(\"123\") | transform(to_string) = " << *opt_result4 << std::endl;
    }

    std::cout << "\n--- fxt::expected examples ---\n";

    // Example 5: Transform value in expected
    if (auto exp_result1 = safe_divide(20, 4) | fxt::transform([](const int x) { return x * 3; }); exp_result1.has_value()) {
        std::cout << "safe_divide(20, 4) | transform(*3) = " << *exp_result1 << std::endl;
    } else {
        std::cout << "safe_divide(20, 4) | transform(*3) = error: " << exp_result1.error() << std::endl;
    }

    // Example 6: Transform with error expected
    if (auto exp_result2 = safe_divide(20, 0) | fxt::transform([](const int x) { return x * 3; }); exp_result2.has_value()) {
        std::cout << "safe_divide(20, 0) | transform(*3) = " << *exp_result2 << std::endl;
    } else {
        std::cout << "safe_divide(20, 0) | transform(*3) = error: " << exp_result2.error() << std::endl;
    }

    // Example 7: Chain multiple transforms on expected
    auto exp_result3 = safe_divide(100, 5)
                     | fxt::transform([](const int x) { return x + 10; })     // 30
                     | fxt::transform([](const int x) { return x / 2; })      // 15
                     | fxt::transform([](const int x) { return x * x; });     // 225
    if (exp_result3.has_value()) {
        std::cout << "safe_divide(100, 5) | multiple transforms = " << *exp_result3 << std::endl;
    }

    // Example 8: Transform with type conversion on expected
    auto exp_result4 = safe_divide(50, 2)
                     | fxt::transform([](const int x) { return "Result: " + std::to_string(x); });
    if (exp_result4.has_value()) {
        std::cout << "safe_divide(50, 2) | transform(to_message) = " << *exp_result4 << std::endl;
    }

    std::cout << "\n--- Mixed operations examples ---\n";

    // Example 9: Combine transform with and_then
    auto mixed_result1 = parse_int("10")
                       | fxt::transform([](const int x) { return x * 2; })                    // 20
                       | fxt::and_then([](const int x) { return parse_int(std::to_string(x)); })  // optional<20>
                       | fxt::transform([](const int x) { return x + 5; });                   // 25
    std::cout << "Mixed optional operations: ";
    if (mixed_result1.has_value()) {
        std::cout << *mixed_result1 << std::endl;
    } else {
        std::cout << "nullopt" << std::endl;
    }

    // Example 10: Complex pipeline with expected
    auto mixed_result2 = safe_divide(60, 3)
                       | fxt::transform([](const int x) { return x + 10; })                       // 30
                       | fxt::and_then([](const int x) { return safe_divide(x, 6); })            // 5
                       | fxt::transform([](const int x) { return x * x; })                        // 25
                       | fxt::transform([](const int x) { return "Final: " + std::to_string(x); });
    std::cout << "Mixed expected operations: ";
    if (mixed_result2.has_value()) {
        std::cout << *mixed_result2 << std::endl;
    } else {
        std::cout << "error: " << mixed_result2.error() << std::endl;
    }

    std::cout << "\n--- Reusable transforms ---\n";

    // Example 11: Create reusable transform operations
    auto double_it = fxt::transform([](const int x) { return x * 2; });
    auto square_it = fxt::transform([](const int x) { return x * x; });
    auto stringify = fxt::transform([](const int x) { return std::to_string(x); });

    auto reusable_result1 = parse_int("7") | double_it | square_it;
    auto reusable_result2 = safe_divide(36, 6) | double_it | stringify;

    std::cout << "Using reusable transforms:\n";
    if (reusable_result1.has_value()) {
        std::cout << "  parse_int(\"7\") | double | square = " << *reusable_result1 << std::endl;
    }
    if (reusable_result2.has_value()) {
        std::cout << "  safe_divide(36, 6) | double | stringify = " << *reusable_result2 << std::endl;
    }

    std::cout << "\n--- Comparison with direct method calls ---\n";

    // Example 12: Show equivalent direct calls
    auto direct_opt = parse_int("8").transform([](const int x) { return x * 3; });
    auto piped_opt = parse_int("8") | fxt::transform([](const int x) { return x * 3; });

    std::cout << "Direct: parse_int(\"8\").transform(*3) = "
              << (direct_opt.has_value() ? std::to_string(*direct_opt) : "nullopt") << std::endl;
    std::cout << "Piped:  parse_int(\"8\") | transform(*3) = "
              << (piped_opt.has_value() ? std::to_string(*piped_opt) : "nullopt") << std::endl;

    return 0;
}

