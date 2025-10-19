//
// Test suite for fxt::tuple_reverse and fxt::mtuple_reverse
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

// ============================================================================
// Tests for fxt::tuple (std::tuple) - Direct Call
// ============================================================================

TEST_CASE("tuple_reverse - fxt::tuple direct call", "[tuple_reverse][tuple][direct]")
{
    SECTION("reverse empty tuple")
    {
        auto t = fxt::tuple<>{};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("reverse single element tuple")
    {
        auto t = fxt::tuple<int>{42};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("reverse two element tuple")
    {
        auto t = fxt::tuple<int, double>{1, 2.5};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 2.5);
        REQUIRE(fxt::get<1>(result) == 1);
    }

    SECTION("reverse three element tuple")
    {
        auto t = fxt::tuple<int, double, std::string>{42, 3.14, std::string{"hello"}};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == "hello");
        REQUIRE(fxt::get<1>(result) == 3.14);
        REQUIRE(fxt::get<2>(result) == 42);
    }

    SECTION("reverse four element tuple with mixed types")
    {
        auto t = fxt::tuple<int, std::string, double, bool>{1, std::string{"test"}, 2.5, true};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == true);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == "test");
        REQUIRE(fxt::get<3>(result) == 1);
    }

    SECTION("reverse five element tuple")
    {
        auto t = fxt::tuple<int, int, int, int, int>{1, 2, 3, 4, 5};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 5);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 2);
        REQUIRE(fxt::get<4>(result) == 1);
    }

    SECTION("double reverse returns to original")
    {
        auto t = fxt::tuple<int, double, std::string>{1, 2.5, std::string{"test"}};
        auto reversed_once = fxt::tuple_reverse(t);
        auto reversed_twice = fxt::tuple_reverse(reversed_once);

        REQUIRE(fxt::tuple_size_v<decltype(reversed_twice)> == 3);
        REQUIRE(fxt::get<0>(reversed_twice) == 1);
        REQUIRE(fxt::get<1>(reversed_twice) == 2.5);
        REQUIRE(fxt::get<2>(reversed_twice) == "test");
    }
}

// ============================================================================
// Tests for fxt::tuple (std::tuple) - Pipe Operator
// ============================================================================

TEST_CASE("tuple_reverse - fxt::tuple pipe operator", "[tuple_reverse][tuple][pipe]")
{
    SECTION("reverse with pipe operator")
    {
        auto t = fxt::tuple<int, double, std::string>{1, 2.5, std::string{"hello"}};
        auto result = t | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == "hello");
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == 1);
    }

    SECTION("reverse single element with pipe")
    {
        auto t = fxt::tuple<int>{42};
        auto result = t | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("double reverse with pipe")
    {
        auto t = fxt::tuple<int, int, int>{1, 2, 3};
        auto result = t | fxt::tuple_reverse() | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("reverse on rvalue")
    {
        auto result = fxt::tuple<int, double>{10, 20.5} | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 20.5);
        REQUIRE(fxt::get<1>(result) == 10);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Direct Call
// ============================================================================

TEST_CASE("tuple_reverse - fxt::flat_tuple direct call", "[tuple_reverse][flat_tuple][direct]")
{
    SECTION("reverse empty flat_tuple")
    {
        auto t = fxt::flat_tuple<>{};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 0);
    }

    SECTION("reverse single element flat_tuple")
    {
        auto t = fxt::flat_tuple<int>{42};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("reverse two element flat_tuple")
    {
        auto t = fxt::flat_tuple<double, double>{1.5, 2.5};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 2.5);
        REQUIRE(fxt::get<1>(result) == 1.5);
    }

    SECTION("reverse three element flat_tuple")
    {
        auto t = fxt::flat_tuple<int, int, int>{10, 20, 30};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 30);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 10);
    }

    SECTION("reverse five element flat_tuple")
    {
        auto t = fxt::flat_tuple<double, double, double, double, double>{1.0, 2.0, 3.0, 4.0, 5.0};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 5.0);
        REQUIRE(fxt::get<1>(result) == 4.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
        REQUIRE(fxt::get<3>(result) == 2.0);
        REQUIRE(fxt::get<4>(result) == 1.0);
    }

    SECTION("double reverse flat_tuple returns to original")
    {
        auto t = fxt::flat_tuple<int, int, int>{100, 200, 300};
        auto reversed_once = fxt::tuple_reverse(t);
        auto reversed_twice = fxt::tuple_reverse(reversed_once);

        REQUIRE(fxt::tuple_size_v<decltype(reversed_twice)> == 3);
        REQUIRE(fxt::get<0>(reversed_twice) == 100);
        REQUIRE(fxt::get<1>(reversed_twice) == 200);
        REQUIRE(fxt::get<2>(reversed_twice) == 300);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Pipe Operator
// ============================================================================

TEST_CASE("tuple_reverse - fxt::flat_tuple pipe operator", "[tuple_reverse][flat_tuple][pipe]")
{
    SECTION("reverse with pipe operator")
    {
        auto t = fxt::flat_tuple<int, int, int>{1, 2, 3};
        auto result = t | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 1);
    }

    SECTION("reverse single element flat_tuple with pipe")
    {
        auto t = fxt::flat_tuple<double>{3.14};
        auto result = t | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 3.14);
    }

    SECTION("double reverse flat_tuple with pipe")
    {
        auto t = fxt::flat_tuple<int, int, int, int>{10, 20, 30, 40};
        auto result = t | fxt::tuple_reverse() | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 30);
        REQUIRE(fxt::get<3>(result) == 40);
    }

    SECTION("reverse flat_tuple on rvalue")
    {
        auto result = fxt::flat_tuple<double, double>{5.5, 6.6} | fxt::tuple_reverse();

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 6.6);
        REQUIRE(fxt::get<1>(result) == 5.5);
    }
}

