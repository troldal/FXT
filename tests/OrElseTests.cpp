//
// Created by kenne on 05/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::or_else with fxt::expected", "[or_else][expected]")
{
    SECTION("or_else not called on success value")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
            return 0;  // Should not be called
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("or_else called on error")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
            return 99;  // Fallback value
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 99);
    }

    SECTION("or_else can propagate error")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("original error");
        auto result = error | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
            return fxt::unexpected<std::string>("new error: " + err);
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "new error: original error");
    }

    SECTION("Chain multiple or_else operations")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return fxt::unexpected<std::string>("still error");
            })
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return fxt::unexpected<std::string>("still error 2");
            })
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 100;  // Finally recover
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("or_else with error inspection")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("404");
        auto result = error | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
            if (err == "404") {
                return 0;  // Default for not found
            }
            return fxt::unexpected<std::string>(err);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 0);
    }

    SECTION("or_else with const lvalue expected")
    {
        const fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
            return 50;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 50);
    }

    SECTION("or_else with rvalue expected")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected<std::string>("error")}
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 75;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 75);
    }

    SECTION("or_else with complex error types")
    {
        struct ErrorInfo {
            int code;
            std::string message;
        };

        fxt::expected<int, ErrorInfo> error = fxt::unexpected<ErrorInfo>{ErrorInfo{404, "Not Found"}};
        auto result = error | fxt::or_else([](const ErrorInfo& err) -> fxt::expected<int, ErrorInfo> {
            if (err.code == 404) {
                return -1;  // Special value for not found
            }
            return fxt::unexpected<ErrorInfo>(err);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == -1);
    }

    SECTION("or_else with lambda capturing variables")
    {
        int default_value = 42;
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::or_else([default_value](const std::string&) -> fxt::expected<int, std::string> {
            return default_value;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("Success propagation through multiple or_else operations")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 20;  // Should not be called
            })
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 30;  // Should not be called
            })
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 40;  // Should not be called
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);  // Original value preserved
    }

    SECTION("or_else with error type transformation")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("42");
        auto result = error | fxt::or_else([](const std::string& err) -> fxt::expected<int, std::string> {
            try {
                return std::stoi(err);  // Try to parse error as number
            } catch (...) {
                return fxt::unexpected<std::string>("parse failed");
            }
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("or_else with retry logic")
    {
        int attempt = 0;
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("failed");

        auto result = error
            | fxt::or_else([&attempt](const std::string&) -> fxt::expected<int, std::string> {
                ++attempt;
                return fxt::unexpected<std::string>("retry 1");
            })
            | fxt::or_else([&attempt](const std::string&) -> fxt::expected<int, std::string> {
                ++attempt;
                return fxt::unexpected<std::string>("retry 2");
            })
            | fxt::or_else([&attempt](const std::string&) -> fxt::expected<int, std::string> {
                ++attempt;
                return 100;  // Success after retries
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
        REQUIRE(attempt == 3);
    }

    SECTION("Mixing or_else with transform")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 5;
            })
            | fxt::transform([](const int x) { return x * 10; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 50);
    }

    SECTION("Mixing or_else with and_then")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 10;
            })
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x * 2;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }
}

