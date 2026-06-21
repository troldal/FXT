//
// Test suite for fxt::tuple_apply_replace and fxt::mtuple_apply_replace
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

// ============================================================================
// Tests for fxt::tuple_apply_replace with fxt::tuple
// ============================================================================

TEST_CASE("apply_replace - basic operations with fxt::tuple", "[apply_replace]")
{
    SECTION("replace two elements with their sum")
    {
        auto t = fxt::tuple{3, 4};
        auto result = fxt::tuple_apply_replace([](int a, int b) { return a + b; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 7);
    }

    SECTION("replace three elements with their product")
    {
        auto t = fxt::tuple{2, 3, 4};
        auto result = fxt::tuple_apply_replace([](int a, int b, int c) { return a * b * c; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 24);
    }

    SECTION("type transformation - ints to string")
    {
        auto t = fxt::tuple{10, 20};
        auto result = fxt::tuple_apply_replace([](int a, int b) {
            return std::to_string(a) + "+" + std::to_string(b);
        }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == "10+20");
    }

    SECTION("single element tuple")
    {
        auto t = fxt::tuple{42};
        auto result = fxt::tuple_apply_replace([](int x) { return x * 2; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 84);
    }
}

TEST_CASE("apply_replace - with pipe operator", "[apply_replace]")
{
    SECTION("single application")
    {
        auto result = fxt::tuple{5, 10}
            | fxt::tuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 15);
    }

    SECTION("chained applications")
    {
        auto result = fxt::tuple{2, 3}
            | fxt::tuple_apply_replace([](int a, int b) { return a * b; })
            | fxt::tuple_apply_replace([](int product) { return product + 10; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 16);
    }

    SECTION("multiple chained applications")
    {
        auto result = fxt::tuple{1, 2}
            | fxt::tuple_apply_replace([](int a, int b) { return a + b; })
            | fxt::tuple_apply_replace([](int sum) { return sum * 2; })
            | fxt::tuple_apply_replace([](int doubled) { return doubled - 1; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 5);  // (1+2)*2-1 = 5
    }
}

TEST_CASE("apply_replace - with flat_tuple", "[apply_replace]")
{
    SECTION("replace two elements")
    {
        auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
        auto result = fxt::tuple_apply_replace([](double a, double b) { return a / b; }, ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == Catch::Approx(0.666667).epsilon(0.001));
    }

    SECTION("flat_tuple with pipe operator")
    {
        auto result = fxt::flat_tuple<int, int, int>{1, 2, 3}
            | fxt::tuple_apply_replace([](int a, int b, int c) { return a + b + c; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 6);
    }

    SECTION("chained flat_tuple operations")
    {
        auto result = fxt::flat_tuple<double, double>{10.0, 2.0}
            | fxt::tuple_apply_replace([](double a, double b) { return a / b; })
            | fxt::tuple_apply_replace([](double quotient) { return quotient * 2; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == Catch::Approx(10.0).epsilon(0.001));
    }
}

TEST_CASE("apply_replace - returning tuple", "[apply_replace]")
{
    SECTION("return nested tuple from two elements")
    {
        auto t = fxt::tuple{10, 3};
        auto result = fxt::tuple_apply_replace([](int a, int b) {
            return fxt::tuple{a / b, a % b};
        }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        auto nested = fxt::get<0>(result);
        REQUIRE(fxt::get<0>(nested) == 3);
        REQUIRE(fxt::get<1>(nested) == 1);
    }

    SECTION("return tuple with mixed types")
    {
        auto t = fxt::tuple{42};
        auto result = fxt::tuple_apply_replace([](int x) {
            return fxt::tuple{x, std::to_string(x)};
        }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        auto nested = fxt::get<0>(result);
        REQUIRE(fxt::get<0>(nested) == 42);
        REQUIRE(fxt::get<1>(nested) == "42");
    }
}

// ============================================================================
// Tests for fxt::mtuple_apply_replace with fxt::expected
// ============================================================================

TEST_CASE("mapply_replace - fxt::expected with regular return values", "[mapply_replace]")
{
    SECTION("replace with sum in successful expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{5, 10}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(result.value())>> == 1);
        REQUIRE(fxt::get<0>(result.value()) == 15);
    }

    SECTION("replace with product in successful expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::tuple{2, 3, 4}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b, int c) { return a * b * c; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(result.value())>> == 1);
        REQUIRE(fxt::get<0>(result.value()) == 24);
    }

    SECTION("type transformation in expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{7, 8}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) {
            return std::to_string(a) + "-" + std::to_string(b);
        });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == "7-8");
    }

    SECTION("chain multiple mapply_replace operations")
    {
        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{2, 3}}
            | fxt::mtuple_apply_replace([](int a, int b) { return a + b; })
            | fxt::mtuple_apply_replace([](int sum) { return sum * 2; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 10);
    }
}

TEST_CASE("mapply_replace - fxt::expected with error propagation", "[mapply_replace]")
{
    SECTION("error in initial expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected("error")};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("error propagates through chain")
    {
        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected("initial error")}
            | fxt::mtuple_apply_replace([](int a, int b) { return a + b; })
            | fxt::mtuple_apply_replace([](int sum) { return sum * 2; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("mapply_replace - fxt::expected with void return", "[mapply_replace]")
{
    SECTION("void function creates empty tuple")
    {
        int side_effect = 0;
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{5, 10}};
        auto result = exp | fxt::mtuple_apply_replace([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 15);
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(result.value())>> == 0);
    }

    SECTION("void function with error propagation")
    {
        int side_effect = 0;
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected("error")};
        auto result = exp | fxt::mtuple_apply_replace([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);  // Function not called
        REQUIRE(result.error() == "error");
    }
}

TEST_CASE("mapply_replace - fxt::expected with monadic return", "[mapply_replace]")
{
    SECTION("function returning expected (success)")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 2}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) -> fxt::expected<int, std::string> {
            if (b == 0) return fxt::unexpected("division by zero");
            return a / b;
        });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 5);
    }

    SECTION("function returning expected (error)")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 0}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) -> fxt::expected<int, std::string> {
            if (b == 0) return fxt::unexpected("division by zero");
            return a / b;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "division by zero");
    }
}

