//
// Demo: fxt::tap
// This demonstrates how to use fxt::tap to inject side effects into pipelines
// without changing the values flowing through them
//

#include <fxt.hpp>
#include <iostream>
#include <string>
#include <vector>

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

// Helper function to simulate finding a user
auto find_user(int id) -> fxt::optional<std::string>
{
    if (id == 1) return "Alice";
    if (id == 2) return "Bob";
    if (id == 3) return "Charlie";
    return fxt::nullopt;
}

int main()
{
    std::cout << "=== fxt::tap Demo ===\n\n";

    // Example 1: Basic tap with expected (success case)
    std::cout << "1. Basic tap with expected (success case):\n";
    {
        auto result = safe_divide(10.0, 2.0)
                    | fxt::tap([](double x) {
                        std::cout << "   [TAP] Division result: " << x << "\n";
                    })
                    | fxt::transform([](double x) { return x * 2; });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 2: Tap with expected (error case - tap is not executed)
    std::cout << "\n2. Tap with expected (error case):\n";
    {
        auto result = safe_divide(10.0, 0.0)
                    | fxt::tap([](double x) {
                        std::cout << "   [TAP] This won't print\n";
                    })
                    | fxt::transform([](double x) { return x * 2; });

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 3: Multiple taps in a pipeline
    std::cout << "\n3. Multiple taps in a pipeline:\n";
    {
        auto result = parse_int("42")
                    | fxt::tap([](int x) {
                        std::cout << "   [TAP 1] Parsed value: " << x << "\n";
                    })
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::tap([](int x) {
                        std::cout << "   [TAP 2] After doubling: " << x << "\n";
                    })
                    | fxt::transform([](int x) { return x + 10; })
                    | fxt::tap([](int x) {
                        std::cout << "   [TAP 3] After adding 10: " << x << "\n";
                    });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 4: Tap with optional (has value)
    std::cout << "\n4. Tap with optional (has value):\n";
    {
        auto result = find_user(1)
                    | fxt::tap([](const std::string& name) {
                        std::cout << "   [TAP] Found user: " << name << "\n";
                    })
                    | fxt::transform([](const std::string& name) {
                        return "Hello, " + name + "!";
                    });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 5: Tap with optional (empty - tap is not executed)
    std::cout << "\n5. Tap with optional (empty):\n";
    {
        auto result = find_user(999)
                    | fxt::tap([](const std::string& name) {
                        std::cout << "   [TAP] This won't print\n";
                    })
                    | fxt::transform([](const std::string& name) {
                        return "Hello, " + name + "!";
                    });

        if (result.has_value())
        {
            std::cout << "   Result: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   No user found\n";
        }
    }

    // Example 6: Using tap for debugging a complex pipeline
    std::cout << "\n6. Debugging a complex pipeline with tap:\n";
    {
        auto result = parse_int("100")
                    | fxt::tap([](int x) {
                        std::cout << "   [DEBUG] Input: " << x << "\n";
                    })
                    | fxt::transform([](int x) { return x / 4; })
                    | fxt::tap([](int x) {
                        std::cout << "   [DEBUG] After division: " << x << "\n";
                    })
                    | fxt::and_then([](int x) -> fxt::expected<double, std::string> {
                        return safe_divide(1000.0, static_cast<double>(x));
                    })
                    | fxt::tap([](double x) {
                        std::cout << "   [DEBUG] After safe_divide: " << x << "\n";
                    });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 7: Tap with side effects (logging, counting, etc.)
    std::cout << "\n7. Tap with side effects (counting operations):\n";
    {
        int operation_count = 0;

        auto result = fxt::expected<int, std::string>{10}
                    | fxt::tap([&operation_count](int x) {
                        operation_count++;
                        std::cout << "   [LOG] Operation " << operation_count << ": value = " << x << "\n";
                    })
                    | fxt::transform([](int x) { return x * 3; })
                    | fxt::tap([&operation_count](int x) {
                        operation_count++;
                        std::cout << "   [LOG] Operation " << operation_count << ": value = " << x << "\n";
                    })
                    | fxt::transform([](int x) { return x + 5; })
                    | fxt::tap([&operation_count](int x) {
                        operation_count++;
                        std::cout << "   [LOG] Operation " << operation_count << ": value = " << x << "\n";
                    });

        std::cout << "   Total operations logged: " << operation_count << "\n";
        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 8: Tap for validation/assertions
    std::cout << "\n8. Tap for validation in pipeline:\n";
    {
        auto result = parse_int("50")
                    | fxt::tap([](int x) {
                        if (x < 0 || x > 100)
                        {
                            std::cout << "   [WARN] Value out of expected range: " << x << "\n";
                        }
                        else
                        {
                            std::cout << "   [OK] Value in valid range: " << x << "\n";
                        }
                    })
                    | fxt::transform([](int x) { return x * 2; });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 9: Tap with different data types
    std::cout << "\n9. Tap with different data types:\n";
    {
        struct Point {
            double x, y;
        };

        auto result = fxt::expected<Point, std::string>{Point{3.0, 4.0}}
                    | fxt::tap([](const Point& p) {
                        std::cout << "   [TAP] Point: (" << p.x << ", " << p.y << ")\n";
                    })
                    | fxt::transform([](const Point& p) {
                        return Point{p.x * 2, p.y * 2};
                    })
                    | fxt::tap([](const Point& p) {
                        std::cout << "   [TAP] Scaled point: (" << p.x << ", " << p.y << ")\n";
                    });

        std::cout << "   Final point: (" << result.value().x << ", " << result.value().y << ")\n";
    }

    // Example 10: Combining tap with or_else
    std::cout << "\n10. Combining tap with or_else:\n";
    {
        auto process = [](const std::string& input) {
            return parse_int(input)
                 | fxt::tap([&input](int x) {
                     std::cout << "   [SUCCESS] Parsed '" << input << "' as: " << x << "\n";
                 })
                 | fxt::or_else([&input](const std::string& err) {
                     std::cout << "   [ERROR] Failed to parse '" << input << "': " << err << "\n";
                     return fxt::expected<int, std::string>{0};
                 });
        };

        auto result1 = process("123");
        auto result2 = process("abc");

        std::cout << "   Result 1: " << result1.value() << "\n";
        std::cout << "   Result 2: " << result2.value() << "\n";
    }

    // Example 11: Tap for collecting intermediate values
    std::cout << "\n11. Collecting intermediate values with tap:\n";
    {
        std::vector<int> trace;

        auto result = fxt::expected<int, std::string>{5}
                    | fxt::tap([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x * 2; })
                    | fxt::tap([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x + 3; })
                    | fxt::tap([&trace](int x) { trace.push_back(x); })
                    | fxt::transform([](int x) { return x * x; })
                    | fxt::tap([&trace](int x) { trace.push_back(x); });

        std::cout << "   Trace: ";
        for (size_t i = 0; i < trace.size(); ++i)
        {
            std::cout << trace[i];
            if (i < trace.size() - 1) std::cout << " -> ";
        }
        std::cout << "\n   Final result: " << result.value() << "\n";
    }

    // Example 12: Tap with optional in a chain
    std::cout << "\n12. Tap with optional in a transformation chain:\n";
    {
        auto result = find_user(2)
                    | fxt::tap([](const std::string& name) {
                        std::cout << "   [LOOKUP] User found: " << name << "\n";
                    })
                    | fxt::to_expected<std::string>("User not found")
                    | fxt::tap([](const std::string& name) {
                        std::cout << "   [CONVERT] Converted to expected: " << name << "\n";
                    })
                    | fxt::transform([](const std::string& name) {
                        return name + " (verified)";
                    });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 13: Tap doesn't change the value
    std::cout << "\n13. Verify tap doesn't change the value:\n";
    {
        auto result = fxt::expected<int, std::string>{42}
                    | fxt::tap([](int x) {
                        std::cout << "   [TAP] Value before: " << x << "\n";
                        // Even if we try to modify x here, it won't affect the pipeline
                        int modified = x * 100;
                        std::cout << "   [TAP] Modified local copy: " << modified << "\n";
                    });

        std::cout << "   Actual value in result: " << result.value() << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}

