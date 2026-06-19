//
// Test suite for fxt::get
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

TEST_CASE("get - fxt::expected by index", "[get]")
{
    SECTION("get element at index 0")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(42, 3.14, "hello")
        };
        auto result = exp | fxt::mget<0>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("get element at index 1")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(42, 3.14, "hello")
        };
        auto result = exp | fxt::mget<1>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 3.14);
    }

    SECTION("get element at index 2")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(42, 3.14, "hello")
        };
        auto result = exp | fxt::mget<2>();

        REQUIRE(result.has_value());
        REQUIRE(*result == "hello");
    }

    SECTION("get from tuple built with append")
    {
        auto exp = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(100)
            | fxt::mtuple_append(200)
            | fxt::mtuple_append(300);

        auto first = exp | fxt::mget<0>();
        auto second = exp | fxt::mget<1>();
        auto third = exp | fxt::mget<2>();

        REQUIRE(first.has_value());
        REQUIRE(*first == 100);
        REQUIRE(second.has_value());
        REQUIRE(*second == 200);
        REQUIRE(third.has_value());
        REQUIRE(*third == 300);
    }

    SECTION("get all elements from a larger tuple")
    {
        auto exp = fxt::expected<std::tuple<int, int, int, int, int>, std::string>{
            std::make_tuple(1, 2, 3, 4, 5)
        };

        REQUIRE(*( exp | fxt::mget<0>()) == 1);
        REQUIRE(*( exp | fxt::mget<1>()) == 2);
        REQUIRE(*( exp | fxt::mget<2>()) == 3);
        REQUIRE(*( exp | fxt::mget<3>()) == 4);
        REQUIRE(*( exp | fxt::mget<4>()) == 5);
    }
}

TEST_CASE("get - fxt::expected by type", "[get]")
{
    SECTION("get int element")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(99, 2.71, "world")
        };
        auto result = exp | fxt::mget<int>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 99);
    }

    SECTION("get double element")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(99, 2.71, "world")
        };
        auto result = exp | fxt::mget<double>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 2.71);
    }

    SECTION("get string element")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(99, 2.71, "world")
        };
        auto result = exp | fxt::mget<std::string>();

        REQUIRE(result.has_value());
        REQUIRE(*result == "world");
    }

    SECTION("get from tuple with custom types")
    {
        struct Point { int x, y; };
        auto exp = fxt::expected<std::tuple<Point, int>, std::string>{
            std::make_tuple(Point{10, 20}, 42)
        };

        auto point_result = exp | fxt::mget<Point>();
        auto int_result = exp | fxt::mget<int>();

        REQUIRE(point_result.has_value());
        REQUIRE(point_result->x == 10);
        REQUIRE(point_result->y == 20);
        REQUIRE(int_result.has_value());
        REQUIRE(*int_result == 42);
    }
}

TEST_CASE("get - fxt::expected error propagation", "[get]")
{
    SECTION("error propagates through get by index")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            fxt::unexpected("error occurred")
        };
        auto result = exp | fxt::mget<0>();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error occurred");
    }

    SECTION("error propagates through get by type")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            fxt::unexpected("type error")
        };
        auto result = exp | fxt::mget<std::string>();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "type error");
    }

    SECTION("error propagates through multiple get operations")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            fxt::unexpected("initial error")
        };

        auto r1 = exp | fxt::mget<0>();
        auto r2 = exp | fxt::mget<1>();
        auto r3 = exp | fxt::mget<2>();

        REQUIRE_FALSE(r1.has_value());
        REQUIRE_FALSE(r2.has_value());
        REQUIRE_FALSE(r3.has_value());
        REQUIRE(r1.error() == "initial error");
    }
}

TEST_CASE("get - fxt::optional by index", "[get]")
{
    SECTION("get element at index 0")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(77, 1.41, "optional")
        };
        auto result = opt | fxt::mget<0>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 77);
    }

    SECTION("get element at index 1")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(77, 1.41, "optional")
        };
        auto result = opt | fxt::mget<1>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 1.41);
    }

    SECTION("get element at index 2")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(77, 1.41, "optional")
        };
        auto result = opt | fxt::mget<2>();

        REQUIRE(result.has_value());
        REQUIRE(*result == "optional");
    }

    SECTION("get from tuple built with append")
    {
        auto opt = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(10)
            | fxt::mtuple_append(20)
            | fxt::mtuple_append(30);

        auto first = opt | fxt::mget<0>();
        auto second = opt | fxt::mget<1>();
        auto third = opt | fxt::mget<2>();

        REQUIRE(first.has_value());
        REQUIRE(*first == 10);
        REQUIRE(second.has_value());
        REQUIRE(*second == 20);
        REQUIRE(third.has_value());
        REQUIRE(*third == 30);
    }

    SECTION("get all elements from a larger tuple")
    {
        auto opt = fxt::optional<std::tuple<int, int, int, int, int>>{
            std::make_tuple(5, 4, 3, 2, 1)
        };

        REQUIRE(*( opt | fxt::mget<0>()) == 5);
        REQUIRE(*( opt | fxt::mget<1>()) == 4);
        REQUIRE(*( opt | fxt::mget<2>()) == 3);
        REQUIRE(*( opt | fxt::mget<3>()) == 2);
        REQUIRE(*( opt | fxt::mget<4>()) == 1);
    }
}

