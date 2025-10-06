//
// Created by AI Assistant
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt/Value.hpp>
#include <fxt/Expected.hpp>
#include <fxt/Optional.hpp>
#include <fxt/Transform.hpp>
#include <fxt/AndThen.hpp>
#include <string>
#include <memory>

TEST_CASE("value - fxt::optional with lvalue", "[value]")
{
    SECTION("extract value from non-empty optional")
    {
        fxt::optional<int> opt = 42;
        int result = opt | fxt::value;
        REQUIRE(result == 42);
    }

    SECTION("extract value from optional with string")
    {
        fxt::optional<std::string> opt = "hello";
        std::string result = opt | fxt::value;
        REQUIRE(result == "hello");
    }

    SECTION("extract value from optional with custom type")
    {
        struct Point { int x, y; };
        fxt::optional<Point> opt = Point{10, 20};
        Point result = opt | fxt::value;
        REQUIRE(result.x == 10);
        REQUIRE(result.y == 20);
    }

    SECTION("throws on empty optional")
    {
        fxt::optional<int> opt;
        REQUIRE_THROWS(opt | fxt::value);
    }
}

TEST_CASE("value - fxt::optional with const lvalue", "[value]")
{
    SECTION("extract value from const optional")
    {
        const fxt::optional<int> opt = 42;
        int result = opt | fxt::value;
        REQUIRE(result == 42);
    }

    SECTION("extract value from const optional with string")
    {
        const fxt::optional<std::string> opt = "world";
        std::string result = opt | fxt::value;
        REQUIRE(result == "world");
    }
}

TEST_CASE("value - fxt::optional with rvalue", "[value]")
{
    SECTION("extract value from temporary optional")
    {
        int result = fxt::optional<int>(42) | fxt::value;
        REQUIRE(result == 42);
    }

    SECTION("extract value from temporary optional with string")
    {
        std::string result = fxt::optional<std::string>("temporary") | fxt::value;
        REQUIRE(result == "temporary");
    }

    SECTION("throws on empty temporary optional")
    {
        REQUIRE_THROWS(fxt::optional<int>() | fxt::value);
    }
}

TEST_CASE("value - fxt::optional with move-only types", "[value]")
{
    SECTION("extract unique_ptr from optional using direct call")
    {
        fxt::optional<std::unique_ptr<int>> opt = std::make_unique<int>(100);
        std::unique_ptr<int> result = fxt::value(std::move(opt));
        REQUIRE(*result == 100);
    }

    SECTION("extract unique_ptr from temporary optional using direct call")
    {
        auto result = fxt::value(fxt::optional<std::unique_ptr<int>>(std::make_unique<int>(200)));
        REQUIRE(*result == 200);
    }
}

TEST_CASE("value - fxt::expected with lvalue (success case)", "[value]")
{
    SECTION("extract value from successful expected")
    {
        fxt::expected<int, std::string> exp = 42;
        int result = exp | fxt::value;
        REQUIRE(result == 42);
    }

    SECTION("extract value from expected with string value")
    {
        fxt::expected<std::string, int> exp = "success";
        std::string result = exp | fxt::value;
        REQUIRE(result == "success");
    }

    SECTION("extract value from expected with custom type")
    {
        struct Data { double value; };
        fxt::expected<Data, std::string> exp = Data{3.14};
        Data result = exp | fxt::value;
        REQUIRE(result.value == 3.14);
    }

    SECTION("throws on expected with error")
    {
        fxt::expected<int, std::string> exp = fxt::unexpected("error");
        REQUIRE_THROWS(exp | fxt::value);
    }
}

TEST_CASE("value - fxt::expected with const lvalue", "[value]")
{
    SECTION("extract value from const expected")
    {
        const fxt::expected<int, std::string> exp = 100;
        int result = exp | fxt::value;
        REQUIRE(result == 100);
    }

    SECTION("extract value from const expected with string")
    {
        const fxt::expected<std::string, int> exp = "constant";
        std::string result = exp | fxt::value;
        REQUIRE(result == "constant");
    }
}

TEST_CASE("value - fxt::expected with rvalue", "[value]")
{
    SECTION("extract value from temporary expected")
    {
        int result = fxt::expected<int, std::string>(42) | fxt::value;
        REQUIRE(result == 42);
    }

    SECTION("extract value from temporary expected with string")
    {
        std::string result = fxt::expected<std::string, int>("rvalue") | fxt::value;
        REQUIRE(result == "rvalue");
    }

    SECTION("throws on temporary expected with error")
    {
        REQUIRE_THROWS(fxt::expected<int, std::string>(fxt::unexpected("error")) | fxt::value);
    }
}

TEST_CASE("value - fxt::expected with move-only types", "[value]")
{
    SECTION("extract unique_ptr from expected using direct call")
    {
        fxt::expected<std::unique_ptr<int>, std::string> exp = std::make_unique<int>(300);
        std::unique_ptr<int> result = fxt::value(std::move(exp));
        REQUIRE(*result == 300);
    }

    SECTION("extract unique_ptr from temporary expected using direct call")
    {
        auto result = fxt::value(fxt::expected<std::unique_ptr<int>, std::string>(std::make_unique<int>(400)));
        REQUIRE(*result == 400);
    }
}

