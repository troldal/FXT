//
// Test suite for fxt::select
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

TEST_CASE("select - fxt::expected by index", "[select]")
{
    SECTION("select two elements")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(42, 3.14, "hello")
        };
        auto result = exp | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        auto [i, s] = *result;
        REQUIRE(i == 42);
        REQUIRE(s == "hello");
    }

    SECTION("select three elements")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string, bool>, std::string>{
            std::make_tuple(99, 2.71, "world", true)
        };
        auto result = exp | fxt::mselect<0, 1, 3>();

        REQUIRE(result.has_value());
        auto [i, d, b] = *result;
        REQUIRE(i == 99);
        REQUIRE(d == 2.71);
        REQUIRE(b == true);
    }

    SECTION("select single element")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(77, 1.41, "single")
        };
        auto result = exp | fxt::mselect<1>();

        REQUIRE(result.has_value());
        auto [d] = *result;
        REQUIRE(d == 1.41);
    }

    SECTION("select in different order")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(1, 2.0, "three")
        };
        auto result = exp | fxt::mselect<2, 0>();

        REQUIRE(result.has_value());
        auto [s, i] = *result;
        REQUIRE(s == "three");
        REQUIRE(i == 1);
    }

    SECTION("select from larger tuple")
    {
        auto exp = fxt::expected<std::tuple<int, int, int, int, int>, std::string>{
            std::make_tuple(1, 2, 3, 4, 5)
        };
        auto result = exp | fxt::mselect<0, 2, 4>();

        REQUIRE(result.has_value());
        auto [a, c, e] = *result;
        REQUIRE(a == 1);
        REQUIRE(c == 3);
        REQUIRE(e == 5);
    }

    SECTION("select with duplicate indices")
    {
        auto exp = fxt::expected<std::tuple<int, int, int>, std::string>{
            std::make_tuple(10, 20, 30)
        };
        auto result = exp | fxt::mselect<0, 0, 1>();

        REQUIRE(result.has_value());
        auto [a, b, c] = *result;
        REQUIRE(a == 10);
        REQUIRE(b == 10);
        REQUIRE(c == 20);
    }
}

TEST_CASE("select - fxt::expected by type", "[select]")
{
    SECTION("select two types")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(42, 3.14, "hello")
        };
        auto result = exp | fxt::mselect<int, std::string>();

        REQUIRE(result.has_value());
        auto [i, s] = *result;
        REQUIRE(i == 42);
        REQUIRE(s == "hello");
    }

    SECTION("select three types")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string, bool>, std::string>{
            std::make_tuple(99, 2.71, "world", false)
        };
        auto result = exp | fxt::mselect<double, bool, int>();

        REQUIRE(result.has_value());
        auto [d, b, i] = *result;
        REQUIRE(d == 2.71);
        REQUIRE(b == false);
        REQUIRE(i == 99);
    }

    SECTION("select single type")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(77, 1.41, "single")
        };
        auto result = exp | fxt::mselect<std::string>();

        REQUIRE(result.has_value());
        auto [s] = *result;
        REQUIRE(s == "single");
    }

    SECTION("select custom types")
    {
        struct Point { int x, y; };
        auto exp = fxt::expected<std::tuple<Point, int, double>, std::string>{
            std::make_tuple(Point{10, 20}, 42, 3.14)
        };
        auto result = exp | fxt::mselect<Point, double>();

        REQUIRE(result.has_value());
        auto [p, d] = *result;
        REQUIRE(p.x == 10);
        REQUIRE(p.y == 20);
        REQUIRE(d == 3.14);
    }
}

TEST_CASE("select - fxt::expected error propagation", "[select]")
{
    SECTION("error propagates through select by index")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            fxt::unexpected("error occurred")
        };
        auto result = exp | fxt::mselect<0, 2>();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error occurred");
    }

    SECTION("error propagates through select by type")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            fxt::unexpected("type error")
        };
        auto result = exp | fxt::mselect<int, std::string>();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "type error");
    }
}