TEST_CASE("get - fxt::optional by type", "[get]")
{
    SECTION("get int element")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(55, 6.28, "type")
        };
        auto result = opt | fxt::mget<int>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 55);
    }

    SECTION("get double element")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(55, 6.28, "type")
        };
        auto result = opt | fxt::mget<double>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 6.28);
    }

    SECTION("get string element")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(55, 6.28, "type")
        };
        auto result = opt | fxt::mget<std::string>();

        REQUIRE(result.has_value());
        REQUIRE(*result == "type");
    }

    SECTION("get from tuple with bool")
    {
        auto opt = fxt::optional<std::tuple<bool, int, std::string>>{
            std::make_tuple(true, 42, "test")
        };

        auto bool_result = opt | fxt::mget<bool>();
        auto int_result = opt | fxt::mget<int>();
        auto str_result = opt | fxt::mget<std::string>();

        REQUIRE(bool_result.has_value());
        REQUIRE(*bool_result == true);
        REQUIRE(int_result.has_value());
        REQUIRE(*int_result == 42);
        REQUIRE(str_result.has_value());
        REQUIRE(*str_result == "test");
    }
}

TEST_CASE("get - fxt::optional empty propagation", "[get]")
{
    SECTION("empty propagates through get by index")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{};
        auto result = opt | fxt::mget<0>();

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("empty propagates through get by type")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{};
        auto result = opt | fxt::mget<std::string>();

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("empty propagates through multiple get operations")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{};

        auto r1 = opt | fxt::mget<0>();
        auto r2 = opt | fxt::mget<1>();
        auto r3 = opt | fxt::mget<2>();

        REQUIRE_FALSE(r1.has_value());
        REQUIRE_FALSE(r2.has_value());
        REQUIRE_FALSE(r3.has_value());
    }
}

TEST_CASE("get - direct call syntax", "[get]")
{
    SECTION("direct call with expected by index")
    {
        auto exp = fxt::expected<std::tuple<int, double>, std::string>{
            std::make_tuple(123, 4.56)
        };

        auto result = fxt::mget<0>()(exp);
        REQUIRE(result.has_value());
        REQUIRE(*result == 123);
    }

    SECTION("direct call with expected by type")
    {
        auto exp = fxt::expected<std::tuple<int, double>, std::string>{
            std::make_tuple(123, 4.56)
        };

        auto result = fxt::mget<double>()(exp);
        REQUIRE(result.has_value());
        REQUIRE(*result == 4.56);
    }

    SECTION("direct call with optional by index")
    {
        auto opt = fxt::optional<std::tuple<int, double>>{
            std::make_tuple(789, 1.23)
        };

        auto result = fxt::mget<0>()(opt);
        REQUIRE(result.has_value());
        REQUIRE(*result == 789);
    }

    SECTION("direct call with optional by type")
    {
        auto opt = fxt::optional<std::tuple<int, double>>{
            std::make_tuple(789, 1.23)
        };

        auto result = fxt::mget<double>()(opt);
        REQUIRE(result.has_value());
        REQUIRE(*result == 1.23);
    }
}

TEST_CASE("get - chaining with other operations", "[get]")
{
    SECTION("chain get with transform (expected)")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(10, 2.5, "chain")
        };

        auto result = exp
            | fxt::mget<0>()
            | fxt::transform([](int x) { return x * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("chain get with transform (optional)")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(15, 3.5, "chain")
        };

        auto result = opt
            | fxt::mget<0>()
            | fxt::transform([](int x) { return x + 5; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);
    }

    SECTION("multiple gets from same container")
    {
        auto exp = fxt::expected<std::tuple<int, int, int>, std::string>{
            std::make_tuple(1, 2, 3)
        };

        auto a = exp | fxt::mget<0>();
        auto b = exp | fxt::mget<1>();
        auto c = exp | fxt::mget<2>();

        REQUIRE(*a == 1);
        REQUIRE(*b == 2);
        REQUIRE(*c == 3);
    }
}

TEST_CASE("get - single element tuple", "[get]")
{
    SECTION("get from single element tuple (expected)")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::make_tuple(42)};
        auto result = exp | fxt::mget<0>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("get from single element tuple (optional)")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::make_tuple(99)};
        auto result = opt | fxt::mget<0>();

        REQUIRE(result.has_value());
        REQUIRE(*result == 99);
    }

    SECTION("get by type from single element tuple")
    {
        auto exp = fxt::expected<std::tuple<std::string>, int>{std::make_tuple("solo")};
        auto result = exp | fxt::mget<std::string>();

        REQUIRE(result.has_value());
        REQUIRE(*result == "solo");
    }
}

