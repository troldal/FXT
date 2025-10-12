// filepath: d:\Dev\XLThermo\FXT\tests\MonadicTupleAppendTests.cpp
//
// Test suite for fxt::mtuple_append
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

// ============================================================================
// Tests for fxt::expected with fxt::tuple - Plain values
// ============================================================================

TEST_CASE("mtuple_append - expected<tuple> with plain value", "[mtuple_append][expected][tuple][direct]")
{
    struct Error { int code; };

    SECTION("append single int to expected<tuple>")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto result = fxt::mtuple_append(exp, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("append multiple values to expected<tuple>")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto result = fxt::mtuple_append(exp, 2, 3, 4);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
    }

    SECTION("error propagation with plain value")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{42}}};
        auto result = fxt::mtuple_append(exp, 3);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 42);
    }
}

// ============================================================================
// Tests for fxt::expected with fxt::tuple - Monadic values
// ============================================================================

TEST_CASE("mtuple_append - expected<tuple> with expected<value>", "[mtuple_append][expected][tuple][monadic]")
{
    struct Error { int code; };

    SECTION("append expected<int> to expected<tuple> - both valid")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto exp_value = fxt::expected<int, Error>{3};
        auto result = fxt::mtuple_append(exp_tuple, exp_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("append expected<int> to expected<tuple> - tuple has error")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{10}}};
        auto exp_value = fxt::expected<int, Error>{3};
        auto result = fxt::mtuple_append(exp_tuple, exp_value);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 10);
    }

    SECTION("append expected<int> to expected<tuple> - value has error")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto exp_value = fxt::expected<int, Error>{fxt::unexpected{Error{20}}};
        auto result = fxt::mtuple_append(exp_tuple, exp_value);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 20);
    }

    SECTION("append multiple expected values - all valid")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto exp2 = fxt::expected<int, Error>{2};
        auto exp3 = fxt::expected<int, Error>{3};
        auto result = fxt::mtuple_append(exp_tuple, exp2, exp3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("append multiple expected values - second has error")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto exp2 = fxt::expected<int, Error>{2};
        auto exp3 = fxt::expected<int, Error>{fxt::unexpected{Error{30}}};
        auto result = fxt::mtuple_append(exp_tuple, exp2, exp3);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 30);
    }
}

// ============================================================================
// Tests for fxt::optional with fxt::tuple - Plain values
// ============================================================================

TEST_CASE("mtuple_append - optional<tuple> with plain value", "[mtuple_append][optional][tuple][direct]")
{
    SECTION("append single int to optional<tuple>")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
        auto result = fxt::mtuple_append(opt, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("append multiple values to optional<tuple>")
    {
        auto opt = fxt::optional<fxt::tuple<std::string>>{fxt::make_tuple(std::string{"hello"})};
        auto result = fxt::mtuple_append(opt, 42, 3.14);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == "hello");
        REQUIRE(fxt::get<1>(t) == 42);
        REQUIRE(fxt::get<2>(t) == 3.14);
    }

    SECTION("nullopt propagation with plain value")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::nullopt};
        auto result = fxt::mtuple_append(opt, 3);

        REQUIRE_FALSE(result.has_value());
    }
}

// ============================================================================
// Tests for fxt::optional with fxt::tuple - Monadic values
// ============================================================================