// ============================================================================
// Tests for fxt::mtuple_reverse with fxt::expected - Direct Call
// ============================================================================

TEST_CASE("mtuple_reverse - fxt::expected with fxt::tuple direct call", "[mtuple_reverse][expected][tuple][direct]")
{
    SECTION("reverse tuple inside expected with value")
    {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::tuple<int, int, int>{1, 2, 3}};
        auto result = fxt::mtuple_reverse(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3);
        REQUIRE(fxt::get<1>(*result) == 2);
        REQUIRE(fxt::get<2>(*result) == 1);
    }

    SECTION("reverse single element tuple inside expected")
    {
        auto exp = fxt::expected<fxt::tuple<int>, std::string>{fxt::tuple<int>{42}};
        auto result = fxt::mtuple_reverse(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 42);
    }

    SECTION("reverse tuple with mixed types inside expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, double, std::string>, std::string>{
            fxt::tuple<int, double, std::string>{10, 2.5, std::string{"test"}}
        };
        auto result = fxt::mtuple_reverse(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == "test");
        REQUIRE(fxt::get<1>(*result) == 2.5);
        REQUIRE(fxt::get<2>(*result) == 10);
    }

    SECTION("error propagation - reverse expected with error")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected{std::string{"error"}}};
        auto result = fxt::mtuple_reverse(exp);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("double reverse inside expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::tuple<int, int, int>{1, 2, 3}};
        auto reversed_once = fxt::mtuple_reverse(exp);
        auto reversed_twice = fxt::mtuple_reverse(reversed_once);

        REQUIRE(reversed_twice.has_value());
        REQUIRE(fxt::get<0>(*reversed_twice) == 1);
        REQUIRE(fxt::get<1>(*reversed_twice) == 2);
        REQUIRE(fxt::get<2>(*reversed_twice) == 3);
    }
}

// ============================================================================
// Tests for fxt::mtuple_reverse with fxt::expected - Pipe Operator
// ============================================================================

TEST_CASE("mtuple_reverse - fxt::expected with fxt::tuple pipe operator", "[mtuple_reverse][expected][tuple][pipe]")
{
    SECTION("reverse tuple inside expected with pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::tuple<int, int, int>{10, 20, 30}};
        auto result = exp | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 30);
        REQUIRE(fxt::get<1>(*result) == 20);
        REQUIRE(fxt::get<2>(*result) == 10);
    }

    SECTION("reverse single element with pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int>, std::string>{fxt::tuple<int>{99}};
        auto result = exp | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 99);
    }

    SECTION("error propagation with pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected{std::string{"error"}}};
        auto result = exp | fxt::mtuple_reverse();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("double reverse with pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int, int, int, int>, std::string>{
            fxt::tuple<int, int, int, int>{1, 2, 3, 4}
        };
        auto result = exp | fxt::mtuple_reverse() | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 1);
        REQUIRE(fxt::get<1>(*result) == 2);
        REQUIRE(fxt::get<2>(*result) == 3);
        REQUIRE(fxt::get<3>(*result) == 4);
    }

    SECTION("reverse on rvalue expected")
    {
        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple<int, int>{5, 10}}
            | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10);
        REQUIRE(fxt::get<1>(*result) == 5);
    }
}