TEST_CASE("get - const correctness", "[get]")
{
    SECTION("get from const expected")
    {
        const auto exp = fxt::expected<std::tuple<int, double>, std::string>{
            std::make_tuple(100, 2.0)
        };

        auto result = exp | fxt::mget<0>();
        REQUIRE(result.has_value());
        REQUIRE(*result == 100);
    }

    SECTION("get from const optional")
    {
        const auto opt = fxt::optional<std::tuple<int, double>>{
            std::make_tuple(200, 3.0)
        };

        auto result = opt | fxt::mget<0>();
        REQUIRE(result.has_value());
        REQUIRE(*result == 200);
    }

    SECTION("get const reference type")
    {
        const std::string s = "const";
        auto opt = fxt::optional<std::tuple<std::string, int>>{
            std::make_tuple(s, 42)
        };

        auto result = opt | fxt::mget<std::string>();
        REQUIRE(result.has_value());
        REQUIRE(*result == "const");
    }
}

TEST_CASE("get - type deduction", "[get]")
{
    SECTION("auto deduction with expected")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(1, 2.0, "three")
        };

        auto result = exp | fxt::mget<0>();
        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::expected<int, std::string>>);
    }

    SECTION("auto deduction with optional")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(1, 2.0, "three")
        };

        auto result = opt | fxt::mget<0>();
        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::optional<int>>);
    }

    SECTION("type deduction by type")
    {
        auto exp = fxt::expected<std::tuple<int, std::string>, int>{
            std::make_tuple(42, "text")
        };

        auto result = exp | fxt::mget<std::string>();
        static_assert(std::is_same_v<decltype(result), fxt::expected<std::string, int>>);
    }
}

TEST_CASE("get - edge cases", "[get]")
{
    SECTION("get with zero value")
    {
        auto opt = fxt::optional<std::tuple<int, double>>{std::make_tuple(0, 0.0)};

        auto int_result = opt | fxt::mget<0>();
        auto double_result = opt | fxt::mget<1>();

        REQUIRE(int_result.has_value());
        REQUIRE(*int_result == 0);
        REQUIRE(double_result.has_value());
        REQUIRE(*double_result == 0.0);
    }

    SECTION("get with false value")
    {
        auto opt = fxt::optional<std::tuple<bool, int>>{std::make_tuple(false, 1)};
        auto result = opt | fxt::mget<0>();

        REQUIRE(result.has_value());
        REQUIRE(*result == false);
    }

    SECTION("get with empty string")
    {
        auto exp = fxt::expected<std::tuple<std::string, int>, int>{
            std::make_tuple("", 42)
        };
        auto result = exp | fxt::mget<std::string>();

        REQUIRE(result.has_value());
        REQUIRE(*result == "");
    }

    SECTION("get from tuple with identical types at different positions")
    {
        auto opt = fxt::optional<std::tuple<int, int, int>>{
            std::make_tuple(1, 2, 3)
        };

        // Can only get by index, not by type (ambiguous)
        REQUIRE(*( opt | fxt::mget<0>()) == 1);
        REQUIRE(*( opt | fxt::mget<1>()) == 2);
        REQUIRE(*( opt | fxt::mget<2>()) == 3);
    }
}

TEST_CASE("get - mixed types", "[get]")
{
    SECTION("tuple with various types")
    {
        auto opt = fxt::optional<std::tuple<std::string, int, bool, double>>{
            std::make_tuple("test", 42, true, 9.99)
        };

        REQUIRE(*( opt | fxt::mget<0>()) == "test");
        REQUIRE(*( opt | fxt::mget<1>()) == 42);
        REQUIRE(*( opt | fxt::mget<2>()) == true);
        REQUIRE(*( opt | fxt::mget<3>()) == 9.99);

        REQUIRE(*( opt | fxt::mget<std::string>()) == "test");
        REQUIRE(*( opt | fxt::mget<int>()) == 42);
        REQUIRE(*( opt | fxt::mget<bool>()) == true);
        REQUIRE(*( opt | fxt::mget<double>()) == 9.99);
    }
}

TEST_CASE("get - large tuples", "[get]")
{
    SECTION("tuple with 10 elements")
    {
        auto exp = fxt::expected<std::tuple<int, int, int, int, int, int, int, int, int, int>, std::string>{
            std::make_tuple(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
        };

        for (int i = 0; i < 10; ++i) {
            // We can't template this in a loop, but we can test a few
        }

        REQUIRE(*( exp | fxt::mget<0>()) == 0);
        REQUIRE(*( exp | fxt::mget<5>()) == 5);
        REQUIRE(*( exp | fxt::mget<9>()) == 9);
    }
}

