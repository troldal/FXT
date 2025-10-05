//
// Created by kenne on 05/10/2025.
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// Helper functions for demonstrations
fxt::expected<int, std::string> safe_divide(int a, int b) {
    if (b == 0) {
        return fxt::unexpected<std::string>{"Division by zero"};
    }
    return a / b;
}

fxt::expected<double, std::string> safe_sqrt(double x) {
    if (x < 0) {
        return fxt::unexpected<std::string>{"Cannot take square root of negative number"};
    }
    return std::sqrt(x);
}

fxt::optional<int> parse_int(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return fxt::nullopt;
    }
}

fxt::optional<std::string> find_user(int id) {
    if (id == 1) return std::string{"Alice"};
    if (id == 2) return std::string{"Bob"};
    return fxt::nullopt;
}

int main() {
    std::cout << "=== fxt::match Demo ===\n\n";

    // Example 1: Basic match with expected (success case)
    std::cout << "1. Basic match with expected (success case):\n";
    auto result1 = safe_divide(10, 2)
        | fxt::match(
            [](int value) { return "Result: " + std::to_string(value); },
            [](const std::string& err) { return "Error: " + err; }
        );
    std::cout << "   10 / 2 = " << result1 << "\n\n";

    // Example 2: Basic match with expected (error case)
    std::cout << "2. Basic match with expected (error case):\n";
    auto result2 = safe_divide(10, 0)
        | fxt::match(
            [](int value) { return "Result: " + std::to_string(value); },
            [](const std::string& err) { return "Error: " + err; }
        );
    std::cout << "   10 / 0 = " << result2 << "\n\n";

    // Example 3: Match with optional (value present)
    std::cout << "3. Match with optional (value present):\n";
    auto result3 = parse_int("42")
        | fxt::match(
            [](int value) { return "Parsed: " + std::to_string(value); },
            []() { return std::string{"Failed to parse"}; }
        );
    std::cout << "   parse_int(\"42\") = " << result3 << "\n\n";

    // Example 4: Match with optional (empty case)
    std::cout << "4. Match with optional (empty case):\n";
    auto result4 = parse_int("not_a_number")
        | fxt::match(
            [](int value) { return "Parsed: " + std::to_string(value); },
            []() { return std::string{"Failed to parse"}; }
        );
    std::cout << "   parse_int(\"not_a_number\") = " << result4 << "\n\n";

    // Example 5: Match returning different types (converted to common type)
    std::cout << "5. Match with numeric result:\n";
    auto result5 = safe_divide(20, 4)
        | fxt::match(
            [](int value) { return value * 2; },
            [](const std::string&) { return -1; }
        );
    std::cout << "   (20 / 4) * 2 = " << result5 << "\n\n";

    // Example 6: Match with side effects
    std::cout << "6. Match with side effects:\n";
    safe_divide(100, 5)
        | fxt::match(
            [](int value) {
                std::cout << "   Success! Result is: " << value << "\n";
                return value;
            },
            [](const std::string& err) {
                std::cout << "   Failure! Error: " << err << "\n";
                return 0;
            }
        );
    std::cout << "\n";

    // Example 7: Chaining operations with match
    std::cout << "7. Chaining operations:\n";
    auto chain_result = safe_divide(16, 2)
        | fxt::transform([](int x) { return x * 3; })
        | fxt::match(
            [](int value) { return "Final result: " + std::to_string(value); },
            [](const std::string& err) { return "Error occurred: " + err; }
        );
    std::cout << "   " << chain_result << "\n\n";

    // Example 8: Match with optional for default values
    std::cout << "8. Match with optional for default values:\n";
    auto username1 = find_user(1)
        | fxt::match(
            [](const std::string& name) { return name; },
            []() { return std::string{"Guest"}; }
        );
    std::cout << "   User 1: " << username1 << "\n";

    auto username3 = find_user(3)
        | fxt::match(
            [](const std::string& name) { return name; },
            []() { return std::string{"Guest"}; }
        );
    std::cout << "   User 3: " << username3 << "\n\n";

    // Example 9: Complex matching with calculation
    std::cout << "9. Complex calculation with match:\n";
    auto calc_result = safe_sqrt(16.0)
        | fxt::match(
            [](double value) {
                return "Square root is " + std::to_string(value);
            },
            [](const std::string& err) {
                return "Calculation failed: " + err;
            }
        );
    std::cout << "   sqrt(16) = " << calc_result << "\n";

    auto calc_result2 = safe_sqrt(-4.0)
        | fxt::match(
            [](double value) {
                return "Square root is " + std::to_string(value);
            },
            [](const std::string& err) {
                return "Calculation failed: " + err;
            }
        );
    std::cout << "   sqrt(-4) = " << calc_result2 << "\n\n";

    // Example 10: Using match without pipe operator
    std::cout << "10. Match without pipe operator:\n";
    auto direct_match = fxt::match(
        [](int value) { return value + 10; },
        [](const std::string&) { return 0; }
    )(safe_divide(30, 3));
    std::cout << "   (30 / 3) + 10 = " << direct_match << "\n\n";

    // Example 11: Match with bool return type
    std::cout << "11. Match returning bool:\n";
    auto is_valid = parse_int("123")
        | fxt::match(
            [](int) { return true; },
            []() { return false; }
        );
    std::cout << "   Is \"123\" a valid number? " << (is_valid ? "yes" : "no") << "\n";

    auto is_valid2 = parse_int("abc")
        | fxt::match(
            [](int) { return true; },
            []() { return false; }
        );
    std::cout << "   Is \"abc\" a valid number? " << (is_valid2 ? "yes" : "no") << "\n\n";

    // Example 12: Practical use case - user authentication
    std::cout << "12. Practical use case - user authentication:\n";
    auto authenticate = [](int user_id) -> fxt::expected<std::string, std::string> {
        if (user_id <= 0) {
            return fxt::unexpected<std::string>{"Invalid user ID"};
        }
        if (user_id > 100) {
            return fxt::unexpected<std::string>{"User not found"};
        }
        return "User_" + std::to_string(user_id);
    };

    auto auth_result1 = authenticate(42)
        | fxt::match(
            [](const std::string& username) {
                return "Welcome, " + username + "!";
            },
            [](const std::string& error) {
                return "Authentication failed: " + error;
            }
        );
    std::cout << "   User 42: " << auth_result1 << "\n";

    auto auth_result2 = authenticate(150)
        | fxt::match(
            [](const std::string& username) {
                return "Welcome, " + username + "!";
            },
            [](const std::string& error) {
                return "Authentication failed: " + error;
            }
        );
    std::cout << "   User 150: " << auth_result2 << "\n\n";

    // Example 13: Match in a pipeline with multiple operations
    std::cout << "13. Complex pipeline with match:\n";
    auto pipeline_result = parse_int("5")
        | fxt::transform([](int x) { return x * x; })
        | fxt::and_then([](int x) -> fxt::optional<int> {
            if (x > 20) return x;
            return fxt::nullopt;
        })
        | fxt::match(
            [](int value) { return "Result: " + std::to_string(value); },
            []() { return std::string{"Value too small"}; }
        );
    std::cout << "   5 -> square -> filter(>20): " << pipeline_result << "\n";

    auto pipeline_result2 = parse_int("3")
        | fxt::transform([](int x) { return x * x; })
        | fxt::and_then([](int x) -> fxt::optional<int> {
            if (x > 20) return x;
            return fxt::nullopt;
        })
        | fxt::match(
            [](int value) { return "Result: " + std::to_string(value); },
            []() { return std::string{"Value too small"}; }
        );
    std::cout << "   3 -> square -> filter(>20): " << pipeline_result2 << "\n\n";

    // Example 14: Match with mutable state
    std::cout << "14. Match with mutable state (counting operations):\n";
    int success_count = 0;
    int error_count = 0;

    for (int divisor : {2, 0, 4, 0, 5}) {
        safe_divide(100, divisor)
            | fxt::match(
                [&success_count](int) {
                    success_count++;
                    return 0;
                },
                [&error_count](const std::string&) {
                    error_count++;
                    return 0;
                }
            );
    }
    std::cout << "   Successes: " << success_count << ", Errors: " << error_count << "\n";

    return 0;
}