// ============================================================================
// Tests for fxt::mtuple_reverse with fxt::optional - Direct Call
// ============================================================================

TEST_CASE("mtuple_reverse - fxt::optional with fxt::tuple direct call", "[mtuple_reverse][optional][tuple][direct]")
{
    SECTION("reverse tuple inside optional with value")
    {
        auto opt = fxt::optional<fxt::tuple<int, int, int>>{fxt::tuple<int, int, int>{1, 2, 3}};
        auto result = fxt::mtuple_reverse(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3);
        REQUIRE(fxt::get<1>(*result) == 2);
        REQUIRE(fxt::get<2>(*result) == 1);
    }

    SECTION("reverse single element tuple inside optional")
    {
        auto opt = fxt::optional<fxt::tuple<int>>{fxt::tuple<int>{42}};
        auto result = fxt::mtuple_reverse(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 42);
    }

    SECTION("nullopt propagation - reverse optional without value")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{};
        auto result = fxt::mtuple_reverse(opt);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("reverse tuple with mixed types inside optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, double, std::string>>{
            fxt::tuple<int, double, std::string>{100, 3.14, std::string{"hello"}}
        };
        auto result = fxt::mtuple_reverse(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == "hello");
        REQUIRE(fxt::get<1>(*result) == 3.14);
        REQUIRE(fxt::get<2>(*result) == 100);
    }

    SECTION("double reverse inside optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int, int>>{fxt::tuple<int, int, int>{7, 8, 9}};
        auto reversed_once = fxt::mtuple_reverse(opt);
        auto reversed_twice = fxt::mtuple_reverse(reversed_once);

        REQUIRE(reversed_twice.has_value());
        REQUIRE(fxt::get<0>(*reversed_twice) == 7);
        REQUIRE(fxt::get<1>(*reversed_twice) == 8);
        REQUIRE(fxt::get<2>(*reversed_twice) == 9);
    }
}

// ============================================================================
// Tests for fxt::mtuple_reverse with fxt::optional - Pipe Operator
// ============================================================================

TEST_CASE("mtuple_reverse - fxt::optional with fxt::tuple pipe operator", "[mtuple_reverse][optional][tuple][pipe]")
{
    SECTION("reverse tuple inside optional with pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int, int, int>>{fxt::tuple<int, int, int>{10, 20, 30}};
        auto result = opt | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 30);
        REQUIRE(fxt::get<1>(*result) == 20);
        REQUIRE(fxt::get<2>(*result) == 10);
    }

    SECTION("reverse single element with pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int>>{fxt::tuple<int>{77}};
        auto result = opt | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 77);
    }

    SECTION("nullopt propagation with pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{};
        auto result = opt | fxt::mtuple_reverse();

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("double reverse with pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int, int, int, int>>{
            fxt::tuple<int, int, int, int>{1, 2, 3, 4}
        };
        auto result = opt | fxt::mtuple_reverse() | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 1);
        REQUIRE(fxt::get<1>(*result) == 2);
        REQUIRE(fxt::get<2>(*result) == 3);
        REQUIRE(fxt::get<3>(*result) == 4);
    }

    SECTION("reverse on rvalue optional")
    {
        auto result = fxt::optional<fxt::tuple<int, int>>{fxt::tuple<int, int>{15, 25}}
            | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 25);
        REQUIRE(fxt::get<1>(*result) == 15);
    }
}

// ============================================================================
// Tests for fxt::mtuple_reverse with fxt::expected and fxt::flat_tuple
// ============================================================================

TEST_CASE("mtuple_reverse - fxt::expected with fxt::flat_tuple", "[mtuple_reverse][expected][flat_tuple]")
{
    SECTION("reverse flat_tuple inside expected - direct call")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int>, std::string>{
            fxt::flat_tuple<int, int, int>{1, 2, 3}
        };
        auto result = fxt::mtuple_reverse(exp);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3);
        REQUIRE(fxt::get<1>(*result) == 2);
        REQUIRE(fxt::get<2>(*result) == 1);
    }

    SECTION("reverse flat_tuple inside expected - pipe operator")
    {
        auto exp = fxt::expected<fxt::flat_tuple<double, double, double>, std::string>{
            fxt::flat_tuple<double, double, double>{1.1, 2.2, 3.3}
        };
        auto result = exp | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3.3);
        REQUIRE(fxt::get<1>(*result) == 2.2);
        REQUIRE(fxt::get<2>(*result) == 1.1);
    }

    SECTION("error propagation with flat_tuple")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, std::string>{
            fxt::unexpected{std::string{"flat_tuple error"}}
        };
        auto result = exp | fxt::mtuple_reverse();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "flat_tuple error");
    }

    SECTION("reverse five element flat_tuple inside expected")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int, int, int, int>, std::string>{
            fxt::flat_tuple<int, int, int, int, int>{10, 20, 30, 40, 50}
        };
        auto result = exp | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 50);
        REQUIRE(fxt::get<1>(*result) == 40);
        REQUIRE(fxt::get<2>(*result) == 30);
        REQUIRE(fxt::get<3>(*result) == 20);
        REQUIRE(fxt::get<4>(*result) == 10);
    }
}

