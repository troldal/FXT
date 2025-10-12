//
// Test suite for fxt::mtuple_prepend
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

// ============================================================================
// Tests for fxt::expected with fxt::tuple - Plain values
// ============================================================================

TEST_CASE("mtuple_prepend - expected<tuple> with plain value", "[mtuple_prepend][expected][tuple][direct]")
{
    struct Error { int code; };

    SECTION("prepend single int to expected<tuple>")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto result = fxt::mtuple_prepend(exp, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 3);
        REQUIRE(fxt::get<1>(t) == 1);
        REQUIRE(fxt::get<2>(t) == 2);
    }

    SECTION("prepend multiple values to expected<tuple>")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto result = fxt::mtuple_prepend(exp, 2, 3, 4);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 2);
        REQUIRE(fxt::get<1>(t) == 3);
        REQUIRE(fxt::get<2>(t) == 4);
        REQUIRE(fxt::get<3>(t) == 1);
    }

    SECTION("error propagation with plain value")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{42}}};
        auto result = fxt::mtuple_prepend(exp, 3);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 42);
    }

    SECTION("prepend string to expected<tuple>")
    {
        auto exp = fxt::expected<fxt::tuple<int, double>, Error>{fxt::make_tuple(42, 3.14)};
        auto result = fxt::mtuple_prepend(exp, std::string{"header"});

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == "header");
        REQUIRE(fxt::get<1>(t) == 42);
        REQUIRE(fxt::get<2>(t) == 3.14);
    }
}

// ============================================================================
// Tests for fxt::expected with fxt::tuple - Monadic values
// ============================================================================

TEST_CASE("mtuple_prepend - expected<tuple> with expected<value>", "[mtuple_prepend][expected][tuple][monadic]")
{
    struct Error { int code; };

    SECTION("prepend expected<int> to expected<tuple> - both valid")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto exp_value = fxt::expected<int, Error>{3};
        auto result = fxt::mtuple_prepend(exp_tuple, exp_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 3);
        REQUIRE(fxt::get<1>(t) == 1);
        REQUIRE(fxt::get<2>(t) == 2);
    }

    SECTION("prepend expected<int> to expected<tuple> - tuple has error")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::unexpected{Error{10}}};
        auto exp_value = fxt::expected<int, Error>{3};
        auto result = fxt::mtuple_prepend(exp_tuple, exp_value);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 10);
    }

    SECTION("prepend expected<int> to expected<tuple> - value has error")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(1, 2)};
        auto exp_value = fxt::expected<int, Error>{fxt::unexpected{Error{20}}};
        auto result = fxt::mtuple_prepend(exp_tuple, exp_value);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 20);
    }

    SECTION("prepend multiple expected values - all valid")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto exp2 = fxt::expected<int, Error>{2};
        auto exp3 = fxt::expected<int, Error>{3};
        auto result = fxt::mtuple_prepend(exp_tuple, exp2, exp3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 2);
        REQUIRE(fxt::get<1>(t) == 3);
        REQUIRE(fxt::get<2>(t) == 1);
    }

    SECTION("prepend multiple expected values - one has error")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(1)};
        auto exp2 = fxt::expected<int, Error>{2};
        auto exp3 = fxt::expected<int, Error>{fxt::unexpected{Error{30}}};
        auto result = fxt::mtuple_prepend(exp_tuple, exp2, exp3);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 30);
    }
}

// ============================================================================
// Tests for fxt::expected with fxt::tuple - Pipe Operator
// ============================================================================

TEST_CASE("mtuple_prepend - expected<tuple> with pipe operator", "[mtuple_prepend][expected][tuple][pipe]")
{
    struct Error { int code; };

    SECTION("prepend single value using pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(2, 3)};
        auto result = exp | fxt::mtuple_prepend(1);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("prepend multiple values using pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(5)};
        auto result = exp | fxt::mtuple_prepend(1, 2, 3, 4);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }

    SECTION("chain multiple prepends using pipe")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(5)};
        auto result = exp
            | fxt::mtuple_prepend(4)
            | fxt::mtuple_prepend(2, 3)
            | fxt::mtuple_prepend(1);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }

    SECTION("prepend expected<value> using pipe")
    {
        auto exp_tuple = fxt::expected<fxt::tuple<int, int>, Error>{fxt::make_tuple(2, 3)};
        auto exp_value = fxt::expected<int, Error>{1};
        auto result = exp_tuple | fxt::mtuple_prepend(exp_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("error propagation in pipe chain")
    {
        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(3)};
        auto exp_err = fxt::expected<int, Error>{fxt::unexpected{Error{50}}};
        auto result = exp
            | fxt::mtuple_prepend(2)
            | fxt::mtuple_prepend(exp_err)
            | fxt::mtuple_prepend(0);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == 50);
    }
}

// ============================================================================
// Tests for fxt::optional with fxt::tuple - Plain values
// ============================================================================

