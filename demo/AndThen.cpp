//
// Created by kenne on 30-09-2025.
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

fxt::optional<int> square(int x) {
    return fxt::optional<int>{x * x};
}

fxt::expected<int, std::string> divide(int a, int b) {
    if (b == 0) {
        return fxt::unexpected<std::string>{"Division by zero"};
    }
    return fxt::expected<int, std::string>{a / b};
}

fxt::expected<int, std::string> add_ten(int x) {
    return fxt::expected<int, std::string>{x + 10};
}

int main() {
    std::cout << "=== AndThenWrapper Demonstration ===\n\n";

    // Demonstration with fxt::optional
    std::cout << "--- fxt::optional examples ---\n";

    // Example 1: Successful optional chaining
    auto opt_result1 = parse_int("5") | fxt::and_then(square);
    if (opt_result1.has_value()) {
        std::cout << "parse_int(\"5\") | and_then(square) = " << *opt_result1 << std::endl;
    } else {
        std::cout << "parse_int(\"5\") | and_then(square) = nullopt" << std::endl;
    }

    // Example 2: Failed optional chaining (invalid input)
    auto opt_result2 = parse_int("invalid") | fxt::and_then(square);
    if (opt_result2.has_value()) {
        std::cout << "parse_int(\"invalid\") | and_then(square) = " << *opt_result2 << std::endl;
    } else {
        std::cout << "parse_int(\"invalid\") | and_then(square) = nullopt" << std::endl;
    }

    // Example 3: Multiple chaining with optional
    auto opt_result3 = parse_int("3")
                     | fxt::and_then(square)
                     | fxt::and_then(square);
    if (opt_result3.has_value()) {
        std::cout << "parse_int(\"3\") | and_then(square) | and_then(square) = " << *opt_result3 << std::endl;
    } else {
        std::cout << "parse_int(\"3\") | and_then(square) | and_then(square) = nullopt" << std::endl;
    }

    std::cout << "\n--- fxt::expected examples ---\n";

    // Example 4: Successful expected chaining
    auto exp_result1 = divide(20, 4) | fxt::and_then(add_ten);
    if (exp_result1.has_value()) {
        std::cout << "divide(20, 4) | and_then(add_ten) = " << *exp_result1 << std::endl;
    } else {
        std::cout << "divide(20, 4) | and_then(add_ten) = error: " << exp_result1.error() << std::endl;
    }

    // Example 5: Failed expected chaining (division by zero)
    auto exp_result2 = divide(20, 0) | fxt::and_then(add_ten);
    if (exp_result2.has_value()) {
        std::cout << "divide(20, 0) | and_then(add_ten) = " << *exp_result2 << std::endl;
    } else {
        std::cout << "divide(20, 0) | and_then(add_ten) = error: " << exp_result2.error() << std::endl;
    }

    // Example 6: Multiple chaining with expected
    auto exp_result3 = divide(100, 5)
                     | fxt::and_then(add_ten)
                     | fxt::and_then([](int x) { return divide(x, 2); });
    if (exp_result3.has_value()) {
        std::cout << "divide(100, 5) | and_then(add_ten) | and_then(divide_by_2) = " << *exp_result3 << std::endl;
    } else {
        std::cout << "divide(100, 5) | and_then(add_ten) | and_then(divide_by_2) = error: " << exp_result3.error() << std::endl;
    }

    std::cout << "\n--- Mixed usage examples ---\n";

    // Example 7: Using the same and_then wrapper with different types
    auto and_then_square = fxt::and_then(square);
    auto and_then_add_ten = fxt::and_then(add_ten);

    auto mixed_result1 = parse_int("7") | and_then_square;
    auto mixed_result2 = divide(35, 7) | and_then_add_ten;

    std::cout << "Using reusable and_then wrappers:\n";
    if (mixed_result1.has_value()) {
        std::cout << "  parse_int(\"7\") | and_then_square = " << *mixed_result1 << std::endl;
    }
    if (mixed_result2.has_value()) {
        std::cout << "  divide(35, 7) | and_then_add_ten = " << *mixed_result2 << std::endl;
    }

    std::cout << "\n--- Complex chaining examples ---\n";

    // Example 8: Complex optional chain
    auto complex_opt = parse_int("2")
                     | fxt::and_then(square)           // 4
                     | fxt::and_then(square)           // 16
                     | fxt::and_then([](int x) {
                         return x > 10 ? fxt::optional<int>{x} : fxt::optional<int>{};
                       });
    std::cout << "Complex optional chain: ";
    if (complex_opt.has_value()) {
        std::cout << *complex_opt << std::endl;
    } else {
        std::cout << "nullopt" << std::endl;
    }

    // Example 9: Complex expected chain with mixed operations
    auto complex_exp = divide(60, 3)                  // 20
                     | fxt::and_then(add_ten)         // 30
                     | fxt::and_then([](int x) { return divide(x, 6); })  // 5
                     | fxt::and_then(add_ten);        // 15
    std::cout << "Complex expected chain: ";
    if (complex_exp.has_value()) {
        std::cout << *complex_exp << std::endl;
    } else {
        std::cout << "error: " << complex_exp.error() << std::endl;
    }

    std::cout << "\n--- Comparison with direct method calls ---\n";

    // Example 10: Show equivalent direct calls
    auto direct_opt = parse_int("4").and_then(square);
    auto piped_opt = parse_int("4") | fxt::and_then(square);

    std::cout << "Direct: parse_int(\"4\").and_then(square) = " << (direct_opt.has_value() ? std::to_string(*direct_opt) : "nullopt") << std::endl;
    std::cout << "Piped:  parse_int(\"4\") | and_then(square) = " << (piped_opt.has_value() ? std::to_string(*piped_opt) : "nullopt") << std::endl;

    return 0;
}
