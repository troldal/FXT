//
// Created by kenne on 05/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::value_or with fxt::expected", "[value_or][expected]")
{
    SECTION("value_or returns value when expected has value")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::value_or(99);

        REQUIRE(result == 42);
    }

    SECTION("value_or returns alternative when expected has error")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::value_or(99);

        REQUIRE(result == 99);
    }

    SECTION("value_or with const lvalue expected")
    {
        const fxt::expected<int, std::string> value = 100;
        auto result = value | fxt::value_or(50);

        REQUIRE(result == 100);
    }

    SECTION("value_or with const lvalue expected containing error")
    {
        const fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::value_or(50);

        REQUIRE(result == 50);
    }

    SECTION("value_or with rvalue expected")
    {
        auto result = fxt::expected<int, std::string>{42} | fxt::value_or(10);

        REQUIRE(result == 42);
    }

    SECTION("value_or with rvalue expected containing error")
    {
        auto result = fxt::expected<int, std::string>{fxt::unexpected<std::string>("error")}
            | fxt::value_or(10);

        REQUIRE(result == 10);
    }

    SECTION("value_or with string types")
    {
        fxt::expected<std::string, int> value = "hello";
        auto result = value | fxt::value_or("default");

        REQUIRE(result == "hello");
    }

    SECTION("value_or with string types and error")
    {
        fxt::expected<std::string, int> error = fxt::unexpected<int>{404};
        auto result = error | fxt::value_or("default");

        REQUIRE(result == "default");
    }

    SECTION("value_or with complex types")
    {
        struct Point { int x; int y; };

        fxt::expected<Point, std::string> value = Point{10, 20};
        auto result = value | fxt::value_or(Point{0, 0});

        REQUIRE(result.x == 10);
        REQUIRE(result.y == 20);
    }

    SECTION("value_or with complex types and error")
    {
        struct Point { int x; int y; };

        fxt::expected<Point, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::value_or(Point{0, 0});

        REQUIRE(result.x == 0);
        REQUIRE(result.y == 0);
    }

    SECTION("value_or after transform")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::value_or(0);

        REQUIRE(result == 20);
    }

    SECTION("value_or after transform with error")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::value_or(0);

        REQUIRE(result == 0);
    }

    SECTION("value_or after and_then")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return x * 2;
            })
            | fxt::value_or(0);

        REQUIRE(result == 20);
    }

    SECTION("value_or after and_then with error")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::expected<int, std::string> {
                return fxt::unexpected<std::string>("failed");
            })
            | fxt::value_or(99);

        REQUIRE(result == 99);
    }

    SECTION("value_or with zero as alternative")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::value_or(0);

        REQUIRE(result == 0);
    }

    SECTION("value_or with negative alternative")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::value_or(-1);

        REQUIRE(result == -1);
    }

    SECTION("value_or preserves value through error recovery chain")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return 10;
            })
            | fxt::value_or(99);

        REQUIRE(result == 42);
    }

    SECTION("value_or as final fallback in error recovery chain")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error
            | fxt::or_else([](const std::string&) -> fxt::expected<int, std::string> {
                return fxt::unexpected<std::string>("still error");
            })
            | fxt::value_or(99);

        REQUIRE(result == 99);
    }

    SECTION("value_or with different error types")
    {
        fxt::expected<int, int> error = fxt::unexpected<int>{404};
        auto result = error | fxt::value_or(0);

        REQUIRE(result == 0);
    }

    SECTION("Function call syntax")
    {
        fxt::expected<int, std::string> value = 42;
        auto value_or_op = fxt::value_or(99);
        auto result = value_or_op(value);

        REQUIRE(result == 42);
    }

    SECTION("Function call syntax with error")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto value_or_op = fxt::value_or(99);
        auto result = value_or_op(error);

        REQUIRE(result == 99);
    }
}

