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

fxt::expected<int, std::string> validate_positive(const int x) {
    if (x <= 0) {
        return fxt::unexpected<std::string>{"Value must be positive"};
    }
    return fxt::expected<int, std::string>{x};
}

int main() {
    std::cout << "=== OrElse Demonstration ===\n\n";

    // Demonstration with fxt::optional
    std::cout << "--- fxt::optional examples ---\n";

    // Example 1: Providing fallback for empty optional
    auto opt_result1 = parse_int("invalid") | fxt::or_else([]() -> fxt::optional<int> {
        return 0;  // Fallback to 0
    });
    std::cout << "parse_int(\"invalid\") | or_else(return 0) = ";
    if (opt_result1.has_value()) {
        std::cout << *opt_result1 << std::endl;
    } else {
        std::cout << "nullopt" << std::endl;
    }

    // Example 2: or_else not called when value exists
    auto opt_result2 = parse_int("42") | fxt::or_else([]() -> fxt::optional<int> {
        std::cout << "  [This should not be printed]\n";
        return 999;
    });
    std::cout << "parse_int(\"42\") | or_else(return 999) = " << *opt_result2 << std::endl;

    // Example 3: Chaining multiple or_else operations (fallback chain)
    auto try_parse_hex = []() -> fxt::optional<int> {
        std::cout << "  Trying hex parse... (returning nullopt)\n";
        return fxt::nullopt;
    };
    auto try_parse_oct = []() -> fxt::optional<int> {
        std::cout << "  Trying octal parse... (returning nullopt)\n";
        return fxt::nullopt;
    };
    auto use_default = []() -> fxt::optional<int> {
        std::cout << "  Using default value\n";
        return 100;
    };

    std::cout << "\nChaining fallback strategies:\n";
    auto opt_result3 = parse_int("xyz")
                     | fxt::or_else(try_parse_hex)
                     | fxt::or_else(try_parse_oct)
                     | fxt::or_else(use_default);
    std::cout << "Final result: " << *opt_result3 << std::endl;

    // Example 4: Conditional fallback
    std::cout << "\nConditional fallback based on context:\n";
    bool use_zero_as_default = true;
    auto opt_result4 = parse_int("bad_input")
                     | fxt::or_else([use_zero_as_default]() -> fxt::optional<int> {
        if (use_zero_as_default) {
            std::cout << "  Using zero as default\n";
            return 0;
        } else {
            std::cout << "  No default available\n";
            return fxt::nullopt;
        }
    });
    std::cout << "Result: " << (opt_result4.has_value() ? std::to_string(*opt_result4) : "nullopt") << std::endl;

    std::cout << "\n--- fxt::expected examples ---\n";

    // Example 5: Error recovery with or_else
    auto exp_result1 = safe_divide(10, 0) | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
        std::cout << "  Error occurred: " << err << "\n";
        std::cout << "  Recovering with default value\n";
        return 0;  // Recover from error
    });
    std::cout << "safe_divide(10, 0) | or_else(return 0) = " << *exp_result1 << std::endl;

    // Example 6: or_else not called on success
    auto exp_result2 = safe_divide(20, 4) | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
        std::cout << "  [This should not be printed]\n";
        return 999;
    });
    std::cout << "safe_divide(20, 4) | or_else(return 999) = " << *exp_result2 << std::endl;

    // Example 7: Error inspection and conditional recovery
    std::cout << "\nError inspection and recovery:\n";
    auto exp_result3 = safe_divide(15, 0) | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
        std::cout << "  Inspecting error: " << err << "\n";
        if (err == "Division by zero") {
            std::cout << "  Recovering with special value for division by zero\n";
            return -1;  // Special sentinel value
        }
        return fxt::unexpected<std::string>(err);  // Propagate other errors
    });
    std::cout << "Result: " << *exp_result3 << std::endl;

    // Example 8: Chaining multiple or_else for retry logic
    std::cout << "\nRetry logic with multiple or_else:\n";
    int retry_count = 0;
    auto exp_result4 = fxt::expected<int, std::string>{fxt::unexpected<std::string>("initial failure")}
                     | fxt::or_else([&retry_count](const std::string&) -> fxt::expected<int, std::string> {
                         ++retry_count;
                         std::cout << "  Retry attempt " << retry_count << " - still failing\n";
                         return fxt::unexpected<std::string>("retry 1 failed");
                     })
                     | fxt::or_else([&retry_count](const std::string&) -> fxt::expected<int, std::string> {
                         ++retry_count;
                         std::cout << "  Retry attempt " << retry_count << " - still failing\n";
                         return fxt::unexpected<std::string>("retry 2 failed");
                     })
                     | fxt::or_else([&retry_count](const std::string&) -> fxt::expected<int, std::string> {
                         ++retry_count;
                         std::cout << "  Retry attempt " << retry_count << " - success!\n";
                         return 42;  // Finally succeed
                     });
    std::cout << "Final result after " << retry_count << " retries: " << *exp_result4 << std::endl;

    // Example 9: Error type transformation
    std::cout << "\nError transformation (parsing error string as value):\n";
    auto exp_result5 = fxt::expected<int, std::string>{fxt::unexpected<std::string>("404")}
                     | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
        std::cout << "  Attempting to parse error as number: " << err << "\n";
        try {
            int error_code = std::stoi(err);
            std::cout << "  Success! Using error code as value\n";
            return error_code;
        } catch (...) {
            std::cout << "  Parse failed, propagating error\n";
            return fxt::unexpected<std::string>("Cannot parse error code");
        }
    });
    std::cout << "Result: " << *exp_result5 << std::endl;

    std::cout << "\n--- Mixed operations examples ---\n";

    // Example 10: Combining or_else with and_then and transform
    std::cout << "Complex pipeline with error recovery:\n";
    auto pipeline_result = parse_int("not_a_number")
                         | fxt::or_else([]() -> fxt::optional<int> {
                             std::cout << "  Parse failed, using default value 10\n";
                             return 10;
                         })
                         | fxt::transform([](const int x) {
                             std::cout << "  Transforming value: " << x << " -> " << (x * 2) << "\n";
                             return x * 2;
                         })
                         | fxt::and_then([](const int x) -> fxt::optional<int> {
                             std::cout << "  Validating value: " << x << "\n";
                             if (x >= 15) {
                                 std::cout << "  Validation passed\n";
                                 return x;
                             } else {
                                 std::cout << "  Validation failed\n";
                                 return fxt::nullopt;
                             }
                         })
                         | fxt::or_else([]() -> fxt::optional<int> {
                             std::cout << "  Validation failed, using fallback value\n";
                             return 100;
                         });
    std::cout << "Final result: " << *pipeline_result << std::endl;

    // Example 11: Reusable error handlers
    std::cout << "\nReusable error handlers:\n";
    auto default_to_zero = fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
        std::cout << "  Error: " << err << " - using default value 0\n";
        return 0;
    });

    auto result_a = safe_divide(100, 0) | default_to_zero;
    auto result_b = validate_positive(-5) | default_to_zero;

    std::cout << "Division result with recovery: " << *result_a << std::endl;
    std::cout << "Validation result with recovery: " << *result_b << std::endl;

    // Example 12: Lazy evaluation demonstration
    std::cout << "\nLazy evaluation (expensive fallback only computed when needed):\n";
    bool expensive_called = false;
    auto expensive_fallback = [&expensive_called]() -> fxt::optional<int> {
        expensive_called = true;
        std::cout << "  Computing expensive fallback...\n";
        return 999;
    };

    std::cout << "With value present:\n";
    auto lazy_result1 = parse_int("5") | fxt::or_else(expensive_fallback);
    std::cout << "  Result: " << *lazy_result1 << ", Expensive called: " << (expensive_called ? "yes" : "no") << "\n";

    expensive_called = false;  // Reset
    std::cout << "Without value:\n";
    auto lazy_result2 = parse_int("invalid") | fxt::or_else(expensive_fallback);
    std::cout << "  Result: " << *lazy_result2 << ", Expensive called: " << (expensive_called ? "yes" : "no") << "\n";

    // Example 13: Real-world scenario - configuration with fallbacks
    std::cout << "\n--- Real-world example: Configuration loading with fallbacks ---\n";

    auto load_from_config = [](const std::string& key) -> fxt::optional<int> {
        std::cout << "  Attempting to load '" << key << "' from config file... (not found)\n";
        return fxt::nullopt;
    };

    auto load_from_env = [](const std::string& key) -> fxt::optional<int> {
        std::cout << "  Attempting to load '" << key << "' from environment... (not found)\n";
        return fxt::nullopt;
    };

    auto use_hardcoded_default = [](const std::string& key, int default_val) {
        return [key, default_val]() -> fxt::optional<int> {
            std::cout << "  Using hardcoded default for '" << key << "': " << default_val << "\n";
            return default_val;
        };
    };

    std::string setting_name = "max_connections";
    auto setting_value = load_from_config(setting_name)
                       | fxt::or_else([&]() { return load_from_env(setting_name); })
                       | fxt::or_else(use_hardcoded_default(setting_name, 100));

    std::cout << "Final setting value: " << *setting_value << std::endl;

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}