TEST_CASE("mtuple_prepend - optional<tuple> with plain value", "[mtuple_prepend][optional][tuple][direct]")
{
    SECTION("prepend single int to optional<tuple>")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(1, 2)};
        auto result = fxt::mtuple_prepend(opt, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 3);
        REQUIRE(fxt::get<1>(t) == 1);
        REQUIRE(fxt::get<2>(t) == 2);
    }

    SECTION("prepend multiple values to optional<tuple>")
    {
        auto opt = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(5)};
        auto result = fxt::mtuple_prepend(opt, 1, 2, 3, 4);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }

    SECTION("nullopt propagation with plain value")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{};
        auto result = fxt::mtuple_prepend(opt, 3);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("prepend string to optional<tuple>")
    {
        auto opt = fxt::optional<fxt::tuple<int, double>>{fxt::make_tuple(42, 3.14)};
        auto result = fxt::mtuple_prepend(opt, std::string{"prefix"});

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == "prefix");
        REQUIRE(fxt::get<1>(t) == 42);
        REQUIRE(fxt::get<2>(t) == 3.14);
    }
}

// ============================================================================
// Tests for fxt::optional with fxt::tuple - Monadic values
// ============================================================================

TEST_CASE("mtuple_prepend - optional<tuple> with optional<value>", "[mtuple_prepend][optional][tuple][monadic]")
{
    SECTION("prepend optional<int> to optional<tuple> - both valid")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(2, 3)};
        auto opt_value = fxt::optional<int>{1};
        auto result = fxt::mtuple_prepend(opt_tuple, opt_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("prepend optional<int> to optional<tuple> - tuple is nullopt")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{};
        auto opt_value = fxt::optional<int>{1};
        auto result = fxt::mtuple_prepend(opt_tuple, opt_value);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("prepend optional<int> to optional<tuple> - value is nullopt")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(2, 3)};
        auto opt_value = fxt::optional<int>{};
        auto result = fxt::mtuple_prepend(opt_tuple, opt_value);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("prepend multiple optional values - all valid")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(4)};
        auto opt2 = fxt::optional<int>{1};
        auto opt3 = fxt::optional<int>{2};
        auto result = fxt::mtuple_prepend(opt_tuple, opt2, opt3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 4);
    }

    SECTION("prepend multiple optional values - one is nullopt")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(3)};
        auto opt1 = fxt::optional<int>{1};
        auto opt2 = fxt::optional<int>{};
        auto result = fxt::mtuple_prepend(opt_tuple, opt1, opt2);

        REQUIRE_FALSE(result.has_value());
    }
}

// ============================================================================
// Tests for fxt::optional with fxt::tuple - Pipe Operator
// ============================================================================

TEST_CASE("mtuple_prepend - optional<tuple> with pipe operator", "[mtuple_prepend][optional][tuple][pipe]")
{
    SECTION("prepend single value using pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(2, 3)};
        auto result = opt | fxt::mtuple_prepend(1);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("prepend multiple values using pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(4)};
        auto result = opt | fxt::mtuple_prepend(1, 2, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
    }

    SECTION("chain multiple prepends using pipe")
    {
        auto opt = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(5)};
        auto result = opt
            | fxt::mtuple_prepend(4)
            | fxt::mtuple_prepend(2, 3)
            | fxt::mtuple_prepend(1);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }

    SECTION("prepend optional<value> using pipe")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(2, 3)};
        auto opt_value = fxt::optional<int>{1};
        auto result = opt_tuple | fxt::mtuple_prepend(opt_value);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
    }

    SECTION("nullopt propagation in pipe chain")
    {
        auto opt = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(3)};
        auto opt_null = fxt::optional<int>{};
        auto result = opt
            | fxt::mtuple_prepend(2)
            | fxt::mtuple_prepend(opt_null)
            | fxt::mtuple_prepend(0);

        REQUIRE_FALSE(result.has_value());
    }
}

// ============================================================================
// Tests for fxt::expected with fxt::flat_tuple
// ============================================================================

TEST_CASE("mtuple_prepend - expected<flat_tuple>", "[mtuple_prepend][expected][flat_tuple]")
{
    struct Error { int code; };

    SECTION("prepend to expected<flat_tuple> - direct call")
    {
        auto exp = fxt::expected<fxt::flat_tuple<double, double>, Error>{
            fxt::make_flat_tuple(2.0, 3.0)
        };
        auto result = fxt::mtuple_prepend(exp, 1.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
    }

    SECTION("prepend multiple values to expected<flat_tuple>")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, Error>{
            fxt::make_flat_tuple(3, 4)
        };
        auto result = fxt::mtuple_prepend(exp, 1, 2);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
    }

    SECTION("prepend to expected<flat_tuple> using pipe")
    {
        auto exp = fxt::expected<fxt::flat_tuple<double, double>, Error>{
            fxt::make_flat_tuple(3.0, 4.0)
        };
        auto result = exp | fxt::mtuple_prepend(1.0, 2.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
        REQUIRE(fxt::get<3>(t) == 4.0);
    }

    SECTION("type preservation - flat_tuple remains flat_tuple")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, Error>{
            fxt::make_flat_tuple(2, 3)
        };
        auto result = fxt::mtuple_prepend(exp, 1);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::impl::is_flat_tuple_v<decltype(t)>);
        REQUIRE(!fxt::impl::is_fxt_tuple_v<decltype(t)>);
    }
}

