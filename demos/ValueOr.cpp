//
// Created by kenne on 05/10/2025.
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

fxt::expected<double, std::string> safe_sqrt(const double x) {
    if (x < 0) {
        return fxt::unexpected<std::string>{"Cannot take square root of negative number"};
    }
    return fxt::expected<double, std::string>{std::sqrt(x)};
}

int main() {
    std::cout << "=== ValueOr Demonstration ===\n\n";

    // Demonstration with fxt::optional
    std::cout << "--- fxt::optional examples ---\n";

    // Example 1: Basic value_or with successful optional
    auto opt_result1 = parse_int("42") | fxt::value_or(0);
    std::cout << "parse_int(\"42\") | value_or(0) = " << opt_result1 << std::endl;

    // Example 2: value_or providing fallback for empty optional
    auto opt_result2 = parse_int("invalid") | fxt::value_or(0);
    std::cout << "parse_int(\"invalid\") | value_or(0) = " << opt_result2 << std::endl;

    // Example 3: value_or with different default values
    std::cout << "\nDifferent default values:\n";
    auto opt_result3a = parse_int("abc") | fxt::value_or(-1);
    auto opt_result3b = parse_int("xyz") | fxt::value_or(999);
    std::cout << "  parse_int(\"abc\") | value_or(-1) = " << opt_result3a << std::endl;
    std::cout << "  parse_int(\"xyz\") | value_or(999) = " << opt_result3b << std::endl;

    // Example 4: value_or in a pipeline with transform
    std::cout << "\nPipeline with transform:\n";
    auto opt_result4 = parse_int("10")
                     | fxt::transform([](const int x) {
                         std::cout << "  Transforming: " << x << " * 2 = " << (x * 2) << "\n";
                         return x * 2;
                       })
                     | fxt::value_or(0);
    std::cout << "Result: " << opt_result4 << std::endl;

    // Example 5: value_or when transform produces empty
    std::cout << "\nPipeline with empty result:\n";
    auto opt_result5 = parse_int("not_a_number")
                     | fxt::transform([](const int x) {
                         std::cout << "  [This won't print]\n";
                         return x * 2;
                       })
                     | fxt::value_or(0);
    std::cout << "Result: " << opt_result5 << std::endl;

    // Example 6: value_or with and_then
    std::cout << "\nCombining with and_then:\n";
    auto opt_result6 = parse_int("5")
                     | fxt::and_then([](const int x) -> fxt::optional<int> {
                         std::cout << "  Validating: " << x << " > 10? ";
                         if (x > 10) {
                             std::cout << "Yes\n";
                             return x;
                         } else {
                             std::cout << "No - returning nullopt\n";
                             return fxt::nullopt;
                         }
                       })
                     | fxt::value_or(100);
    std::cout << "Result (fallback used): " << opt_result6 << std::endl;

    auto opt_result6b = parse_int("15")
                      | fxt::and_then([](const int x) -> fxt::optional<int> {
                          std::cout << "  Validating: " << x << " > 10? ";
                          if (x > 10) {
                              std::cout << "Yes\n";
                              return x;
                          } else {
                              std::cout << "No - returning nullopt\n";
                              return fxt::nullopt;
                          }
                        })
                      | fxt::value_or(100);
    std::cout << "Result (value preserved): " << opt_result6b << std::endl;

    // Example 7: value_or with string types
    std::cout << "\nWith string types:\n";
    fxt::optional<std::string> name = "Alice";
    fxt::optional<std::string> no_name = fxt::nullopt;

    auto name_result1 = name | fxt::value_or(std::string("Unknown"));
    auto name_result2 = no_name | fxt::value_or(std::string("Unknown"));

    std::cout << "Name: " << name_result1 << std::endl;
    std::cout << "No name: " << name_result2 << std::endl;

    std::cout << "\n--- fxt::expected examples ---\n";

    // Example 8: Basic value_or with successful expected
    auto exp_result1 = safe_divide(20, 4) | fxt::value_or(0);
    std::cout << "safe_divide(20, 4) | value_or(0) = " << exp_result1 << std::endl;

    // Example 9: value_or providing fallback for error expected
    auto exp_result2 = safe_divide(20, 0) | fxt::value_or(0);
    std::cout << "safe_divide(20, 0) | value_or(0) = " << exp_result2 << std::endl;

    // Example 10: value_or with different default values
    std::cout << "\nDifferent default values for errors:\n";
    auto exp_result3a = safe_divide(10, 0) | fxt::value_or(-1);
    auto exp_result3b = safe_divide(10, 0) | fxt::value_or(999);
    std::cout << "  safe_divide(10, 0) | value_or(-1) = " << exp_result3a << std::endl;
    std::cout << "  safe_divide(10, 0) | value_or(999) = " << exp_result3b << std::endl;

    // Example 11: value_or in a pipeline with transform
    std::cout << "\nPipeline with transform:\n";
    auto exp_result4 = safe_divide(100, 5)
                     | fxt::transform([](const int x) {
                         std::cout << "  Transforming: " << x << " + 10 = " << (x + 10) << "\n";
                         return x + 10;
                       })
                     | fxt::value_or(0);
    std::cout << "Result: " << exp_result4 << std::endl;

    // Example 12: value_or when pipeline has error
    std::cout << "\nPipeline with error:\n";
    auto exp_result5 = safe_divide(100, 0)
                     | fxt::transform([](const int x) {
                         std::cout << "  [This won't print]\n";
                         return x + 10;
                       })
                     | fxt::value_or(0);
    std::cout << "Result (fallback used): " << exp_result5 << std::endl;

    // Example 13: value_or with and_then
    std::cout << "\nCombining with and_then:\n";
    auto exp_result6 = safe_divide(100, 5)
                     | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                         std::cout << "  Validating: " << x << " > 10? ";
                         if (x > 10) {
                             std::cout << "Yes\n";
                             return x;
                         } else {
                             std::cout << "No - returning error\n";
                             return fxt::unexpected<std::string>("Value too small");
                         }
                       })
                     | fxt::value_or(100);
    std::cout << "Result (value preserved): " << exp_result6 << std::endl;

    auto exp_result6b = safe_divide(30, 5)
                      | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                          std::cout << "  Validating: " << x << " > 10? ";
                          if (x > 10) {
                              std::cout << "Yes\n";
                              return x;
                          } else {
                              std::cout << "No - returning error\n";
                              return fxt::unexpected<std::string>("Value too small");
                          }
                        })
                      | fxt::value_or(100);
    std::cout << "Result (fallback used): " << exp_result6b << std::endl;

    // Example 14: value_or with or_else
    std::cout << "\nCombining with or_else:\n";
    auto exp_result7 = safe_divide(50, 0)
                     | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
                         std::cout << "  or_else: Error '" << err << "' - still returning error\n";
                         return fxt::unexpected<std::string>("Still error");
                       })
                     | fxt::value_or(42);
    std::cout << "Result (value_or as final fallback): " << exp_result7 << std::endl;

    std::cout << "\n--- Mixed operations examples ---\n";

    // Example 15: Reusable value_or operation
    std::cout << "Reusable value_or:\n";
    auto default_to_zero = fxt::value_or(0);

    auto reuse_result1 = parse_int("123") | default_to_zero;
    auto reuse_result2 = parse_int("invalid") | default_to_zero;
    auto reuse_result3 = safe_divide(100, 5) | default_to_zero;
    auto reuse_result4 = safe_divide(100, 0) | default_to_zero;

    std::cout << "  parse_int(\"123\") | default_to_zero = " << reuse_result1 << "\n";
    std::cout << "  parse_int(\"invalid\") | default_to_zero = " << reuse_result2 << "\n";
    std::cout << "  safe_divide(100, 5) | default_to_zero = " << reuse_result3 << "\n";
    std::cout << "  safe_divide(100, 0) | default_to_zero = " << reuse_result4 << "\n";

    // Example 16: Complex pipeline demonstrating value extraction
    std::cout << "\nComplex pipeline (note: value_or extracts the value):\n";
    int computed_result = parse_int("7")
                        | fxt::transform([](const int x) {
                            std::cout << "  Step 1: " << x << " * 3 = " << (x * 3) << "\n";
                            return x * 3;
                          })
                        | fxt::and_then([](const int x) -> fxt::optional<int> {
                            std::cout << "  Step 2: Checking if " << x << " is even\n";
                            if (x % 2 == 0) return x;
                            std::cout << "  Step 2: Not even, returning nullopt\n";
                            return fxt::nullopt;
                          })
                        | fxt::value_or(100);

    // computed_result is int, not optional<int>
    computed_result = computed_result + 50;
    std::cout << "After adding 50: " << computed_result << "\n";

    // Example 17: Real-world scenario - Configuration with defaults
    std::cout << "\n--- Real-world example: Configuration values ---\n";

    struct Config {
        std::string host;
        int port;
        int timeout;
        int max_connections;
    };

    auto get_config_value = [](const std::string& key) -> fxt::optional<int> {
        std::cout << "  Looking up '" << key << "' in config... ";
        // Simulate: only "port" is in config
        if (key == "port") {
            std::cout << "Found: 8080\n";
            return 8080;
        }
        std::cout << "Not found\n";
        return fxt::nullopt;
    };

    Config config;
    config.host = "localhost";
    config.port = get_config_value("port") | fxt::value_or(3000);
    config.timeout = get_config_value("timeout") | fxt::value_or(30);
    config.max_connections = get_config_value("max_connections") | fxt::value_or(100);

    std::cout << "\nFinal configuration:\n";
    std::cout << "  Host: " << config.host << "\n";
    std::cout << "  Port: " << config.port << "\n";
    std::cout << "  Timeout: " << config.timeout << " seconds\n";
    std::cout << "  Max connections: " << config.max_connections << "\n";

    // Example 18: Mathematical operations with error handling
    std::cout << "\n--- Real-world example: Mathematical calculations ---\n";

    auto calculate_formula = [](double a, double b) {
        std::cout << "Calculating: sqrt(a) / b where a=" << a << ", b=" << b << "\n";
        return safe_sqrt(a)
             | fxt::and_then([b](const double x) { return safe_divide(static_cast<int>(x), static_cast<int>(b)); })
             | fxt::transform([](const int x) { return x * 100; })
             | fxt::value_or(-1);
    };

    std::cout << "\nCase 1: Valid inputs (a=16, b=2)\n";
    auto math_result1 = calculate_formula(16.0, 2.0);
    std::cout << "Result: " << math_result1 << "\n";

    std::cout << "\nCase 2: Negative sqrt (a=-16, b=2)\n";
    auto math_result2 = calculate_formula(-16.0, 2.0);
    std::cout << "Result: " << math_result2 << " (error sentinel)\n";

    std::cout << "\nCase 3: Division by zero (a=16, b=0)\n";
    auto math_result3 = calculate_formula(16.0, 0.0);
    std::cout << "Result: " << math_result3 << " (error sentinel)\n";

    // Example 19: Comparison - with and without value_or
    std::cout << "\n--- Comparison: Traditional vs value_or ---\n";

    std::cout << "Traditional approach:\n";
    auto opt_traditional = parse_int("invalid");
    int traditional_result;
    if (opt_traditional.has_value()) {
        traditional_result = *opt_traditional;
        std::cout << "  Got value: " << traditional_result << "\n";
    } else {
        traditional_result = 0;
        std::cout << "  Using default: " << traditional_result << "\n";
    }

    std::cout << "With value_or:\n";
    int value_or_result = parse_int("invalid") | fxt::value_or(0);
    std::cout << "  Result: " << value_or_result << "\n";
    std::cout << "  (Much more concise!)\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}