TEST_CASE("select - fxt::optional by index", "[select]")
{
    SECTION("select two elements")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(55, 6.28, "optional")
        };
        auto result = opt | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        auto [i, s] = *result;
        REQUIRE(i == 55);
        REQUIRE(s == "optional");
    }

    SECTION("select three elements")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string, bool>>{
            std::make_tuple(11, 2.22, "test", true)
        };
        auto result = opt | fxt::mselect<1, 2, 3>();

        REQUIRE(result.has_value());
        auto [d, s, b] = *result;
        REQUIRE(d == 2.22);
        REQUIRE(s == "test");
        REQUIRE(b == true);
    }

    SECTION("select single element")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(88, 4.44, "one")
        };
        auto result = opt | fxt::mselect<2>();

        REQUIRE(result.has_value());
        auto [s] = *result;
        REQUIRE(s == "one");
    }

    SECTION("select in reverse order")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(1, 2.0, "three")
        };
        auto result = opt | fxt::mselect<2, 1, 0>();

        REQUIRE(result.has_value());
        auto [s, d, i] = *result;
        REQUIRE(s == "three");
        REQUIRE(d == 2.0);
        REQUIRE(i == 1);
    }

    SECTION("select from larger tuple")
    {
        auto opt = fxt::optional<std::tuple<int, int, int, int, int>>{
            std::make_tuple(5, 4, 3, 2, 1)
        };
        auto result = opt | fxt::mselect<4, 2, 0>();

        REQUIRE(result.has_value());
        auto [a, c, e] = *result;
        REQUIRE(a == 1);
        REQUIRE(c == 3);
        REQUIRE(e == 5);
    }

    SECTION("select with duplicate indices")
    {
        auto opt = fxt::optional<std::tuple<int, int, int>>{
            std::make_tuple(100, 200, 300)
        };
        auto result = opt | fxt::mselect<1, 1, 1>();

        REQUIRE(result.has_value());
        auto [a, b, c] = *result;
        REQUIRE(a == 200);
        REQUIRE(b == 200);
        REQUIRE(c == 200);
    }
}

TEST_CASE("select - fxt::optional by type", "[select]")
{
    SECTION("select two types")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(123, 4.56, "type")
        };
        auto result = opt | fxt::mselect<double, int>();

        REQUIRE(result.has_value());
        auto [d, i] = *result;
        REQUIRE(d == 4.56);
        REQUIRE(i == 123);
    }

    SECTION("select three types")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string, bool>>{
            std::make_tuple(33, 5.55, "multi", false)
        };
        auto result = opt | fxt::mselect<std::string, bool, double>();

        REQUIRE(result.has_value());
        auto [s, b, d] = *result;
        REQUIRE(s == "multi");
        REQUIRE(b == false);
        REQUIRE(d == 5.55);
    }

    SECTION("select single type")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(999, 7.77, "solo")
        };
        auto result = opt | fxt::mselect<int>();

        REQUIRE(result.has_value());
        auto [i] = *result;
        REQUIRE(i == 999);
    }

    SECTION("select with bool")
    {
        auto opt = fxt::optional<std::tuple<bool, int, std::string>>{
            std::make_tuple(true, 42, "bool")
        };
        auto result = opt | fxt::mselect<bool, std::string>();

        REQUIRE(result.has_value());
        auto [b, s] = *result;
        REQUIRE(b == true);
        REQUIRE(s == "bool");
    }
}

TEST_CASE("select - fxt::optional empty propagation", "[select]")
{
    SECTION("empty propagates through select by index")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{};
        auto result = opt | fxt::mselect<0, 2>();

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("empty propagates through select by type")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{};
        auto result = opt | fxt::mselect<int, std::string>();

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("select - direct call syntax", "[select]")
{
    SECTION("direct call with expected by index")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(11, 2.2, "direct")
        };
        auto result = fxt::mselect<0, 2>()(exp);

        REQUIRE(result.has_value());
        auto [i, s] = *result;
        REQUIRE(i == 11);
        REQUIRE(s == "direct");
    }

    SECTION("direct call with expected by type")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(22, 3.3, "call")
        };
        auto result = fxt::mselect<double, int>()(exp);

        REQUIRE(result.has_value());
        auto [d, i] = *result;
        REQUIRE(d == 3.3);
        REQUIRE(i == 22);
    }

    SECTION("direct call with optional by index")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(33, 4.4, "opt")
        };
        auto result = fxt::mselect<1, 2>()(opt);

        REQUIRE(result.has_value());
        auto [d, s] = *result;
        REQUIRE(d == 4.4);
        REQUIRE(s == "opt");
    }

    SECTION("direct call with optional by type")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(44, 5.5, "syntax")
        };
        auto result = fxt::mselect<std::string, int>()(opt);

        REQUIRE(result.has_value());
        auto [s, i] = *result;
        REQUIRE(s == "syntax");
        REQUIRE(i == 44);
    }
}

TEST_CASE("select - chaining operations", "[select]")
{
    SECTION("chain two select operations (expected)")
    {
        auto exp = fxt::expected<std::tuple<int, int, int, int, int>, std::string>{
            std::make_tuple(1, 2, 3, 4, 5)
        };

        auto first = exp | fxt::mselect<1, 2, 3>();
        auto second = first | fxt::mselect<0, 2>();

        REQUIRE(second.has_value());
        auto [a, c] = *second;
        REQUIRE(a == 2);
        REQUIRE(c == 4);
    }

    SECTION("chain two select operations (optional)")
    {
        auto opt = fxt::optional<std::tuple<int, int, int, int, int>>{
            std::make_tuple(10, 20, 30, 40, 50)
        };

        auto first = opt | fxt::mselect<0, 2, 4>();
        auto second = first | fxt::mselect<1, 2>();

        REQUIRE(second.has_value());
        auto [b, c] = *second;
        REQUIRE(b == 30);
        REQUIRE(c == 50);
    }
}

