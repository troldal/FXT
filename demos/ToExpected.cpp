//
// Demo: fxt::to_expected
// This demonstrates how to use fxt::to_expected to convert fxt::optional to fxt::expected
//

#include <fxt.hpp>
#include <iostream>
#include <string>

// Helper function to simulate finding a user by ID
auto find_user(int id) -> fxt::optional<std::string>
{
    if (id == 1) return "Alice";
    if (id == 2) return "Bob";
    if (id == 3) return "Charlie";
    return fxt::nullopt;
}

// Helper function to simulate parsing a configuration value
auto get_config(const std::string& key) -> fxt::optional<int>
{
    if (key == "port") return 8080;
    if (key == "timeout") return 30;
    if (key == "retries") return 3;
    return fxt::nullopt;
}

// Helper function to simulate looking up a value
auto lookup(const std::string& key) -> fxt::optional<double>
{
    if (key == "pi") return 3.14159;
    if (key == "e") return 2.71828;
    return fxt::nullopt;
}

int main()
{
    std::cout << "=== fxt::to_expected Demo ===\n\n";

    // Example 1: Convert optional with value to expected
    std::cout << "1. Convert successful optional to expected:\n";
    {
        auto result = find_user(1) | fxt::to_expected<std::string>("User not found");

        if (result.has_value())
        {
            std::cout << "   Found user: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 2: Convert empty optional to expected with error
    std::cout << "\n2. Convert empty optional to expected with error:\n";
    {
        auto result = find_user(999) | fxt::to_expected<std::string>("User not found");

        if (result.has_value())
        {
            std::cout << "   Found user: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 3: Convert to expected with integer error code
    std::cout << "\n3. Convert to expected with error code:\n";
    {
        auto result = get_config("invalid_key") | fxt::to_expected<int>(404);

        if (result.has_value())
        {
            std::cout << "   Config value: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error code: " << result.error() << "\n";
        }
    }

    // Example 4: Chain operations after conversion
    std::cout << "\n4. Chain operations after to_expected:\n";
    {
        auto result = get_config("port")
                    | fxt::to_expected<std::string>("Config not found")
                    | fxt::transform([](int port) {
                        return "Server running on port " + std::to_string(port);
                    });

        if (result.has_value())
        {
            std::cout << "   " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 5: Use with or_else after conversion
    std::cout << "\n5. Combine to_expected with or_else:\n";
    {
        auto result = find_user(999)
                    | fxt::to_expected<std::string>("User not found")
                    | fxt::or_else([](const std::string& err) {
                        std::cout << "   Handling error: " << err << "\n";
                        return fxt::expected<std::string, std::string>{"Guest"};
                    });

        std::cout << "   Final result: " << result.value() << "\n";
    }

    // Example 6: Different error types for different contexts
    std::cout << "\n6. Use different error types:\n";
    {
        struct ErrorInfo {
            int code;
            std::string message;
        };

        auto result = lookup("unknown")
                    | fxt::to_expected<ErrorInfo>(ErrorInfo{404, "Key not found"});

        if (result.has_value())
        {
            std::cout << "   Value: " << result.value() << "\n";
        }
        else
        {
            const auto& err = result.error();
            std::cout << "   Error [" << err.code << "]: " << err.message << "\n";
        }
    }

    // Example 7: Chain multiple conversions
    std::cout << "\n7. Complex pipeline with to_expected:\n";
    {
        auto result = get_config("timeout")
                    | fxt::to_expected<std::string>("Timeout not configured")
                    | fxt::transform([](int timeout) { return timeout * 1000; })
                    | fxt::transform([](int ms) { return static_cast<double>(ms) / 1000.0; })
                    | fxt::to_optional()
                    | fxt::to_expected<int>(-1);

        if (result.has_value())
        {
            std::cout << "   Timeout in seconds: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error code: " << result.error() << "\n";
        }
    }

    // Example 8: Use with and_then
    std::cout << "\n8. Combine to_expected with and_then:\n";
    {
        auto result = find_user(1)
                    | fxt::to_expected<std::string>("User not found")
                    | fxt::and_then([](const std::string& name) -> fxt::expected<int, std::string> {
                        if (name == "Alice") return 100;
                        if (name == "Bob") return 200;
                        return fxt::unexpected("Unknown user score");
                    });

        if (result.has_value())
        {
            std::cout << "   User score: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error: " << result.error() << "\n";
        }
    }

    // Example 9: Provide meaningful error messages
    std::cout << "\n9. Convert with context-specific error messages:\n";
    {
        auto port_result = get_config("port")
                         | fxt::to_expected<std::string>("Port configuration missing");
        auto invalid_result = get_config("invalid")
                            | fxt::to_expected<std::string>("Configuration key not found");

        std::cout << "   Port: "
                  << (port_result.has_value() ? std::to_string(port_result.value()) : port_result.error())
                  << "\n";
        std::cout << "   Invalid: "
                  << (invalid_result.has_value() ? std::to_string(invalid_result.value()) : invalid_result.error())
                  << "\n";
    }

    // Example 10: Bridge optional and expected APIs
    std::cout << "\n10. Bridge between optional and expected APIs:\n";
    {
        // Suppose we have an API that returns optional
        auto optional_api = []() -> fxt::optional<std::string> {
            return "data";
        };

        // We can convert it to expected for richer error handling
        auto result = optional_api() | fxt::to_expected<int>(500);

        if (result.has_value())
        {
            std::cout << "   Success: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   Error code: " << result.error() << "\n";
        }
    }

    // Example 11: Handle multiple optional values
    std::cout << "\n11. Process multiple optional values:\n";
    {
        auto user1 = find_user(1) | fxt::to_expected<int>(404);
        auto user2 = find_user(2) | fxt::to_expected<int>(404);
        auto user3 = find_user(999) | fxt::to_expected<int>(404);

        std::cout << "   User 1: "
                  << (user1.has_value() ? user1.value() : "Error " + std::to_string(user1.error()))
                  << "\n";
        std::cout << "   User 2: "
                  << (user2.has_value() ? user2.value() : "Error " + std::to_string(user2.error()))
                  << "\n";
        std::cout << "   User 3: "
                  << (user3.has_value() ? user3.value() : "Error " + std::to_string(user3.error()))
                  << "\n";
    }

    // Example 12: Transform error after conversion
    std::cout << "\n12. Transform error after to_expected:\n";
    {
        auto result = lookup("unknown")
                    | fxt::to_expected<std::string>("Not found")
                    | fxt::transform_error([](const std::string& err) {
                        return "[LOOKUP_ERROR] " + err;
                    });

        if (result.has_value())
        {
            std::cout << "   Value: " << result.value() << "\n";
        }
        else
        {
            std::cout << "   " << result.error() << "\n";
        }
    }

    // Example 13: Value_or on optional vs or_else on expected
    std::cout << "\n13. Compare optional value_or vs expected or_else:\n";
    {
        std::cout << "   Optional approach:\n";
        auto opt_result = find_user(999) | fxt::value_or("Unknown");
        std::cout << "      Result: " << opt_result << "\n";

        std::cout << "   Expected approach:\n";
        auto exp_result = find_user(999)
                        | fxt::to_expected<std::string>("User not found")
                        | fxt::or_else([](const std::string& err) {
                            return fxt::expected<std::string, std::string>{"Unknown"};
                        });
        std::cout << "      Result: " << exp_result.value() << "\n";
    }

    // -------------------------------------------------------------------------
    // Factory overload: fxt::to_expected(factory)
    // The factory is a no-argument callable whose return type becomes the error
    // type. It is invoked only when the optional is empty, so no error object
    // is constructed on the success path.
    // -------------------------------------------------------------------------

    // Example 14: Factory not called when optional has a value
    std::cout << "\n14. Factory overload — factory is NOT called on the success path:\n";
    {
        bool factory_called = false;
        auto result = find_user(1)
                    | fxt::to_expected([&]{
                          factory_called = true;
                          return std::string{"User not found"};
                      });

        std::cout << "   Found user: " << result.value() << "\n";
        std::cout << "   Factory called: " << (factory_called ? "yes" : "no") << "\n";
    }

    // Example 15: Factory called exactly once when optional is empty
    std::cout << "\n15. Factory overload — factory IS called on the empty path:\n";
    {
        int call_count = 0;
        auto result = find_user(999)
                    | fxt::to_expected([&]{
                          ++call_count;
                          return std::string{"User not found"};
                      });

        std::cout << "   Error: " << result.error() << "\n";
        std::cout << "   Factory call count: " << call_count << "\n";
    }

    // Example 16: Rich error object built lazily — the primary motivation
    std::cout << "\n16. Factory builds a rich error object lazily:\n";
    {
        struct RequestError {
            int         status;
            std::string message;
            std::string context;
        };

        auto result = get_config("missing_key")
                    | fxt::to_expected([]{
                          return RequestError{404, "Key not found", "get_config(\"missing_key\")"};
                      });

        if (result.has_value())
        {
            std::cout << "   Value: " << result.value() << "\n";
        }
        else
        {
            const auto& e = result.error();
            std::cout << "   Error " << e.status << ": " << e.message
                      << " [" << e.context << "]\n";
        }
    }

    // Example 17: Contrast eager vs lazy — side by side
    std::cout << "\n17. Eager (value) vs lazy (factory) — side by side:\n";
    {
        // Eager: error string always constructed, even for id == 2 (success)
        auto eager  = find_user(2) | fxt::to_expected<std::string>("User not found");

        // Lazy: factory called only if find_user returns nullopt
        auto lazy   = find_user(2) | fxt::to_expected([]{ return std::string{"User not found"}; });

        std::cout << "   Eager result:  " << eager.value()  << "\n";
        std::cout << "   Lazy result:   " << lazy.value()   << "\n";
    }

    // Example 18: Factory overload in a pipeline
    std::cout << "\n18. Factory overload in a pipeline:\n";
    {
        auto result = get_config("port")
                    | fxt::to_expected([]{ return std::string{"port not configured"}; })
                    | fxt::transform([](int port) { return port + 1; })
                    | fxt::and_then([](int port) -> fxt::expected<std::string, std::string> {
                          if (port > 65535) return fxt::unexpected<std::string>{"port out of range"};
                          return "Listening on port " + std::to_string(port);
                      });

        if (result.has_value())
            std::cout << "   " << result.value() << "\n";
        else
            std::cout << "   Error: " << result.error() << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}