TEST_CASE("fxt::or_else with fxt::optional", "[or_else][optional]")
{
    SECTION("or_else not called on value present")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::or_else([]() -> fxt::optional<int> {
            return 0;  // Should not be called
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("or_else called on empty optional")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::or_else([]() -> fxt::optional<int> {
            return 99;  // Fallback value
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 99);
    }

    SECTION("or_else can return nullopt")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::or_else([]() -> fxt::optional<int> {
            return fxt::nullopt;  // Still no value
        });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Chain multiple or_else operations")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::or_else([]() -> fxt::optional<int> {
                return fxt::nullopt;  // Still empty
            })
            | fxt::or_else([]() -> fxt::optional<int> {
                return fxt::nullopt;  // Still empty
            })
            | fxt::or_else([]() -> fxt::optional<int> {
                return 100;  // Finally provide value
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("or_else with const lvalue optional")
    {
        const fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::or_else([]() -> fxt::optional<int> {
            return 50;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 50);
    }

    SECTION("or_else with rvalue optional")
    {
        auto result = fxt::optional<int>{}
            | fxt::or_else([]() -> fxt::optional<int> {
                return 75;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 75);
    }

    SECTION("or_else with complex types")
    {
        struct Person { std::string name; int age; };

        fxt::optional<Person> empty = fxt::nullopt;
        auto result = empty | fxt::or_else([]() -> fxt::optional<Person> {
            return Person{"Default", 0};
        });

        REQUIRE(result.has_value());
        REQUIRE(result->name == "Default");
        REQUIRE(result->age == 0);
    }

    SECTION("or_else with lambda capturing variables")
    {
        int default_value = 42;
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::or_else([default_value]() -> fxt::optional<int> {
            return default_value;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("Value propagation through multiple or_else operations")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::or_else([]() -> fxt::optional<int> {
                return 20;  // Should not be called
            })
            | fxt::or_else([]() -> fxt::optional<int> {
                return 30;  // Should not be called
            })
            | fxt::or_else([]() -> fxt::optional<int> {
                return 40;  // Should not be called
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);  // Original value preserved
    }

    SECTION("or_else with fallback chain")
    {
        fxt::optional<int> empty = fxt::nullopt;

        auto try_source_1 = []() -> fxt::optional<int> { return fxt::nullopt; };
        auto try_source_2 = []() -> fxt::optional<int> { return fxt::nullopt; };
        auto try_source_3 = []() -> fxt::optional<int> { return 42; };

        auto result = empty
            | fxt::or_else(try_source_1)
            | fxt::or_else(try_source_2)
            | fxt::or_else(try_source_3);

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("or_else with conditional logic")
    {
        bool use_default = true;
        fxt::optional<int> empty = fxt::nullopt;

        auto result = empty | fxt::or_else([use_default]() -> fxt::optional<int> {
            if (use_default) {
                return 100;
            }
            return fxt::nullopt;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("Mixing or_else with transform")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::or_else([]() -> fxt::optional<int> {
                return 5;
            })
            | fxt::transform([](const int x) { return x * 10; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 50);
    }

    SECTION("Mixing or_else with and_then")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::or_else([]() -> fxt::optional<int> {
                return 10;
            })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x * 2;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("or_else for lazy evaluation")
    {
        bool expensive_computed = false;
        auto expensive_fallback = [&expensive_computed]() -> fxt::optional<int> {
            expensive_computed = true;
            return 42;
        };

        // With value - should not compute expensive fallback
        fxt::optional<int> value = 10;
        auto result1 = value | fxt::or_else(expensive_fallback);

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 10);
        REQUIRE_FALSE(expensive_computed);  // Not called

        // Without value - should compute expensive fallback
        fxt::optional<int> empty = fxt::nullopt;
        auto result2 = empty | fxt::or_else(expensive_fallback);

        REQUIRE(result2.has_value());
        REQUIRE(*result2 == 42);
        REQUIRE(expensive_computed);  // Now called
    }
}

TEST_CASE("fxt::or_else mixed usage patterns", "[or_else][expected][optional]")
{
    SECTION("Function call syntax with expected")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto or_else_op = fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
            return 42;
        });
        auto result = or_else_op(error);

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("Function call syntax with optional")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto or_else_op = fxt::or_else([]() -> fxt::optional<int> {
            return 42;
        });
        auto result = or_else_op(empty);

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("Stored or_else can be reused")
    {
        auto fallback_to_zero_exp = fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
            return 0;
        });
        auto fallback_to_zero_opt = fxt::or_else([]() -> fxt::optional<int> {
            return 0;
        });

        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        fxt::optional<int> empty = fxt::nullopt;

        auto exp_result = error | fallback_to_zero_exp;
        auto opt_result = empty | fallback_to_zero_opt;

        REQUIRE(exp_result.has_value());
        REQUIRE(*exp_result == 0);
        REQUIRE(opt_result.has_value());
        REQUIRE(*opt_result == 0);
    }

    SECTION("Complex pipeline with error recovery")
    {
        auto parse_int = [](const std::string& s) -> fxt::expected<int, std::string> {
            try {
                return std::stoi(s);
            } catch (...) {
                return fxt::unexpected<std::string>("parse error");
            }
        };

        auto validate_positive = [](const int x) -> fxt::expected<int, std::string> {
            if (x > 0) return x;
            return fxt::unexpected<std::string>("not positive");
        };

        // Successful parsing
        fxt::expected<std::string, std::string> input1 = "42";
        auto result1 = input1
            | fxt::and_then(parse_int)
            | fxt::and_then(validate_positive)
            | fxt::transform([](const int x) { return x * 2; });

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 84);

        // Failed parsing with recovery
        fxt::expected<std::string, std::string> input2 = "invalid";
        auto result2 = input2
            | fxt::and_then(parse_int)
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 0;  // Default value on parse error
            })
            | fxt::transform([](const int x) { return x * 2; });

        REQUIRE(result2.has_value());
        REQUIRE(*result2 == 0);
    }
}