// ============================================================================
// Tests for fxt::optional with fxt::flat_tuple
// ============================================================================

TEST_CASE("mtuple_prepend - optional<flat_tuple>", "[mtuple_prepend][optional][flat_tuple]")
{
    SECTION("prepend to optional<flat_tuple> - direct call")
    {
        auto opt = fxt::optional<fxt::flat_tuple<double, double>>{
            fxt::make_flat_tuple(2.0, 3.0)
        };
        auto result = fxt::mtuple_prepend(opt, 1.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 3);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
    }

    SECTION("prepend multiple values to optional<flat_tuple>")
    {
        auto opt = fxt::optional<fxt::flat_tuple<int, int>>{
            fxt::make_flat_tuple(4, 5)
        };
        auto result = fxt::mtuple_prepend(opt, 1, 2, 3);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }

    SECTION("prepend to optional<flat_tuple> using pipe")
    {
        auto opt = fxt::optional<fxt::flat_tuple<double, double>>{
            fxt::make_flat_tuple(3.0, 4.0)
        };
        auto result = opt | fxt::mtuple_prepend(1.0, 2.0);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 4);
        REQUIRE(fxt::get<0>(t) == 1.0);
        REQUIRE(fxt::get<1>(t) == 2.0);
        REQUIRE(fxt::get<2>(t) == 3.0);
        REQUIRE(fxt::get<3>(t) == 4.0);
    }

    SECTION("chain prepends on optional<flat_tuple>")
    {
        auto opt = fxt::optional<fxt::flat_tuple<int>>{fxt::make_flat_tuple(5)};
        auto result = opt
            | fxt::mtuple_prepend(4)
            | fxt::mtuple_prepend(2, 3)
            | fxt::mtuple_prepend(1);

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
// Tests for Complex Scenarios
// ============================================================================

TEST_CASE("mtuple_prepend - complex scenarios", "[mtuple_prepend][complex]")
{
    struct Error { int code; };

    SECTION("build sequence by prepending in monadic context")
    {
        auto result = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(5)}
            | fxt::mtuple_prepend(4)
            | fxt::mtuple_prepend(3)
            | fxt::mtuple_prepend(2)
            | fxt::mtuple_prepend(1);

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        for (int i = 0; i < 5; ++i) {
            REQUIRE(fxt::get<0>(t) == 1);
            REQUIRE(fxt::get<1>(t) == 2);
            REQUIRE(fxt::get<2>(t) == 3);
            REQUIRE(fxt::get<3>(t) == 4);
            REQUIRE(fxt::get<4>(t) == 5);
        }
    }

    SECTION("mix plain and monadic values")
    {
        auto opt_tuple = fxt::optional<fxt::tuple<int>>{fxt::make_tuple(5)};
        auto opt_val = fxt::optional<int>{2};
        auto result = opt_tuple
            | fxt::mtuple_prepend(3, 4)    // Plain values
            | fxt::mtuple_prepend(opt_val) // Monadic value
            | fxt::mtuple_prepend(1);      // Plain value

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::tuple_size_v<decltype(t)> == 5);
        REQUIRE(fxt::get<0>(t) == 1);
        REQUIRE(fxt::get<1>(t) == 2);
        REQUIRE(fxt::get<2>(t) == 3);
        REQUIRE(fxt::get<3>(t) == 4);
        REQUIRE(fxt::get<4>(t) == 5);
    }

    SECTION("prepend with computed monadic values")
    {
        auto compute = [](int x) -> fxt::expected<int, Error> {
            if (x < 0) return fxt::unexpected{Error{-1}};
            return x * 2;
        };

        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(10)};
        auto result = exp
            | fxt::mtuple_prepend(compute(4))
            | fxt::mtuple_prepend(compute(3))
            | fxt::mtuple_prepend(compute(2));

        REQUIRE(result.has_value());
        auto t = result.value();
        REQUIRE(fxt::get<0>(t) == 4);  // 2*2
        REQUIRE(fxt::get<1>(t) == 6);  // 3*2
        REQUIRE(fxt::get<2>(t) == 8);  // 4*2
        REQUIRE(fxt::get<3>(t) == 10);
    }

    SECTION("error in computed value stops chain")
    {
        auto compute = [](int x) -> fxt::expected<int, Error> {
            if (x < 0) return fxt::unexpected{Error{x}};
            return x * 2;
        };

        auto exp = fxt::expected<fxt::tuple<int>, Error>{fxt::make_tuple(10)};
        auto result = exp
            | fxt::mtuple_prepend(compute(2))
            | fxt::mtuple_prepend(compute(-5))  // This will fail
            | fxt::mtuple_prepend(compute(1));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error().code == -5);
    }
}