TEST_CASE("mtuple_append - optional<tuple> with optional<value>", "[mtuple_append][optional][tuple][monadic]")
{
    SECTION("append optional<int> to optional<tuple> - both valid")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
        auto opt_value = fxt::optional<int>{3};
        auto result = fxt::mtuple_append(opt_tuple, opt_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("append optional<int> to optional<tuple> - tuple is nullopt")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{fxt::nullopt};
        auto opt_value = fxt::optional<int>{3};
        auto result = fxt::mtuple_append(opt_tuple, opt_value);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("append optional<int> to optional<tuple> - value is nullopt")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
        auto opt_value = fxt::optional<int>{fxt::nullopt};
        auto result = fxt::mtuple_append(opt_tuple, opt_value);

        REQUIRE_FALSE(result.has_value());
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Plain values
// ============================================================================

TEST_CASE("mtuple_append - expected<flat_tuple> with plain value", "[mtuple_append][expected][flat_tuple][direct]")
{
    struct Error { int code; };

    SECTION("append double to expected<flat_tuple>")
    {
        auto exp = fxt::expected<fxt::flat_tuple<double, double>, Error>{fxt::make_flat_tuple(1.0, 2.0)};
        auto result = fxt::mtuple_append(exp, 3.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
    }

    SECTION("append multiple values to expected<flat_tuple>")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int>, Error>{fxt::make_flat_tuple(1)};
        auto result = fxt::mtuple_append(exp, 2, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }
}

TEST_CASE("mtuple_append - optional<flat_tuple> with plain value", "[mtuple_append][optional][flat_tuple][direct]")
{
    SECTION("append double to optional<flat_tuple>")
    {
        auto opt = fxt::optional<fxt::flat_tuple<double, double>>{fxt::make_flat_tuple(1.0, 2.0)};
        auto result = fxt::mtuple_append(opt, 3.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
    }

    SECTION("append multiple values to optional<flat_tuple>")
    {
        auto opt = fxt::optional<fxt::flat_tuple<int, int>>{fxt::make_flat_tuple(1, 2)};
        auto result = fxt::mtuple_append(opt, 3, 4, 5);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }
}

// ============================================================================
// Tests for pipe operator - Plain values
// ============================================================================

TEST_CASE("mtuple_append - pipe operator with plain value", "[mtuple_append][pipe][plain]")
{
    struct Error { int code; };

    SECTION("expected<tuple> | mtuple_append(value)")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto result = exp | fxt::mtuple_append(3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("expected<tuple> | mtuple_append(multiple values)")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto result = exp | fxt::mtuple_append(2, 3, 4);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
    }

    SECTION("optional<tuple> | mtuple_append(value)")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(10, 20)};
        auto result = opt | fxt::mtuple_append(30);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 10);
        REQUIRE(fxt::get<1>(t) == 20);
        REQUIRE(fxt::get<2>(t) == 30);
    }

    SECTION("optional<flat_tuple> | mtuple_append(multiple values)")
    {
        auto opt = fxt::optional<fxt::flat_tuple<double>>{fxt::make_flat_tuple(1.0)};
        auto result = opt | fxt::mtuple_append(2.0, 3.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
    }
}

// ============================================================================
// Tests for pipe operator - Monadic values
// ============================================================================

TEST_CASE("mtuple_append - pipe operator with monadic value", "[mtuple_append][pipe][monadic]")
{
    struct Error { int code; };

    SECTION("expected<tuple> | mtuple_append(expected<value>)")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto exp_value = fxt::expected<int, Error>{3};
        auto result = exp_tuple | fxt::mtuple_append(exp_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("optional<tuple> | mtuple_append(optional<value>)")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<std::string>>{fxt::make_tuple(std::string{"hello"})};
        auto opt_value = fxt::optional<int>{42};
        auto result = opt_tuple | fxt::mtuple_append(opt_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 2);
        REQUIRE(fxt::get<0>(t) == "hello");
        REQUIRE(fxt::get<1>(t) == 42);
    }
}

// ============================================================================
// Tests for chaining operations
// ============================================================================

TEST_CASE("mtuple_append - chaining with other monadic operations", "[mtuple_append][chaining]")
{
    struct Error { int code; };

    SECTION("chain multiple mtuple_append calls")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto result = exp
            | fxt::mtuple_append(2)
            | fxt::mtuple_append(3)
            | fxt::mtuple_append(4);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
    }

    SECTION("chain mtuple_append with different value counts")
    {
        auto opt = fxt::optional<fxt::tuple<std::string>>{fxt::make_tuple(std::string{"start"})};
        auto result = opt
            | fxt::mtuple_append(1, 2)
            | fxt::mtuple_append(3.14)
            | fxt::mtuple_append(true, false);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 6);
        REQUIRE(fxt::get<0>(t) == "start");
        REQUIRE(fxt::get<1>(t) == 1);
        REQUIRE(fxt::get<2>(t) == 2);
        REQUIRE(fxt::get<3>(t) == 3.14);
        REQUIRE(fxt::get<4>(t) == true);
        REQUIRE(fxt::get<5>(t) == false);
    }
}