TEST_CASE("fxt::value_or with fxt::optional", "[value_or][optional]")
{
    SECTION("value_or returns value when optional has value")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::value_or(99);

        REQUIRE(result == 42);
    }

    SECTION("value_or returns alternative when optional is empty")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(99);

        REQUIRE(result == 99);
    }

    SECTION("value_or with const lvalue optional")
    {
        const fxt::optional<int> value = 100;
        auto result = value | fxt::value_or(50);

        REQUIRE(result == 100);
    }

    SECTION("value_or with const lvalue empty optional")
    {
        const fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(50);

        REQUIRE(result == 50);
    }

    SECTION("value_or with rvalue optional")
    {
        auto result = fxt::optional<int>{42} | fxt::value_or(10);

        REQUIRE(result == 42);
    }

    SECTION("value_or with rvalue empty optional")
    {
        auto result = fxt::optional<int>{} | fxt::value_or(10);

        REQUIRE(result == 10);
    }

    SECTION("value_or with string types")
    {
        fxt::optional<std::string> value = "hello";
        auto result = value | fxt::value_or("default");

        REQUIRE(result == "hello");
    }

    SECTION("value_or with string types and empty")
    {
        fxt::optional<std::string> empty = fxt::nullopt;
        auto result = empty | fxt::value_or("default");

        REQUIRE(result == "default");
    }

    SECTION("value_or with complex types")
    {
        struct Point { int x; int y; };

        fxt::optional<Point> value = Point{10, 20};
        auto result = value | fxt::value_or(Point{0, 0});

        REQUIRE(result.x == 10);
        REQUIRE(result.y == 20);
    }

    SECTION("value_or with complex types and empty")
    {
        struct Point { int x; int y; };

        fxt::optional<Point> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(Point{0, 0});

        REQUIRE(result.x == 0);
        REQUIRE(result.y == 0);
    }

    SECTION("value_or after transform")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::value_or(0);

        REQUIRE(result == 20);
    }

    SECTION("value_or after transform with empty")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::value_or(0);

        REQUIRE(result == 0);
    }

    SECTION("value_or after and_then")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return x * 2;
            })
            | fxt::value_or(0);

        REQUIRE(result == 20);
    }

    SECTION("value_or after and_then with empty result")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                return fxt::nullopt;
            })
            | fxt::value_or(99);

        REQUIRE(result == 99);
    }

    SECTION("value_or with zero as alternative")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(0);

        REQUIRE(result == 0);
    }

    SECTION("value_or with negative alternative")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(-1);

        REQUIRE(result == -1);
    }

    SECTION("value_or preserves value through fallback chain")
    {
        fxt::optional<int> value = 42;
        auto result = value
            | fxt::or_else([]() -> fxt::optional<int> {
                return 10;
            })
            | fxt::value_or(99);

        REQUIRE(result == 42);
    }

    SECTION("value_or as final fallback in empty chain")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::or_else([]() -> fxt::optional<int> {
                return fxt::nullopt;
            })
            | fxt::value_or(99);

        REQUIRE(result == 99);
    }

    SECTION("value_or with floating point")
    {
        fxt::optional<double> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(3.14);

        REQUIRE(result == 3.14);
    }

    SECTION("value_or with bool type")
    {
        fxt::optional<bool> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(true);

        REQUIRE(result == true);
    }

    SECTION("Function call syntax")
    {
        fxt::optional<int> value = 42;
        auto value_or_op = fxt::value_or(99);
        auto result = value_or_op(value);

        REQUIRE(result == 42);
    }

    SECTION("Function call syntax with empty")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto value_or_op = fxt::value_or(99);
        auto result = value_or_op(empty);

        REQUIRE(result == 99);
    }

    SECTION("value_or with constexpr optional")
    {
        constexpr fxt::optional<int> value = 42;
        auto result = value | fxt::value_or(99);

        REQUIRE(result == 42);
    }
}

TEST_CASE("fxt::value_or mixed usage patterns", "[value_or][expected][optional]")
{
    SECTION("Reusable value_or operation")
    {
        auto default_to_zero = fxt::value_or(0);

        fxt::expected<int, std::string> exp_value = 42;
        fxt::expected<int, std::string> exp_error = fxt::unexpected<std::string>("error");
        fxt::optional<int> opt_value = 42;
        fxt::optional<int> opt_empty = fxt::nullopt;

        REQUIRE((exp_value | default_to_zero) == 42);
        REQUIRE((exp_error | default_to_zero) == 0);
        REQUIRE((opt_value | default_to_zero) == 42);
        REQUIRE((opt_empty | default_to_zero) == 0);
    }

    SECTION("Complex pipeline ending with value_or")
    {
        auto parse_int = [](const std::string& s) -> fxt::optional<int> {
            try {
                return std::stoi(s);
            } catch (...) {
                return fxt::nullopt;
            }
        };

        // Valid input
        auto result1 = parse_int("42")
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                if (x > 50) return x;
                return fxt::nullopt;
            })
            | fxt::value_or(100);

        REQUIRE(result1 == 84);

        // Invalid input
        auto result2 = parse_int("invalid")
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                if (x > 50) return x;
                return fxt::nullopt;
            })
            | fxt::value_or(100);

        REQUIRE(result2 == 100);

        // Valid input but fails validation
        auto result3 = parse_int("10")
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::and_then([](const int x) -> fxt::optional<int> {
                if (x > 50) return x;
                return fxt::nullopt;
            })
            | fxt::value_or(100);

        REQUIRE(result3 == 100);
    }

    SECTION("value_or with computed alternative")
    {
        int computed_default = 42 + 58;

        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::value_or(computed_default);

        REQUIRE(result == 100);
    }

    SECTION("value_or in error recovery strategy")
    {
        auto safe_divide = [](int a, int b) -> fxt::expected<int, std::string> {
            if (b == 0) return fxt::unexpected<std::string>("division by zero");
            return a / b;
        };

        // Success case
        auto result1 = safe_divide(100, 5) | fxt::value_or(0);
        REQUIRE(result1 == 20);

        // Error case
        auto result2 = safe_divide(100, 0) | fxt::value_or(0);
        REQUIRE(result2 == 0);

        // With transformation
        auto result3 = safe_divide(100, 5)
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::value_or(0);
        REQUIRE(result3 == 40);

        // With transformation and error
        auto result4 = safe_divide(100, 0)
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::value_or(0);
        REQUIRE(result4 == 0);
    }

    SECTION("value_or with std::string alternatives")
    {
        fxt::optional<std::string> name = fxt::nullopt;
        auto result = name | fxt::value_or(std::string("Unknown"));

        REQUIRE(result == "Unknown");
    }

    SECTION("Multiple value_or operations")
    {
        // This demonstrates that value_or extracts the value, not returning optional/expected
        fxt::optional<int> value = 42;
        int result = value | fxt::value_or(0);

        // result is int, not optional<int>, so we can use it directly
        result = result + 10;

        REQUIRE(result == 52);
    }

    SECTION("value_or with chained or_else")
    {
        fxt::optional<int> empty = fxt::nullopt;

        auto result = empty
            | fxt::or_else([]() -> fxt::optional<int> { return fxt::nullopt; })
            | fxt::or_else([]() -> fxt::optional<int> { return fxt::nullopt; })
            | fxt::value_or(42);

        REQUIRE(result == 42);
    }
}