// ============================================================================
// Tests for fxt::optional
// ============================================================================

TEST_CASE("mapply_replace - fxt::optional with regular return values", "[mapply_replace]")
{
    SECTION("replace with sum in optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{7, 8}};
        auto result = opt | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(*result)>> == 1);
        REQUIRE(fxt::get<0>(*result) == 15);
    }

    SECTION("replace with difference in optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{10, 3}};
        auto result = opt | fxt::mtuple_apply_replace([](int a, int b) { return a - b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 7);
    }

    SECTION("type transformation in optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{3, 4}};
        auto result = opt | fxt::mtuple_apply_replace([](int a, int b) {
            return std::to_string(a * b);
        });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == "12");
    }

    SECTION("chain multiple mapply_replace operations")
    {
        auto result = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{4, 5}}
            | fxt::mtuple_apply_replace([](int a, int b) { return a * b; })
            | fxt::mtuple_apply_replace([](int product) { return product - 10; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10);
    }
}

TEST_CASE("mapply_replace - fxt::optional with nullopt propagation", "[mapply_replace]")
{
    SECTION("nullopt in initial optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
        auto result = opt | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("nullopt propagates through chain")
    {
        auto result = fxt::optional<fxt::tuple<int, int>>{std::nullopt}
            | fxt::mtuple_apply_replace([](int a, int b) { return a + b; })
            | fxt::mtuple_apply_replace([](int sum) { return sum * 2; });

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("mapply_replace - fxt::optional with void return", "[mapply_replace]")
{
    SECTION("void function creates empty tuple")
    {
        int side_effect = 0;
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{6, 9}};
        auto result = opt | fxt::mtuple_apply_replace([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 15);
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(*result)>> == 0);
    }

    SECTION("void function with nullopt propagation")
    {
        int side_effect = 0;
        auto opt = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
        auto result = opt | fxt::mtuple_apply_replace([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);  // Function not called
    }
}

// ============================================================================
// Tests for mixed tuple types
// ============================================================================

TEST_CASE("mapply_replace - preserves tuple type", "[mapply_replace]")
{
    SECTION("fxt::tuple is preserved")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{3, 4}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::impl::is_fxt_tuple_v<decltype(result.value())>);
    }

    SECTION("fxt::flat_tuple is preserved")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, std::string>{fxt::flat_tuple<int, int>{3, 4}};
        auto result = exp | fxt::mtuple_apply_replace([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::impl::is_flat_tuple_v<decltype(result.value())>);
    }
}

// ============================================================================
// Tests for const and reference qualifiers
// ============================================================================

TEST_CASE("apply_replace - works with const and reference qualifiers", "[apply_replace]")
{
    SECTION("const lvalue reference")
    {
        const auto t = fxt::tuple{5, 10};
        auto result = fxt::tuple_apply_replace([](int a, int b) { return a + b; }, t);

        REQUIRE(fxt::get<0>(result) == 15);
    }

    SECTION("rvalue reference")
    {
        auto result = fxt::tuple_apply_replace([](int a, int b) { return a + b; }, fxt::tuple{7, 3});

        REQUIRE(fxt::get<0>(result) == 10);
    }
}

// ============================================================================
// Comparison tests: apply_replace vs apply_append
// ============================================================================

TEST_CASE("apply_replace - comparison with apply_append", "[apply_replace]")
{
    SECTION("apply_append keeps original elements")
    {
        auto t = fxt::tuple{3, 4};
        auto result = fxt::tuple_apply_append([](int a, int b) { return a + b; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 7);
    }

    SECTION("apply_replace removes original elements")
    {
        auto t = fxt::tuple{3, 4};
        auto result = fxt::tuple_apply_replace([](int a, int b) { return a + b; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 7);
    }
}