TEST_CASE("value - chaining with other operations", "[value]")
{
    SECTION("chain with optional transform")
    {
        fxt::optional<int> opt = 5;
        auto result = opt
            | fxt::transform([](int x) { return x * 2; })
            | fxt::value;
        REQUIRE(result == 10);
    }

    SECTION("chain with expected transform")
    {
        fxt::expected<int, std::string> exp = 10;
        auto result = exp
            | fxt::transform([](int x) { return x + 5; })
            | fxt::value;
        REQUIRE(result == 15);
    }

    SECTION("chain with and_then and value")
    {
        fxt::optional<int> opt = 20;
        auto result = opt
            | fxt::and_then([](int x) -> fxt::optional<int> {
                return x > 10 ? fxt::optional<int>(x * 2) : fxt::optional<int>();
            })
            | fxt::value;
        REQUIRE(result == 40);
    }
}

TEST_CASE("value - with references", "[value]")
{
    SECTION("extract reference from optional")
    {
        int x = 42;
        fxt::optional<int> opt = x;
        int result = opt | fxt::value;
        REQUIRE(result == 42);
        // Note: .value() returns a copy for non-reference types
    }

    SECTION("extract value preserving constness")
    {
        const fxt::optional<int> opt = 100;
        int result = opt | fxt::value;
        REQUIRE(result == 100);
    }
}

TEST_CASE("value - type deduction", "[value]")
{
    SECTION("auto deduction with optional")
    {
        fxt::optional<int> opt = 42;
        auto result = opt | fxt::value;
        REQUIRE(result == 42);
        static_assert(std::is_same_v<decltype(result), int>);
    }

    SECTION("auto deduction with expected")
    {
        fxt::expected<std::string, int> exp = "text";
        auto result = exp | fxt::value;
        REQUIRE(result == "text");
        static_assert(std::is_same_v<decltype(result), std::string>);
    }

    SECTION("auto deduction with move-only type")
    {
        auto result = fxt::value(fxt::optional<std::unique_ptr<int>>(std::make_unique<int>(42)));
        REQUIRE(*result == 42);
        static_assert(std::is_same_v<decltype(result), std::unique_ptr<int>>);
    }
}

TEST_CASE("value - edge cases", "[value]")
{
    SECTION("optional with bool value")
    {
        fxt::optional<bool> opt_true = true;
        fxt::optional<bool> opt_false = false;
        REQUIRE((opt_true | fxt::value) == true);
        REQUIRE((opt_false | fxt::value) == false);
    }

    SECTION("optional with zero value")
    {
        fxt::optional<int> opt = 0;
        REQUIRE((opt | fxt::value) == 0);
    }

    SECTION("optional with empty string")
    {
        fxt::optional<std::string> opt = "";
        REQUIRE((opt | fxt::value) == "");
    }

    SECTION("expected with zero value")
    {
        fxt::expected<int, std::string> exp = 0;
        REQUIRE((exp | fxt::value) == 0);
    }

    SECTION("expected with empty string value")
    {
        fxt::expected<std::string, int> exp = "";
        REQUIRE((exp | fxt::value) == "");
    }
}

TEST_CASE("value - without pipe operator", "[value]")
{
    SECTION("direct call with optional")
    {
        fxt::optional<int> opt = 42;
        int result = fxt::value(opt);
        REQUIRE(result == 42);
    }

    SECTION("direct call with expected")
    {
        fxt::expected<int, std::string> exp = 100;
        int result = fxt::value(exp);
        REQUIRE(result == 100);
    }

    SECTION("direct call with rvalue optional")
    {
        int result = fxt::value(fxt::optional<int>(42));
        REQUIRE(result == 42);
    }

    SECTION("direct call with rvalue expected")
    {
        int result = fxt::value(fxt::expected<int, std::string>(100));
        REQUIRE(result == 100);
    }
}

TEST_CASE("value - multiple extractions", "[value]")
{
    SECTION("extract from same optional multiple times")
    {
        fxt::optional<int> opt = 42;
        int result1 = opt | fxt::value;
        int result2 = opt | fxt::value;
        REQUIRE(result1 == 42);
        REQUIRE(result2 == 42);
    }

    SECTION("extract from same expected multiple times")
    {
        fxt::expected<int, std::string> exp = 100;
        int result1 = exp | fxt::value;
        int result2 = exp | fxt::value;
        REQUIRE(result1 == 100);
        REQUIRE(result2 == 100);
    }
}

TEST_CASE("value - complex types", "[value]")
{
    SECTION("optional with vector")
    {
        fxt::optional<std::vector<int>> opt = std::vector<int>{1, 2, 3, 4, 5};
        auto result = opt | fxt::value;
        REQUIRE(result.size() == 5);
        REQUIRE(result[0] == 1);
        REQUIRE(result[4] == 5);
    }

    SECTION("expected with pair")
    {
        fxt::expected<std::pair<int, std::string>, int> exp = std::make_pair(42, "answer");
        auto result = exp | fxt::value;
        REQUIRE(result.first == 42);
        REQUIRE(result.second == "answer");
    }

    SECTION("optional with nested optional")
    {
        fxt::optional<fxt::optional<int>> nested = fxt::optional<int>(42);
        auto outer = nested | fxt::value;
        auto inner = outer | fxt::value;
        REQUIRE(inner == 42);
    }
}
