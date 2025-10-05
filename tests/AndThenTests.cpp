//
// Created by kenne on 05/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::and_then with fxt::expected", "[and_then][expected]")
{
    SECTION("and_then with success value")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
            return x * 2;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }

    SECTION("and_then with error propagation")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
            return x * 2;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("and_then can return error")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
            if (x > 40) {
                return fxt::unexpected<std::string>("value too large");
            }
            return x * 2;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "value too large");
    }

    SECTION("Chain multiple and_then operations")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x * 2;
            })
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x + 5;
            })
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x * 3;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 75);  // (10 * 2 + 5) * 3 = 75
    }

    SECTION("and_then changes type")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::and_then([](const int x) -> fxt::expected<std::string, std::string> {
            return std::to_string(x);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == "42");
    }

    SECTION("and_then with const lvalue expected")
    {
        const fxt::expected<int, std::string> value = 100;
        auto result = value | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
            return x / 10;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);
    }

    SECTION("and_then with rvalue expected")
    {
        auto result = fxt::expected<int, std::string>{50}
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x + 50;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("and_then with complex types")
    {
        struct Person { std::string name; int age; };

        fxt::expected<Person, std::string> person = Person{"Alice", 30};
        auto result = person | fxt::and_then([](const Person& p) -> fxt::expected<std::string, std::string> {
            if (p.age < 18) {
                return fxt::unexpected<std::string>("too young");
            }
            return p.name + " is " + std::to_string(p.age);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == "Alice is 30");
    }

    SECTION("and_then with lambda capturing variables")
    {
        int threshold = 50;
        fxt::expected<int, std::string> value = 45;
        auto result = value | fxt::and_then([threshold](const int x) -> fxt::expected<int, std::string> {
            if (x < threshold) {
                return fxt::unexpected<std::string>("below threshold");
            }
            return x * 2;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "below threshold");
    }

    SECTION("Error propagation through multiple and_then operations")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("initial error");
        auto result = error
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x * 2;
            })
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x + 5;
            })
            | fxt::and_then([](const int x) -> fxt::expected<std::string, std::string> {
                return std::to_string(x);
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }

    SECTION("and_then with validation logic")
    {
        auto divide = [](int numerator) {
            return [numerator](const int denominator) -> fxt::expected<int, std::string> {
                if (denominator == 0) {
                    return fxt::unexpected<std::string>("division by zero");
                }
                return numerator / denominator;
            };
        };

        fxt::expected<int, std::string> value1 = 5;
        auto result1 = value1 | fxt::and_then(divide(100));

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 20);

        fxt::expected<int, std::string> value2 = 0;
        auto result2 = value2 | fxt::and_then(divide(100));

        REQUIRE_FALSE(result2.has_value());
        REQUIRE(result2.error() == "division by zero");
    }

    SECTION("Mixing and_then with transform")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                if (x < 5) return fxt::unexpected<std::string>("too small");
                return x * 2;
            })
            | fxt::transform([](const int x) { return x + 5; })
            | fxt::and_then([](const int x) -> fxt::expected<std::string, std::string> {
                return "Result: " + std::to_string(x);
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == "Result: 25");
    }

    SECTION("and_then with early error in chain")
    {
        fxt::expected<int, std::string> value = 2;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                if (x < 5) return fxt::unexpected<std::string>("first check failed");
                return x * 2;
            })
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x + 100;  // This should not execute
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "first check failed");
    }
}

