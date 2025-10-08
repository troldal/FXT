//
// Created by kenne on 08/10/2025.
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

fxt::expected<int, std::string> safe_divide(int a, int b) {
    if (b == 0) {
        return fxt::unexpected<std::string>{"Division by zero"};
    }
    return fxt::expected<int, std::string>{a / b};
}

fxt::expected<std::string, std::string> get_config(const std::string& key) {
    if (key == "username") {
        return fxt::expected<std::string, std::string>{"admin"};
    }
    return fxt::unexpected<std::string>{"Key not found: " + key};
}

int main() {
    std::cout << "=== Logical OR (||) Operator Demonstration ===\n\n";

    // ========================================
    // Section 1: expected || expected
    // ========================================
    std::cout << "--- expected || expected ---\n";

    // Example 1: First has value, return first
    auto exp1 = safe_divide(10, 2);  // Returns 5
    auto exp2 = safe_divide(20, 4);  // Returns 5
    auto result1 = exp1 || exp2;
    std::cout << "safe_divide(10, 2) || safe_divide(20, 4) = ";
    if (result1.has_value()) {
        std::cout << *result1 << " (first has value, returns first)\n";
    } else {
        std::cout << "Error: " << result1.error() << "\n";
    }

    // Example 2: First has error, return second
    auto exp3 = safe_divide(10, 0);  // Error: Division by zero
    auto exp4 = safe_divide(20, 4);  // Returns 5
    auto result2 = exp3 || exp4;
    std::cout << "safe_divide(10, 0) || safe_divide(20, 4) = ";
    if (result2.has_value()) {
        std::cout << *result2 << " (first has error, returns second)\n";
    } else {
        std::cout << "Error: " << result2.error() << "\n";
    }

    // Example 3: Both have errors, return second error
    auto exp5 = safe_divide(10, 0);  // Error: Division by zero
    auto exp6 = safe_divide(20, 0);  // Error: Division by zero
    auto result3 = exp5 || exp6;
    std::cout << "safe_divide(10, 0) || safe_divide(20, 0) = ";
    if (result3.has_value()) {
        std::cout << *result3 << "\n";
    } else {
        std::cout << "Error: " << result3.error() << " (both have errors, returns second)\n";
    }

    std::cout << "\n";

    // ========================================
    // Section 2: expected || value
    // ========================================
    std::cout << "--- expected || value ---\n";

    // Example 4: expected has value, return it
    auto exp7 = safe_divide(10, 2);  // Returns 5
    auto result4 = exp7 || 999;
    std::cout << "safe_divide(10, 2) || 999 = ";
    if (result4.has_value()) {
        std::cout << *result4 << " (expected has value, returns it)\n";
    }

    // Example 5: expected has error, return fallback value
    auto exp8 = safe_divide(10, 0);  // Error: Division by zero
    auto result5 = exp8 || 999;
    std::cout << "safe_divide(10, 0) || 999 = ";
    if (result5.has_value()) {
        std::cout << *result5 << " (expected has error, returns fallback)\n";
    }

    std::cout << "\n";

    // ========================================
    // Section 3: optional || optional
    // ========================================
    std::cout << "--- optional || optional ---\n";

    // Example 6: First has value, return first
    auto opt1 = parse_int("42");      // Returns 42
    auto opt2 = parse_int("100");     // Returns 100
    auto result6 = opt1 || opt2;
    std::cout << "parse_int(\"42\") || parse_int(\"100\") = ";
    if (result6.has_value()) {
        std::cout << *result6 << " (first has value, returns first)\n";
    } else {
        std::cout << "nullopt\n";
    }

    // Example 7: First is empty, return second
    auto opt3 = parse_int("invalid");  // Returns nullopt
    auto opt4 = parse_int("100");      // Returns 100
    auto result7 = opt3 || opt4;
    std::cout << "parse_int(\"invalid\") || parse_int(\"100\") = ";
    if (result7.has_value()) {
        std::cout << *result7 << " (first is empty, returns second)\n";
    } else {
        std::cout << "nullopt\n";
    }

    // Example 8: Both are empty, return empty
    auto opt5 = parse_int("invalid1");  // Returns nullopt
    auto opt6 = parse_int("invalid2");  // Returns nullopt
    auto result8 = opt5 || opt6;
    std::cout << "parse_int(\"invalid1\") || parse_int(\"invalid2\") = ";
    if (result8.has_value()) {
        std::cout << *result8 << "\n";
    } else {
        std::cout << "nullopt (both are empty, returns empty)\n";
    }

    std::cout << "\n";

    // ========================================
    // Section 4: optional || value
    // ========================================
    std::cout << "--- optional || value ---\n";

    // Example 9: optional has value, return it
    auto opt7 = parse_int("42");  // Returns 42
    auto result9 = opt7 || 999;
    std::cout << "parse_int(\"42\") || 999 = ";
    if (result9.has_value()) {
        std::cout << *result9 << " (optional has value, returns it)\n";
    }

    // Example 10: optional is empty, return fallback value
    auto opt8 = parse_int("invalid");  // Returns nullopt
    auto result10 = opt8 || 999;
    std::cout << "parse_int(\"invalid\") || 999 = ";
    if (result10.has_value()) {
        std::cout << *result10 << " (optional is empty, returns fallback)\n";
    }

    std::cout << "\n";

    // ========================================
    // Section 5: Chaining with other operators
    // ========================================
    std::cout << "--- Chaining with other operators ---\n";

    // Example 11: Using || with transform
    auto result11 = parse_int("invalid")
                    || parse_int("50")
                    | fxt::transform([](int x) { return x * 2; });
    std::cout << "(parse_int(\"invalid\") || parse_int(\"50\")) | transform(*2) = ";
    if (result11.has_value()) {
        std::cout << *result11 << "\n";
    }

    // Example 12: Using || with expected and transform
    auto result12 = safe_divide(10, 0)  // Error
                    || safe_divide(20, 4)  // Returns 5
                    | fxt::transform([](int x) { return x * 10; });
    std::cout << "(safe_divide(10, 0) || safe_divide(20, 4)) | transform(*10) = ";
    if (result12.has_value()) {
        std::cout << *result12 << "\n";
    } else {
        std::cout << "Error: " << result12.error() << "\n";
    }

    // Example 13: Multiple || chains
    auto result13 = parse_int("bad1")
                    || parse_int("bad2")
                    || parse_int("42")
                    || parse_int("100");
    std::cout << "parse_int(\"bad1\") || parse_int(\"bad2\") || parse_int(\"42\") || parse_int(\"100\") = ";
    if (result13.has_value()) {
        std::cout << *result13 << " (first successful parse)\n";
    }

    std::cout << "\n";

    // ========================================
    // Section 6: Working with strings
    // ========================================
    std::cout << "--- Working with strings ---\n";

    // Example 14: String expected with fallback
    auto config1 = get_config("username");  // Returns "admin"
    auto config2 = get_config("password");  // Error
    auto result14 = config1 || config2;
    std::cout << "get_config(\"username\") || get_config(\"password\") = ";
    if (result14.has_value()) {
        std::cout << *result14 << "\n";
    }

    // Example 15: String expected with literal fallback
    auto config3 = get_config("missing_key");  // Error
    auto result15 = config3 || std::string("default_value");
    std::cout << "get_config(\"missing_key\") || \"default_value\" = ";
    if (result15.has_value()) {
        std::cout << *result15 << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";

    return 0;
}