// ============================================================================
// Tests for fxt::mtuple_reverse with fxt::optional and fxt::flat_tuple
// ============================================================================

TEST_CASE("mtuple_reverse - fxt::optional with fxt::flat_tuple", "[mtuple_reverse][optional][flat_tuple]")
{
    SECTION("reverse flat_tuple inside optional - direct call")
    {
        auto opt = fxt::optional<fxt::flat_tuple<int, int, int>>{
            fxt::flat_tuple<int, int, int>{5, 10, 15}
        };
        auto result = fxt::mtuple_reverse(opt);

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 15);
        REQUIRE(fxt::get<1>(*result) == 10);
        REQUIRE(fxt::get<2>(*result) == 5);
    }

    SECTION("reverse flat_tuple inside optional - pipe operator")
    {
        auto opt = fxt::optional<fxt::flat_tuple<double, double, double, double>>{
            fxt::flat_tuple<double, double, double, double>{1.0, 2.0, 3.0, 4.0}
        };
        auto result = opt | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 4.0);
        REQUIRE(fxt::get<1>(*result) == 3.0);
        REQUIRE(fxt::get<2>(*result) == 2.0);
        REQUIRE(fxt::get<3>(*result) == 1.0);
    }

    SECTION("nullopt propagation with flat_tuple")
    {
        auto opt = fxt::optional<fxt::flat_tuple<int, int>>{};
        auto result = opt | fxt::mtuple_reverse();

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("reverse single element flat_tuple inside optional")
    {
        auto opt = fxt::optional<fxt::flat_tuple<int>>{fxt::flat_tuple<int>{999}};
        auto result = opt | fxt::mtuple_reverse();

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 999);
    }
}

// ============================================================================
// Type Verification Tests
// ============================================================================

TEST_CASE("tuple_reverse - type preservation", "[tuple_reverse][types]")
{
    SECTION("fxt::tuple remains fxt::tuple")
    {
        auto t = fxt::tuple<int, double>{1, 2.5};
        auto result = fxt::tuple_reverse(t);

        static_assert(fxt::impl::is_fxt_tuple_v<decltype(result)>, "Result should be fxt::tuple");
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
    }

    SECTION("fxt::flat_tuple remains fxt::flat_tuple")
    {
        auto ft = fxt::flat_tuple<int, int>{1, 2};
        auto result = fxt::tuple_reverse(ft);

        static_assert(fxt::impl::is_flat_tuple_v<decltype(result)>, "Result should be fxt::flat_tuple");
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
    }

    SECTION("element types are reversed in type")
    {
        auto t = fxt::tuple<int, double, std::string>{1, 2.5, std::string{"test"}};
        auto result = fxt::tuple_reverse(t);

        // Check that the types are in reverse order
        static_assert(std::is_same_v<decltype(fxt::get<0>(result)), std::string&>, "First element should be string");
        static_assert(std::is_same_v<decltype(fxt::get<1>(result)), double&>, "Second element should be double");
        static_assert(std::is_same_v<decltype(fxt::get<2>(result)), int&>, "Third element should be int");
    }
}

// ============================================================================
// Edge Cases and Special Scenarios
// ============================================================================

TEST_CASE("tuple_reverse - edge cases", "[tuple_reverse][edge_cases]")
{
    SECTION("reverse palindromic tuple")
    {
        auto t = fxt::tuple<int, double, int>{1, 2.5, 1};
        auto result = fxt::tuple_reverse(t);

        // Values should be reversed but structure is palindromic
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == 1);
    }

    SECTION("reverse with const references")
    {
        const auto t = fxt::tuple<int, int, int>{1, 2, 3};
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 1);
    }

    SECTION("reverse large tuple")
    {
        auto t = fxt::tuple<int, int, int, int, int, int, int, int, int, int>{
            1, 2, 3, 4, 5, 6, 7, 8, 9, 10
        };
        auto result = fxt::tuple_reverse(t);

        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<4>(result) == 6);
        REQUIRE(fxt::get<9>(result) == 1);
    }
}