TEST_CASE("fxt::and_then with fxt::optional", "[and_then][optional]")
{
    SECTION("and_then with value present")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::and_then([](const int x) -> fxt::optional<int> {
            return x * 2;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }

    SECTION("and_then with empty optional")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::and_then([](const int x) -> fxt::optional<int> {
            return x * 2;
        });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("and_then can return nullopt")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::and_then([](const int x) -> fxt::optional<int> {
            if (x > 40) {
                return fxt::nullopt;
            }
            return x * 2;
        });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Chain multiple and_then operations")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x * 2;
            })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x + 5;
            })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x * 3;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 75);  // (10 * 2 + 5) * 3 = 75
    }

    SECTION("and_then changes type")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::and_then([](const int x) -> fxt::optional<std::string> {
            return std::to_string(x);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == "42");
    }

    SECTION("and_then with const lvalue optional")
    {
        constexpr fxt::optional<int> value = 100;
        auto result = value | fxt::and_then([](const int x) -> fxt::optional<int> {
            return x / 10;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);
    }

    SECTION("and_then with rvalue optional")
    {
        auto result = fxt::optional<int>{50}
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x + 50;
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("and_then with complex types")
    {
        struct Person { std::string name; int age; };

        fxt::optional<Person> person = Person{"Bob", 25};
        auto result = person | fxt::and_then([](const Person& p) -> fxt::optional<std::string> {
            if (p.age < 18) {
                return fxt::nullopt;
            }
            return p.name + " is " + std::to_string(p.age);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == "Bob is 25");
    }

    SECTION("and_then with lambda capturing variables")
    {
        int threshold = 50;
        fxt::optional<int> value = 45;
        auto result = value | fxt::and_then([threshold](const int x) -> fxt::optional<int> {
            if (x < threshold) {
                return fxt::nullopt;
            }
            return x * 2;
        });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Empty propagation through multiple and_then operations")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x * 2;
            })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x + 5;
            })
            | fxt::and_then([](const int x) -> fxt::optional<std::string> {
                return std::to_string(x);
            });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("and_then with validation logic")
    {
        auto safe_divide = [](int numerator) {
            return [numerator](const int denominator) -> fxt::optional<int> {
                if (denominator == 0) {
                    return fxt::nullopt;
                }
                return numerator / denominator;
            };
        };

        fxt::optional<int> value1 = 5;
        auto result1 = value1 | fxt::and_then(safe_divide(100));

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 20);

        fxt::optional<int> value2 = 0;
        auto result2 = value2 | fxt::and_then(safe_divide(100));

        REQUIRE_FALSE(result2.has_value());
    }

    SECTION("Mixing and_then with transform")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                if (x < 5) return fxt::nullopt;
                return x * 2;
            })
            | fxt::transform([](const int x) { return x + 5; })
            | fxt::and_then([](const int x) -> fxt::optional<std::string> {
                return "Result: " + std::to_string(x);
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == "Result: 25");
    }

    SECTION("and_then with early nullopt in chain")
    {
        fxt::optional<int> value = 2;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                if (x < 5) return fxt::nullopt;
                return x * 2;
            })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x + 100;  // This should not execute
            });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("and_then for string parsing")
    {
        auto parse_int = [](const std::string& s) -> fxt::optional<int> {
            try {
                return std::stoi(s);
            } catch (...) {
                return fxt::nullopt;
            }
        };

        fxt::optional<std::string> valid = "123";
        auto result1 = valid | fxt::and_then(parse_int);

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 123);

        fxt::optional<std::string> invalid = "abc";
        auto result2 = invalid | fxt::and_then(parse_int);

        REQUIRE_FALSE(result2.has_value());
    }
}

TEST_CASE("fxt::and_then mixed usage patterns", "[and_then][expected][optional]")
{
    SECTION("Function call syntax with expected")
    {
        fxt::expected<int, std::string> value = 10;
        auto and_then_op = fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
            return x * 2;
        });
        auto result = and_then_op(value);

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("Function call syntax with optional")
    {
        fxt::optional<int> value = 10;
        auto and_then_op = fxt::and_then([](const int x) -> fxt::optional<int> {
            return x * 2;
        });
        auto result = and_then_op(value);

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("Stored and_then can be reused")
    {
        auto safe_doubler = fxt::and_then([](const int x) -> fxt::optional<int> {
            if (x > 1000) return fxt::nullopt;
            return x * 2;
        });

        fxt::optional<int> value1 = 5;
        fxt::optional<int> value2 = 2000;

        auto result1 = value1 | safe_doubler;
        auto result2 = value2 | safe_doubler;

        REQUIRE(result1.has_value());
        REQUIRE(*result1 == 10);
        REQUIRE_FALSE(result2.has_value());
    }

    SECTION("and_then with move-only types")
    {
        fxt::expected<std::unique_ptr<int>, std::string> value = std::make_unique<int>(42);
        auto result = std::move(value) | fxt::and_then([](auto&& p) -> fxt::expected<int, std::string> {
            return *p * 2;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }

    SECTION("Complex pipeline with and_then and transform")
    {
        // Simulate a parsing and validation pipeline
        auto parse_positive = [](const std::string& s) -> fxt::expected<int, std::string> {
            try {
                int val = std::stoi(s);
                if (val <= 0) {
                    return fxt::unexpected<std::string>("not positive");
                }
                return val;
            } catch (...) {
                return fxt::unexpected<std::string>("parse error");
            }
        };

        auto validate_range = [](const int x) -> fxt::expected<int, std::string> {
            if (x > 100) {
                return fxt::unexpected<std::string>("out of range");
            }
            return x;
        };

        fxt::expected<std::string, std::string> input = "42";
        auto result = input
            | fxt::and_then(parse_positive)
            | fxt::and_then(validate_range)
            | fxt::transform([](const int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);

        fxt::expected<std::string, std::string> invalid_input = "200";
        auto result2 = invalid_input
            | fxt::and_then(parse_positive)
            | fxt::and_then(validate_range)
            | fxt::transform([](const int x) { return x * 2; });

        REQUIRE_FALSE(result2.has_value());
        REQUIRE(result2.error() == "out of range");
    }
}

