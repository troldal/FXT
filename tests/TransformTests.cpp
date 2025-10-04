//
// Created by kenne on 04/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::transform with fxt::expected", "[transform][expected]")
{
    SECTION("Transform value in expected with success")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::transform([](const int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }

    SECTION("Transform value in expected with error propagation")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("error");
        auto result = error | fxt::transform([](const int x) { return x * 2; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("Chain multiple transforms on expected")
    {
        fxt::expected<int, std::string> value = 10;
        auto result = value
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::transform([](const int x) { return x + 5; })
            | fxt::transform([](const int x) { return x * 3; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 75);  // (10 * 2 + 5) * 3 = 75
    }

    SECTION("Transform changes type")
    {
        fxt::expected<int, std::string> value = 42;
        auto result = value | fxt::transform([](const int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        REQUIRE(*result == "42");
    }

    SECTION("Transform with const lvalue expected")
    {
        const fxt::expected<int, std::string> value = 100;
        auto result = value | fxt::transform([](const int x) { return x / 10; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);
    }

    SECTION("Transform with rvalue expected")
    {
        auto result = fxt::expected<int, std::string>{50}
            | fxt::transform([](const int x) { return x + 50; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("Transform with complex types")
    {
        struct Person { std::string name; int age; };

        fxt::expected<Person, std::string> person = Person{"Alice", 30};
        auto result = person | fxt::transform([](const Person& p) {
            return p.name + " is " + std::to_string(p.age);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == "Alice is 30");
    }

    SECTION("Transform with lambda capturing variables")
    {
        int multiplier = 3;
        fxt::expected<int, std::string> value = 7;
        auto result = value | fxt::transform([multiplier](const int x) { return x * multiplier; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 21);
    }

    SECTION("Error propagation through multiple transforms")
    {
        fxt::expected<int, std::string> error = fxt::unexpected<std::string>("initial error");
        auto result = error
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::transform([](const int x) { return x + 5; })
            | fxt::transform([](const int x) { return std::to_string(x); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("fxt::transform with fxt::optional", "[transform][optional]")
{
    SECTION("Transform value in optional with value present")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::transform([](const int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }

    SECTION("Transform value in empty optional")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty | fxt::transform([](const int x) { return x * 2; });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Chain multiple transforms on optional")
    {
        fxt::optional<int> value = 10;
        auto result = value
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::transform([](const int x) { return x + 5; })
            | fxt::transform([](const int x) { return x * 3; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 75);  // (10 * 2 + 5) * 3 = 75
    }

    SECTION("Transform changes type")
    {
        fxt::optional<int> value = 42;
        auto result = value | fxt::transform([](const int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        REQUIRE(*result == "42");
    }

    SECTION("Transform with const lvalue optional")
    {
        constexpr fxt::optional<int> value = 100;
        auto result = value | fxt::transform([](const int x) { return x / 10; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 10);
    }

    SECTION("Transform with rvalue optional")
    {
        auto result = fxt::optional<int>{50}
            | fxt::transform([](const int x) { return x + 50; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("Transform with complex types")
    {
        struct Person { std::string name; int age; };

        fxt::optional<Person> person = Person{"Bob", 25};
        auto result = person | fxt::transform([](const Person& p) {
            return p.name + " is " + std::to_string(p.age);
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == "Bob is 25");
    }

    SECTION("Transform with lambda capturing variables")
    {
        int multiplier = 5;
        fxt::optional<int> value = 4;
        auto result = value | fxt::transform([multiplier](const int x) { return x * multiplier; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("Empty propagation through multiple transforms")
    {
        fxt::optional<int> empty = fxt::nullopt;
        auto result = empty
            | fxt::transform([](const int x) { return x * 2; })
            | fxt::transform([](const int x) { return x + 5; })
            | fxt::transform([](const int x) { return std::to_string(x); });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Transform from string to int")
    {
        fxt::optional<std::string> str = "hello";
        auto result = str | fxt::transform([](const std::string& s) { return s.length(); });

        REQUIRE(result.has_value());
        REQUIRE(*result == 5);
    }
}

TEST_CASE("fxt::transform mixed usage patterns", "[transform][expected][optional]")
{
    SECTION("Function call syntax with expected")
    {
        fxt::expected<int, std::string> value = 10;
        auto transformer = fxt::transform([](const int x) { return x * 2; });
        auto result = transformer(value);

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("Function call syntax with optional")
    {
        fxt::optional<int> value = 10;
        auto transformer = fxt::transform([](const int x) { return x * 2; });
        auto result = transformer(value);

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("Stored transformer can be reused")
    {
        auto doubler = fxt::transform([](const int x) { return x * 2; });

        fxt::expected<int, std::string> exp_value = 5;
        fxt::optional<int> opt_value = 7;

        auto exp_result = exp_value | doubler;
        auto opt_result = opt_value | doubler;

        REQUIRE(exp_result.has_value());
        REQUIRE(*exp_result == 10);
        REQUIRE(opt_result.has_value());
        REQUIRE(*opt_result == 14);
    }

    SECTION("Transform with move-only types")
    {
        fxt::expected<std::unique_ptr<int>, std::string> value = std::make_unique<int>(42);
        auto result = std::move(value) | fxt::transform([](auto&& p) {
            return *p * 2;
        });

        REQUIRE(result.has_value());
        REQUIRE(*result == 84);
    }

    SECTION("Transform composing expected and optional operations")
    {
        // This demonstrates that transform works consistently across types
        auto square = fxt::transform([](const int x) { return x * x; });

        fxt::expected<int, std::string> exp = 5;
        fxt::optional<int> opt = 5;

        auto exp_squared = exp | square;
        auto opt_squared = opt | square;

        REQUIRE(exp_squared.has_value());
        REQUIRE(*exp_squared == 25);
        REQUIRE(opt_squared.has_value());
        REQUIRE(*opt_squared == 25);
    }
}

