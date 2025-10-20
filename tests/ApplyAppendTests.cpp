//
// Test suite for fxt::apply_append and fxt::mapply_append
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <tuple>

// ============================================================================
// Tests for fxt::apply_append with fxt::tuple
// ============================================================================

TEST_CASE("apply_append - basic operations with fxt::tuple", "[apply_append]")
{
    SECTION("append sum to two-element tuple")
    {
        auto t = fxt::tuple{3, 4};
        auto result = fxt::apply_append([](int a, int b) { return a + b; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 7);
    }

    SECTION("append product to three-element tuple")
    {
        auto t = fxt::tuple{2, 3, 4};
        auto result = fxt::apply_append([](int a, int b, int c) { return a * b * c; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 3);
        REQUIRE(fxt::get<2>(result) == 4);
        REQUIRE(fxt::get<3>(result) == 24);
    }

    SECTION("type transformation - ints to string")
    {
        auto t = fxt::tuple{10, 20};
        auto result = fxt::apply_append([](int a, int b) {
            return std::to_string(a) + "+" + std::to_string(b);
        }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == "10+20");
    }

    SECTION("single element tuple")
    {
        auto t = fxt::tuple{42};
        auto result = fxt::apply_append([](int x) { return x * 2; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 84);
    }
}

TEST_CASE("apply_append - with pipe operator", "[apply_append]")
{
    SECTION("single application")
    {
        auto result = fxt::tuple{5, 10}
            | fxt::apply_append([](int a, int b) { return a + b; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 5);
        REQUIRE(fxt::get<1>(result) == 10);
        REQUIRE(fxt::get<2>(result) == 15);
    }

    SECTION("chained applications")
    {
        auto result = fxt::tuple{2, 3}
            | fxt::apply_append([](int a, int b) { return a * b; })
            | fxt::apply_append([](int a, int b, int product) { return a + b + product; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 3);
        REQUIRE(fxt::get<2>(result) == 6);   // 2 * 3
        REQUIRE(fxt::get<3>(result) == 11);  // 2 + 3 + 6
    }

    SECTION("multiple chained applications")
    {
        auto result = fxt::tuple{1, 2}
            | fxt::apply_append([](int a, int b) { return a + b; })
            | fxt::apply_append([](int a, int b, int sum) { return a * b; })
            | fxt::apply_append([](int a, int b, int sum, int product) { return sum + product; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);  // 1 + 2
        REQUIRE(fxt::get<3>(result) == 2);  // 1 * 2
        REQUIRE(fxt::get<4>(result) == 5);  // 3 + 2
    }
}

TEST_CASE("apply_append - with flat_tuple", "[apply_append]")
{
    SECTION("append to two elements")
    {
        auto ft = fxt::flat_tuple<double, double>{2.0, 3.0};
        auto result = fxt::apply_append([](double a, double b) { return a / b; }, ft);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 2.0);
        REQUIRE(fxt::get<1>(result) == 3.0);
        REQUIRE(fxt::get<2>(result) == Catch::Approx(0.666667).epsilon(0.001));
    }

    SECTION("flat_tuple with pipe operator")
    {
        auto result = fxt::flat_tuple<int, int, int>{1, 2, 3}
            | fxt::apply_append([](int a, int b, int c) { return a + b + c; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 6);
    }

    SECTION("chained flat_tuple operations")
    {
        auto result = fxt::flat_tuple<double, double>{10.0, 2.0}
            | fxt::apply_append([](double a, double b) { return a / b; })
            | fxt::apply_append([](double a, double b, double quotient) { return quotient * 2; });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 10.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 5.0);
        REQUIRE(fxt::get<3>(result) == Catch::Approx(10.0).epsilon(0.001));
    }
}

TEST_CASE("apply_append - void-returning functions", "[apply_append]")
{
    SECTION("void function doesn't append anything")
    {
        int side_effect = 0;
        auto t = fxt::tuple{5, 10};
        auto result = fxt::apply_append([&](int a, int b) {
            side_effect = a + b;
        }, t);

        REQUIRE(side_effect == 15);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 5);
        REQUIRE(fxt::get<1>(result) == 10);
    }

    SECTION("void function in chain")
    {
        int side_effect = 0;
        auto result = fxt::tuple{3, 4}
            | fxt::apply_append([](int a, int b) { return a + b; })
            | fxt::apply_append([&](int a, int b, int sum) {
                side_effect = sum;
            });

        REQUIRE(side_effect == 7);
        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<2>(result) == 7);
    }
}

// ============================================================================
// Tests for fxt::mapply_append with fxt::expected
// ============================================================================

TEST_CASE("mapply_append - fxt::expected with regular return values", "[mapply_append]")
{
    SECTION("append sum to successful expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{5, 10}};
        auto result = exp | fxt::mapply_append([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(result.value())>> == 3);
        REQUIRE(fxt::get<0>(result.value()) == 5);
        REQUIRE(fxt::get<1>(result.value()) == 10);
        REQUIRE(fxt::get<2>(result.value()) == 15);
    }

    SECTION("append product to three-element tuple")
    {
        auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{fxt::tuple{2, 3, 4}};
        auto result = exp | fxt::mapply_append([](int a, int b, int c) { return a * b * c; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(result.value())>> == 4);
        REQUIRE(fxt::get<0>(result.value()) == 2);
        REQUIRE(fxt::get<1>(result.value()) == 3);
        REQUIRE(fxt::get<2>(result.value()) == 4);
        REQUIRE(fxt::get<3>(result.value()) == 24);
    }

    SECTION("type transformation in expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{7, 8}};
        auto result = exp | fxt::mapply_append([](int a, int b) {
            return std::to_string(a) + "-" + std::to_string(b);
        });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 7);
        REQUIRE(fxt::get<1>(result.value()) == 8);
        REQUIRE(fxt::get<2>(result.value()) == "7-8");
    }

    SECTION("chain multiple mapply_append operations")
    {
        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{2, 3}}
            | fxt::mapply_append([](int a, int b) { return a + b; })
            | fxt::mapply_append([](int a, int b, int sum) { return a * b; })
            | fxt::mapply_append([](int a, int b, int sum, int product) { return sum + product; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 2);
        REQUIRE(fxt::get<1>(result.value()) == 3);
        REQUIRE(fxt::get<2>(result.value()) == 5);   // 2 + 3
        REQUIRE(fxt::get<3>(result.value()) == 6);   // 2 * 3
        REQUIRE(fxt::get<4>(result.value()) == 11);  // 5 + 6
    }
}

TEST_CASE("mapply_append - fxt::expected with error propagation", "[mapply_append]")
{
    SECTION("error in initial expected")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected("error")};
        auto result = exp | fxt::mapply_append([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("error propagates through chain")
    {
        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected("initial error")}
            | fxt::mapply_append([](int a, int b) { return a + b; })
            | fxt::mapply_append([](int a, int b, int sum) { return sum * 2; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("mapply_append - fxt::expected with void return", "[mapply_append]")
{
    SECTION("void function doesn't append")
    {
        int side_effect = 0;
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{5, 10}};
        auto result = exp | fxt::mapply_append([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 15);
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(result.value())>> == 2);
        REQUIRE(fxt::get<0>(result.value()) == 5);
        REQUIRE(fxt::get<1>(result.value()) == 10);
    }

    SECTION("void function with error propagation")
    {
        int side_effect = 0;
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::unexpected("error")};
        auto result = exp | fxt::mapply_append([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);  // Function not called
        REQUIRE(result.error() == "error");
    }
}

TEST_CASE("mapply_append - fxt::expected with monadic return", "[mapply_append]")
{
    SECTION("function returning expected (success)")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 2}};
        auto result = exp | fxt::mapply_append([](int a, int b) -> fxt::expected<int, std::string> {
            if (b == 0) return fxt::unexpected("division by zero");
            return a / b;
        });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 10);
        REQUIRE(fxt::get<1>(result.value()) == 2);
        REQUIRE(fxt::get<2>(result.value()) == 5);
    }

    SECTION("function returning expected (error)")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{10, 0}};
        auto result = exp | fxt::mapply_append([](int a, int b) -> fxt::expected<int, std::string> {
            if (b == 0) return fxt::unexpected("division by zero");
            return a / b;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "division by zero");
    }

    SECTION("chain with monadic returns")
    {
        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{12, 3}}
            | fxt::mapply_append([](int a, int b) -> fxt::expected<int, std::string> {
                if (b == 0) return fxt::unexpected("division by zero");
                return a / b;
            })
            | fxt::mapply_append([](int a, int b, int quotient) { return quotient * 2; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 12);
        REQUIRE(fxt::get<1>(result.value()) == 3);
        REQUIRE(fxt::get<2>(result.value()) == 4);
        REQUIRE(fxt::get<3>(result.value()) == 8);
    }
}

// ============================================================================
// Tests for fxt::mapply_append with fxt::optional
// ============================================================================

TEST_CASE("mapply_append - fxt::optional with regular return values", "[mapply_append]")
{
    SECTION("append sum to optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{7, 8}};
        auto result = opt | fxt::mapply_append([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(*result)>> == 3);
        REQUIRE(fxt::get<0>(*result) == 7);
        REQUIRE(fxt::get<1>(*result) == 8);
        REQUIRE(fxt::get<2>(*result) == 15);
    }

    SECTION("append difference to optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{10, 3}};
        auto result = opt | fxt::mapply_append([](int a, int b) { return a - b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10);
        REQUIRE(fxt::get<1>(*result) == 3);
        REQUIRE(fxt::get<2>(*result) == 7);
    }

    SECTION("type transformation in optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{3, 4}};
        auto result = opt | fxt::mapply_append([](int a, int b) {
            return std::to_string(a * b);
        });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 3);
        REQUIRE(fxt::get<1>(*result) == 4);
        REQUIRE(fxt::get<2>(*result) == "12");
    }

    SECTION("chain multiple mapply_append operations")
    {
        auto result = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{4, 5}}
            | fxt::mapply_append([](int a, int b) { return a * b; })
            | fxt::mapply_append([](int a, int b, int product) { return product - 10; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 4);
        REQUIRE(fxt::get<1>(*result) == 5);
        REQUIRE(fxt::get<2>(*result) == 20);
        REQUIRE(fxt::get<3>(*result) == 10);
    }
}

TEST_CASE("mapply_append - fxt::optional with nullopt propagation", "[mapply_append]")
{
    SECTION("nullopt in initial optional")
    {
        auto opt = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
        auto result = opt | fxt::mapply_append([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("nullopt propagates through chain")
    {
        auto result = fxt::optional<fxt::tuple<int, int>>{std::nullopt}
            | fxt::mapply_append([](int a, int b) { return a + b; })
            | fxt::mapply_append([](int a, int b, int sum) { return sum * 2; });

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("mapply_append - fxt::optional with void return", "[mapply_append]")
{
    SECTION("void function doesn't append")
    {
        int side_effect = 0;
        auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::tuple{6, 9}};
        auto result = opt | fxt::mapply_append([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 15);
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(*result)>> == 2);
        REQUIRE(fxt::get<0>(*result) == 6);
        REQUIRE(fxt::get<1>(*result) == 9);
    }

    SECTION("void function with nullopt propagation")
    {
        int side_effect = 0;
        auto opt = fxt::optional<fxt::tuple<int, int>>{std::nullopt};
        auto result = opt | fxt::mapply_append([&](int a, int b) {
            side_effect = a + b;
        });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);  // Function not called
    }
}

// ============================================================================
// Tests for mixed tuple types
// ============================================================================

TEST_CASE("mapply_append - preserves tuple type", "[mapply_append]")
{
    SECTION("fxt::tuple is preserved")
    {
        auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{3, 4}};
        auto result = exp | fxt::mapply_append([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::impl::is_fxt_tuple_v<decltype(result.value())>);
    }

    SECTION("fxt::flat_tuple is preserved")
    {
        auto exp = fxt::expected<fxt::flat_tuple<int, int>, std::string>{fxt::flat_tuple<int, int>{3, 4}};
        auto result = exp | fxt::mapply_append([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::impl::is_flat_tuple_v<decltype(result.value())>);
    }
}

// ============================================================================
// Tests for const and reference qualifiers
// ============================================================================

TEST_CASE("apply_append - works with const and reference qualifiers", "[apply_append]")
{
    SECTION("const lvalue reference")
    {
        const auto t = fxt::tuple{5, 10};
        auto result = fxt::apply_append([](int a, int b) { return a + b; }, t);

        REQUIRE(fxt::get<0>(result) == 5);
        REQUIRE(fxt::get<1>(result) == 10);
        REQUIRE(fxt::get<2>(result) == 15);
    }

    SECTION("rvalue reference")
    {
        auto result = fxt::apply_append([](int a, int b) { return a + b; }, fxt::tuple{7, 3});

        REQUIRE(fxt::get<0>(result) == 7);
        REQUIRE(fxt::get<1>(result) == 3);
        REQUIRE(fxt::get<2>(result) == 10);
    }
}

// ============================================================================
// Complex computation tests
// ============================================================================

TEST_CASE("apply_append - complex computation pipelines", "[apply_append]")
{
    SECTION("building up mathematical results")
    {
        auto result = fxt::tuple{3.0, 4.0}
            | fxt::apply_append([](double x, double y) {
                return std::sqrt(x * x + y * y);  // hypotenuse
            })
            | fxt::apply_append([](double x, double y, double h) {
                return (x * y) / 2.0;  // triangle area
            })
            | fxt::apply_append([](double x, double y, double h, double area) {
                return x + y + h;  // perimeter
            });

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 3.0);
        REQUIRE(fxt::get<1>(result) == 4.0);
        REQUIRE(fxt::get<2>(result) == Catch::Approx(5.0).epsilon(0.001));
        REQUIRE(fxt::get<3>(result) == Catch::Approx(6.0).epsilon(0.001));
        REQUIRE(fxt::get<4>(result) == Catch::Approx(12.0).epsilon(0.001));
    }

    SECTION("monadic pipeline with error handling")
    {
        auto safe_divide = [](int a, int b) -> fxt::expected<int, std::string> {
            if (b == 0) return fxt::unexpected("division by zero");
            return a / b;
        };

        auto result = fxt::expected<fxt::tuple<int, int>, std::string>{fxt::tuple{20, 4}}
            | fxt::mapply_append(safe_divide)
            | fxt::mapply_append([](int a, int b, int quotient) { return quotient * 2; })
            | fxt::mapply_append([](int a, int b, int quotient, int doubled) {
                return a + b + quotient + doubled;
            });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(result.value()) == 20);
        REQUIRE(fxt::get<1>(result.value()) == 4);
        REQUIRE(fxt::get<2>(result.value()) == 5);
        REQUIRE(fxt::get<3>(result.value()) == 10);
        REQUIRE(fxt::get<4>(result.value()) == 39);  // 20 + 4 + 5 + 10
    }
}

// ============================================================================
// Edge cases
// ============================================================================

TEST_CASE("apply_append - edge cases", "[apply_append]")
{
    SECTION("empty tuple becomes single element")
    {
        auto t = fxt::tuple{};
        auto result = fxt::apply_append([]() { return 42; }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("appending tuple creates nested tuple")
    {
        auto t = fxt::tuple{1, 2};
        auto result = fxt::apply_append([](int a, int b) {
            return fxt::tuple{a + b, a * b};
        }, t);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        auto nested = fxt::get<2>(result);
        REQUIRE(fxt::get<0>(nested) == 3);
        REQUIRE(fxt::get<1>(nested) == 2);
    }
}

