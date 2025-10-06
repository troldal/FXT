//
// Demo: fxt::to_optional
// This demonstrates how to use fxt::to_optional to convert fxt::expected to fxt::optional
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// Helper function to simulate a division operation that can fail
auto safe_divide(double numerator, double denominator) -> fxt::expected<double, std::string>
{
    if (denominator == 0.0)
    {
        return fxt::unexpected("Division by zero");
    }
    return numerator / denominator;
}

// Helper function to simulate parsing an integer
auto parse_int(const std::string& str) -> fxt::expected<int, std::string>
{
    if (str.empty())
    {
        return fxt::unexpected("Empty string");
    }
    if (!std::isdigit(str[0]) && str[0] != '-')
    {
        return fxt::unexpected("Invalid format");
    }
    try
    {
        return std::stoi(str);
    }
    catch (...)
    {
        return fxt::unexpected("Out of range");
    }
}

// Helper function to simulate a database lookup
auto find_user_by_id(int id) -> fxt::expected<std::string, int>
{
    if (id == 1)
    {
        return "Alice";
    }
    else if (id == 2)
    {
        return "Bob";
    }
    else
    {
        return fxt::unexpected(404);  // Not found
    }
}

int main()
{
    std::cout << "=== fxt::to_optional Demo ===\n\n";

    // Example 1: Convert successful expected to optional
    std::cout << "1. Convert successful expected to optional:\n";
    {
        auto result = safe_divide(10.0, 2.0) | fxt::to_optional();

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   No value\n";
        }
    }

    // Example 2: Convert failed expected to empty optional
    std::cout << "\n2. Convert failed expected to empty optional:\n";
    {
        auto result = safe_divide(10.0, 0.0) | fxt::to_optional();

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   No value (error was discarded)\n";
        }
    }

    // Example 3: Use with value_or after conversion
    std::cout << "\n3. Combine to_optional with value_or:\n";
    {
        auto result1 = safe_divide(10.0, 2.0) | fxt::to_optional() | fxt::value_or(0.0);
        auto result2 = safe_divide(10.0, 0.0) | fxt::to_optional() | fxt::value_or(0.0);

        std::cout << "   Success case: " << result1 << "\n";
        std::cout << "   Error case (using default): " << result2 << "\n";
    }

    // Example 4: Chain multiple operations
    std::cout << "\n4. Chain multiple operations:\n";
    {
        auto result = parse_int("42")
                    | fxt::to_optional()
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::transform([](int x) { return "Result: " + std::to_string(x); });

        if (result.has_value())
        {
            std::cout << "   " << result.value() << "\n";
        }
        else
        {
            std::cout << "   No value\n";
        }
    }

    // Example 5: Convert different error types
    std::cout << "\n5. Convert expected with different error types:\n";
    {
        auto string_error = parse_int("abc") | fxt::to_optional();
        auto int_error = find_user_by_id(999) | fxt::to_optional();

        std::cout << "   String error result: "
                  << (string_error.has_value() ? string_error.value_or(0) : 0) << "\n";
        std::cout << "   Int error result: "
                  << (int_error.has_value() ? int_error.value() : "(none)") << "\n";
    }

    // Example 6: Use in conditional logic
    std::cout << "\n6. Use in conditional logic:\n";
    {
        auto maybe_value = safe_divide(100.0, 5.0) | fxt::to_optional();

        if (auto val = maybe_value)
        {
            std::cout << "   Got value: " << *val << "\n";
            if (*val > 15.0)
            {
                std::cout << "   Value is greater than 15\n";
            }
        }
        else
        {
            std::cout << "   No value available\n";
        }
    }

    // Example 7: When you don't care about error details
    std::cout << "\n7. Simplify error handling when details don't matter:\n";
    {
        auto process = [](const std::string& input) {
            auto opt_value = parse_int(input) | fxt::to_optional();

            // We don't care what the error was, just whether it succeeded
            if (!opt_value.has_value())
            {
                return "Invalid input";
            }

            return opt_value.value() > 0 ? "Positive" : "Non-positive";
        };

        std::cout << "   process(\"42\"): " << process("42") << "\n";
        std::cout << "   process(\"-5\"): " << process("-5") << "\n";
        std::cout << "   process(\"abc\"): " << process("abc") << "\n";
    }

    // Example 8: Convert to optional for optional-based APIs
    std::cout << "\n8. Bridge between expected and optional APIs:\n";
    {
        // Suppose we have an API that expects optional
        auto process_optional = [](const fxt::optional<std::string>& opt) {
            return opt.has_value() ? "User: " + opt.value() : "Anonymous";
        };

        // We can convert our expected results to optional
        auto user1 = find_user_by_id(1) | fxt::to_optional();
        auto user2 = find_user_by_id(999) | fxt::to_optional();

        std::cout << "   " << process_optional(user1) << "\n";
        std::cout << "   " << process_optional(user2) << "\n";
    }

    // Example 9: Multiple conversions in a pipeline
    std::cout << "\n9. Complex pipeline with to_optional:\n";
    {
        auto result = parse_int("10")
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::and_then([](int x) -> fxt::expected<double, std::string> {
                        return safe_divide(100.0, static_cast<double>(x));
                    })
                    | fxt::to_optional()
                    | fxt::transform([](double x) { return static_cast<int>(x); });

        std::cout << "   Final result: "
                  << (result.has_value() ? std::to_string(result.value()) : "none")
                  << "\n";
    }

    // Example 10: Compare expected vs optional handling
    std::cout << "\n10. Compare error handling approaches:\n";
    {
        auto exp_result = safe_divide(10.0, 0.0);
        auto opt_result = safe_divide(10.0, 0.0) | fxt::to_optional();

        std::cout << "   Expected approach:\n";
        if (exp_result.has_value())
        {
            std::cout << "      Value: " << exp_result.value() << "\n";
        }
        else
        {
            std::cout << "      Error: " << exp_result.error() << "\n";
        }

        std::cout << "   Optional approach (error details lost):\n";
        if (opt_result.has_value())
        {
            std::cout << "      Value: " << opt_result.value() << "\n";
        }
        else
        {
            std::cout << "      No value (we don't know why)\n";
        }
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}

