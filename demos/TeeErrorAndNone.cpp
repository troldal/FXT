//
// Demo: fxt::tap_error and fxt::tap_none
// This demonstrates how to use tap_error and tap_none to inject side effects
// on error/empty states without changing the values flowing through pipelines
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
    if (denominator < 0.0)
    {
        return fxt::unexpected("Negative denominator not allowed");
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

// Helper function to simulate database lookup
auto lookup_config(const std::string& key) -> fxt::optional<int>
{
    if (key == "port") return 8080;
    if (key == "timeout") return 30;
    if (key == "retries") return 3;
    return fxt::nullopt;
}

int main()
{
    std::cout << "=== fxt::tap_error and fxt::tap_none Demo ===\n\n";

    // =========================================================================
    // PART 1: tap_error with fxt::expected
    // =========================================================================

    std::cout << "PART 1: fxt::tap_error with fxt::expected\n";
    std::cout << "==========================================\n\n";

    // Example 1: Basic tap_error on error
    std::cout << "1. Basic tap_error (error case):\n";
    {
        auto result = safe_divide(10.0, 0.0)
                    | fxt::tap_error([](const std::string& err) {
                        std::cout << "   [ERROR] " << err << "\n";
                    });

        if (!result.has_value())
        {
            std::cout << "   Result: Error handled\n";
        }
    }

    // Example 2: tap_error not executed on success
    std::cout << "\n2. tap_error (success case - not executed):\n";
    {
        auto result = safe_divide(10.0, 2.0)
                    | fxt::tap_error([](const std::string& err) {
                        std::cout << "   [ERROR] This won't print\n";
                    });

        std::cout << "   Result: " << result.value() << "\n";
    }

    // Example 3: Combining tap and tap_error for complete logging
    std::cout << "\n3. Combining tap and tap_error:\n";
    {
        std::cout << "   Success case:\n";
        auto result1 = safe_divide(100.0, 5.0)
                     | fxt::tap([](double x) {
                         std::cout << "      [SUCCESS] Result: " << x << "\n";
                     })
                     | fxt::tap_error([](const std::string& err) {
                         std::cout << "      [ERROR] " << err << "\n";
                     });

        std::cout << "   Error case:\n";
        auto result2 = safe_divide(100.0, 0.0)
                     | fxt::tap([](double x) {
                         std::cout << "      [SUCCESS] Result: " << x << "\n";
                     })
                     | fxt::tap_error([](const std::string& err) {
                         std::cout << "      [ERROR] " << err << "\n";
                     });
    }

    // Example 4: Multiple tap_error in a pipeline
    std::cout << "\n4. Multiple tap_error calls:\n";
    {
        auto result = parse_int("abc")
                    | fxt::tap_error([](const std::string& err) {
                        std::cout << "   [LOG 1] Error detected: " << err << "\n";
                    })
                    | fxt::tap_error([](const std::string& err) {
                        std::cout << "   [LOG 2] Logging again: " << err << "\n";
                    })
                    | fxt::tap_error([](const std::string& err) {
                        std::cout << "   [LOG 3] Final error log: " << err << "\n";
                    });
    }

    // Example 5: tap_error for error counting
    std::cout << "\n5. Using tap_error to count errors:\n";
    {
        int error_count = 0;

        auto test_values = {"10", "abc", "20", "xyz", "30"};
        for (const auto& val : test_values)
        {
            auto result = parse_int(val)
                        | fxt::tap_error([&error_count](const std::string& err) {
                            error_count++;
                        });
        }

        std::cout << "   Total errors encountered: " << error_count << "\n";
    }

    // Example 6: tap_error with different error types
    std::cout << "\n6. tap_error with different error types:\n";
    {
        auto result = safe_divide(10.0, -5.0)
                    | fxt::tap_error([](const std::string& err) {
                        if (err.find("zero") != std::string::npos)
                        {
                            std::cout << "   Math error: " << err << "\n";
                        }
                        else
                        {
                            std::cout << "   Validation error: " << err << "\n";
                        }
                    });
    }

    // Example 7: tap_error in error recovery pipeline
    std::cout << "\n7. tap_error with error recovery:\n";
    {
        auto result = safe_divide(10.0, 0.0)
                    | fxt::tap_error([](const std::string& err) {
                        std::cout << "   [RECOVERY] Caught error: " << err << "\n";
                        std::cout << "   [RECOVERY] Attempting to use default value...\n";
                    })
                    | fxt::or_else([](const std::string& err) {
                        return fxt::expected<double, std::string>{0.0};
                    })
                    | fxt::tap([](double x) {
                        std::cout << "   [RECOVERY] Using value: " << x << "\n";
                    });
    }

    // Example 8: tap_error for collecting error logs
    std::cout << "\n8. Collecting error logs with tap_error:\n";
    {
        std::vector<std::string> error_log;

        auto result = parse_int("")
                    | fxt::tap_error([&error_log](const std::string& err) {
                        error_log.push_back("Parse error: " + err);
                    })
                    | fxt::transform_error([](const std::string& err) {
                        return "[WRAPPED] " + err;
                    })
                    | fxt::tap_error([&error_log](const std::string& err) {
                        error_log.push_back("Transformed error: " + err);
                    });

        std::cout << "   Error log entries:\n";
        for (const auto& entry : error_log)
        {
            std::cout << "      - " << entry << "\n";
        }
    }

    // =========================================================================
    // PART 2: tap_none with fxt::optional
    // =========================================================================

    std::cout << "\n\nPART 2: fxt::tap_none with fxt::optional\n";
    std::cout << "=========================================\n\n";

    // Example 9: Basic tap_none on empty optional
    std::cout << "9. Basic tap_none (empty case):\n";
    {
        auto result = find_user(999)
                    | fxt::tap_none([] {
                        std::cout << "   [INFO] User not found\n";
                    });

        if (!result.has_value())
        {
            std::cout << "   Result: Empty\n";
        }
    }

    // Example 10: tap_none not executed when optional has value
    std::cout << "\n10. tap_none (has value - not executed):\n";
    {
        auto result = find_user(1)
                    | fxt::tap_none([] {
                        std::cout << "   [INFO] This won't print\n";
                    });

        std::cout << "   Result: " << result.value() << "\n";
    }

    // Example 11: Combining tap and tap_none for complete logging
    std::cout << "\n11. Combining tap and tap_none:\n";
    {
        std::cout << "   Case 1 (has value):\n";
        auto result1 = find_user(2)
                     | fxt::tap([](const std::string& name) {
                         std::cout << "      [FOUND] User: " << name << "\n";
                     })
                     | fxt::tap_none([] {
                         std::cout << "      [NOT FOUND] User does not exist\n";
                     });

        std::cout << "   Case 2 (empty):\n";
        auto result2 = find_user(999)
                     | fxt::tap([](const std::string& name) {
                         std::cout << "      [FOUND] User: " << name << "\n";
                     })
                     | fxt::tap_none([] {
                         std::cout << "      [NOT FOUND] User does not exist\n";
                     });
    }

    // Example 12: Multiple tap_none calls
    std::cout << "\n12. Multiple tap_none calls:\n";
    {
        auto result = lookup_config("invalid_key")
                    | fxt::tap_none([] {
                        std::cout << "   [LOG 1] Config key not found\n";
                    })
                    | fxt::tap_none([] {
                        std::cout << "   [LOG 2] Using default configuration\n";
                    })
                    | fxt::tap_none([] {
                        std::cout << "   [LOG 3] Configuration fallback activated\n";
                    });
    }

    // Example 13: tap_none for counting empty optionals
    std::cout << "\n13. Counting empty optionals with tap_none:\n";
    {
        int not_found_count = 0;

        auto test_ids = {1, 999, 2, 888, 3, 777};
        for (int id : test_ids)
        {
            auto result = find_user(id)
                        | fxt::tap_none([&not_found_count] {
                            not_found_count++;
                        });
        }

        std::cout << "   Total users not found: " << not_found_count << "\n";
    }

    // Example 14: tap_none with or_else for default values
    std::cout << "\n14. tap_none with or_else:\n";
    {
        auto result = lookup_config("missing_key")
                    | fxt::tap_none([] {
                        std::cout << "   [FALLBACK] Config not found, using default\n";
                    })
                    | fxt::or_else([]() {
                        return fxt::optional<int>{9999};
                    })
                    | fxt::tap([](int x) {
                        std::cout << "   [CONFIG] Using value: " << x << "\n";
                    });
    }

    // Example 15: tap_none in a transformation pipeline
    std::cout << "\n15. tap_none in a pipeline:\n";
    {
        auto result = find_user(500)
                    | fxt::tap_none([] {
                        std::cout << "   [WARN] User ID not in database\n";
                    })
                    | fxt::transform([](const std::string& name) {
                        return "Hello, " + name;
                    })
                    | fxt::tap([](const std::string& greeting) {
                        std::cout << "   [OUTPUT] " << greeting << "\n";
                    })
                    | fxt::tap_none([] {
                        std::cout << "   [OUTPUT] No greeting to display\n";
                    });
    }

    // Example 16: tap_none for cache miss logging
    std::cout << "\n16. Using tap_none for cache miss logging:\n";
    {
        auto lookup_cache = [](int id) -> fxt::optional<std::string> {
            // Simulate cache misses
            return fxt::nullopt;
        };

        int cache_misses = 0;

        auto ids = {101, 102, 103, 104, 105};
        for (int id : ids)
        {
            auto result = lookup_cache(id)
                        | fxt::tap_none([&cache_misses, id] {
                            std::cout << "   [CACHE MISS] ID: " << id << "\n";
                            cache_misses++;
                        });
        }

        std::cout << "   Total cache misses: " << cache_misses << "\n";
    }

    // =========================================================================
    // PART 3: Advanced combinations
    // =========================================================================

    std::cout << "\n\nPART 3: Advanced Combinations\n";
    std::cout << "==============================\n\n";

    // Example 17: Combining all three tap variants
    std::cout << "17. Using tap, tap_error, and tap_none together:\n";
    {
        std::cout << "   Pipeline 1 (expected success):\n";
        auto result1 = parse_int("42")
                     | fxt::tap([](int x) {
                         std::cout << "      [VALUE] " << x << "\n";
                     })
                     | fxt::tap_error([](const std::string& err) {
                         std::cout << "      [ERROR] " << err << "\n";
                     })
                     | fxt::to_optional()
                     | fxt::tap([](int x) {
                         std::cout << "      [OPTIONAL VALUE] " << x << "\n";
                     })
                     | fxt::tap_none([] {
                         std::cout << "      [OPTIONAL EMPTY]\n";
                     });

        std::cout << "   Pipeline 2 (expected error):\n";
        auto result2 = parse_int("xyz")
                     | fxt::tap([](int x) {
                         std::cout << "      [VALUE] " << x << "\n";
                     })
                     | fxt::tap_error([](const std::string& err) {
                         std::cout << "      [ERROR] " << err << "\n";
                     })
                     | fxt::to_optional()
                     | fxt::tap([](int x) {
                         std::cout << "      [OPTIONAL VALUE] " << x << "\n";
                     })
                     | fxt::tap_none([] {
                         std::cout << "      [OPTIONAL EMPTY]\n";
                     });
    }

    // Example 18: Error tracking across conversions
    std::cout << "\n18. Tracking errors across conversions:\n";
    {
        bool error_in_expected = false;
        bool resulted_in_empty = false;

        auto result = safe_divide(10.0, 0.0)
                    | fxt::tap_error([&error_in_expected](const std::string& err) {
                        std::cout << "   [TRACK] Error in expected: " << err << "\n";
                        error_in_expected = true;
                    })
                    | fxt::to_optional()
                    | fxt::tap_none([&resulted_in_empty] {
                        std::cout << "   [TRACK] Converted to empty optional\n";
                        resulted_in_empty = true;
                    });

        std::cout << "   Error occurred: " << (error_in_expected ? "yes" : "no") << "\n";
        std::cout << "   Resulted in empty: " << (resulted_in_empty ? "yes" : "no") << "\n";
    }

    // Example 19: Building comprehensive logs
    std::cout << "\n19. Building comprehensive operation logs:\n";
    {
        struct OperationLog {
            std::vector<std::string> successes;
            std::vector<std::string> errors;
            std::vector<std::string> empties;
        };

        OperationLog log;

        auto process = [&log](const std::string& input) {
            return parse_int(input)
                 | fxt::tap([&log, input](int x) {
                     log.successes.push_back("Parsed '" + input + "' = " + std::to_string(x));
                 })
                 | fxt::tap_error([&log, input](const std::string& err) {
                     log.errors.push_back("Failed '" + input + "': " + err);
                 })
                 | fxt::to_optional()
                 | fxt::tap_none([&log, input] {
                     log.empties.push_back("Empty result for '" + input + "'");
                 });
        };

        process("123");
        process("abc");
        process("456");
        process("");

        std::cout << "   Successes: " << log.successes.size() << "\n";
        for (const auto& s : log.successes)
            std::cout << "      " << s << "\n";

        std::cout << "   Errors: " << log.errors.size() << "\n";
        for (const auto& e : log.errors)
            std::cout << "      " << e << "\n";

        std::cout << "   Empties: " << log.empties.size() << "\n";
        for (const auto& e : log.empties)
            std::cout << "      " << e << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}