TEST_CASE("select - with append", "[select]")
{
    SECTION("build tuple with append and select (expected)")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mappend(100)
            | fxt::mappend(200)
            | fxt::mappend(300)
            | fxt::mappend(400)
            | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        auto [a, c] = *result;
        REQUIRE(a == 100);
        REQUIRE(c == 300);
    }

    SECTION("build tuple with append and select (optional)")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mappend(10)
            | fxt::mappend(20)
            | fxt::mappend(30)
            | fxt::mselect<1, 2>();

        REQUIRE(result.has_value());
        auto [b, c] = *result;
        REQUIRE(b == 20);
        REQUIRE(c == 30);
    }
}

TEST_CASE("select - const correctness", "[select]")
{
    SECTION("select from const expected")
    {
        const auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(42, 3.14, "const")
        };
        auto result = exp | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        auto [i, s] = *result;
        REQUIRE(i == 42);
        REQUIRE(s == "const");
    }

    SECTION("select from const optional")
    {
        const auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(99, 2.71, "const_opt")
        };
        auto result = opt | fxt::mselect<1, 2>();

        REQUIRE(result.has_value());
        auto [d, s] = *result;
        REQUIRE(d == 2.71);
        REQUIRE(s == "const_opt");
    }
}

TEST_CASE("select - type deduction", "[select]")
{
    SECTION("auto deduction with expected by index")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, std::string>{
            std::make_tuple(1, 2.0, "three")
        };
        auto result = exp | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::expected<std::tuple<int, std::string>, std::string>>);
    }

    SECTION("auto deduction with optional by index")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(1, 2.0, "three")
        };
        auto result = opt | fxt::mselect<1, 2>();

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::optional<std::tuple<double, std::string>>>);
    }

    SECTION("auto deduction with type selection")
    {
        auto exp = fxt::expected<std::tuple<int, double, std::string>, int>{
            std::make_tuple(1, 2.0, "three")
        };
        auto result = exp | fxt::mselect<std::string, int>();

        static_assert(std::is_same_v<decltype(result), fxt::expected<std::tuple<std::string, int>, int>>);
    }
}

TEST_CASE("select - edge cases", "[select]")
{
    SECTION("select with zero values")
    {
        auto opt = fxt::optional<std::tuple<int, double, int>>{
            std::make_tuple(0, 0.0, 1)
        };
        auto result = opt | fxt::mselect<0, 1>();

        REQUIRE(result.has_value());
        auto [i, d] = *result;
        REQUIRE(i == 0);
        REQUIRE(d == 0.0);
    }

    SECTION("select with false value")
    {
        auto opt = fxt::optional<std::tuple<bool, int, bool>>{
            std::make_tuple(false, 42, true)
        };
        auto result = opt | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        auto [b1, b2] = *result;
        REQUIRE(b1 == false);
        REQUIRE(b2 == true);
    }

    SECTION("select with empty string")
    {
        auto exp = fxt::expected<std::tuple<std::string, int, std::string>, int>{
            std::make_tuple("", 42, "nonempty")
        };
        auto result = exp | fxt::mselect<0, 2>();

        REQUIRE(result.has_value());
        auto [s1, s2] = *result;
        REQUIRE(s1 == "");
        REQUIRE(s2 == "nonempty");
    }

    SECTION("select all elements in same order")
    {
        auto opt = fxt::optional<std::tuple<int, double, std::string>>{
            std::make_tuple(1, 2.0, "three")
        };
        auto result = opt | fxt::mselect<0, 1, 2>();

        REQUIRE(result.has_value());
        auto [i, d, s] = *result;
        REQUIRE(i == 1);
        REQUIRE(d == 2.0);
        REQUIRE(s == "three");
    }
}

TEST_CASE("select - large tuples", "[select]")
{
    SECTION("select from 10-element tuple")
    {
        auto exp = fxt::expected<std::tuple<int, int, int, int, int, int, int, int, int, int>, std::string>{
            std::make_tuple(0, 1, 2, 3, 4, 5, 6, 7, 8, 9)
        };
        auto result = exp | fxt::mselect<0, 3, 6, 9>();

        REQUIRE(result.has_value());
        auto [a, d, g, j] = *result;
        REQUIRE(a == 0);
        REQUIRE(d == 3);
        REQUIRE(g == 6);
        REQUIRE(j == 9);
    }
}

TEST_CASE("select - mixed element selection", "[select]")
{
    SECTION("select various types in different order")
    {
        auto opt = fxt::optional<std::tuple<std::string, int, bool, double, char>>{
            std::make_tuple("hello", 42, true, 3.14, 'X')
        };
        auto result = opt | fxt::mselect<4, 1, 0>();

        REQUIRE(result.has_value());
        auto [c, i, s] = *result;
        REQUIRE(c == 'X');
        REQUIRE(i == 42);
        REQUIRE(s == "hello");
    }
}

