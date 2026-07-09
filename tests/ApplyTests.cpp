//
// Test suite for fxt::apply
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <memory>
#include <string>
#include <tuple>

TEST_CASE("apply - fxt::expected with regular return values", "[apply]")
{
    SECTION("apply function to two-element tuple")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(5)
            | fxt::mtuple_append(10)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        auto [a, b, sum] = *result;
        REQUIRE(a == 5);
        REQUIRE(b == 10);
        REQUIRE(sum == 15);
    }

    SECTION("apply function returning different type")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(42)
            | fxt::mtuple_apply_append([](int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        auto [x, str] = *result;
        REQUIRE(x == 42);
        REQUIRE(str == "42");
    }

    SECTION("chain multiple apply operations")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(3)
            | fxt::mtuple_append(4)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; })
            | fxt::mtuple_apply_append([](int a, int b, int sum) { return sum * 2; });

        REQUIRE(result.has_value());
        auto [a, b, sum, doubled] = *result;
        REQUIRE(a == 3);
        REQUIRE(b == 4);
        REQUIRE(sum == 7);
        REQUIRE(doubled == 14);
    }

    SECTION("apply with mixed types")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(42)
            | fxt::mtuple_append(std::string{"hello"})
            | fxt::mtuple_apply_append([](int x, const std::string& s) {
                return s + " " + std::to_string(x);
            });

        REQUIRE(result.has_value());
        auto [x, s, message] = *result;
        REQUIRE(x == 42);
        REQUIRE(s == "hello");
        REQUIRE(message == "hello 42");
    }
}

TEST_CASE("apply - fxt::expected with error propagation", "[apply]")
{
    SECTION("error in initial expected")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{fxt::unexpected("error")}
            | fxt::mtuple_append(1)
            | fxt::mtuple_append(2)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("error propagates through multiple apply operations")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{fxt::unexpected("initial error")}
            | fxt::mtuple_append(1)
            | fxt::mtuple_apply_append([](int x) { return x * 2; })
            | fxt::mtuple_apply_append([](int x, int y) { return x + y; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("apply - fxt::expected with monadic return values", "[apply]")
{
    SECTION("apply function returning expected (success)")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(10.0)
            | fxt::mtuple_append(2.0)
            | fxt::mtuple_apply_append([](double a, double b) -> fxt::expected<double, std::string> {
                if (b == 0.0) {
                    return fxt::unexpected("Division by zero");
                }
                return a / b;
            });

        REQUIRE(result.has_value());
        auto [a, b, quotient] = *result;
        REQUIRE(a == 10.0);
        REQUIRE(b == 2.0);
        REQUIRE(quotient == 5.0);
    }

    SECTION("apply function returning expected (error)")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(10.0)
            | fxt::mtuple_append(0.0)
            | fxt::mtuple_apply_append([](double a, double b) -> fxt::expected<double, std::string> {
                if (b == 0.0) {
                    return fxt::unexpected("Division by zero");
                }
                return a / b;
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "Division by zero");
    }

    SECTION("chain monadic apply operations")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(100.0)
            | fxt::mtuple_append(4.0)
            | fxt::mtuple_apply_append([](double a, double b) -> fxt::expected<double, std::string> {
                if (b == 0.0) return fxt::unexpected("Division by zero");
                return a / b;
            })
            | fxt::mtuple_apply_append([](double a, double b, double quotient) -> fxt::expected<double, std::string> {
                if (quotient < 0.0) return fxt::unexpected("Negative number");
                return std::sqrt(quotient);
            });

        REQUIRE(result.has_value());
        auto [a, b, quotient, root] = *result;
        REQUIRE(a == 100.0);
        REQUIRE(b == 4.0);
        REQUIRE(quotient == 25.0);
        REQUIRE(root == 5.0);
    }
}

TEST_CASE("apply - fxt::expected with void return", "[apply]")
{
    SECTION("apply void function for side effects")
    {
        int side_effect = 0;

        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(42)
            | fxt::mtuple_append(100)
            | fxt::mtuple_apply_append([&side_effect](int a, int b) {
                side_effect = a + b;
            });

        REQUIRE(result.has_value());
        auto [a, b] = *result;
        REQUIRE(a == 42);
        REQUIRE(b == 100);
        REQUIRE(side_effect == 142);
    }

    SECTION("void function with error propagates error")
    {
        int side_effect = 0;

        auto result = fxt::expected<std::tuple<>, std::string>{fxt::unexpected("error")}
            | fxt::mtuple_append(1)
            | fxt::mtuple_apply_append([&side_effect](int x) {
                side_effect = x * 2;
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
        REQUIRE(side_effect == 0);  // Should not be called
    }

    SECTION("chain void and regular apply operations")
    {
        int side_effect = 0;

        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(5)
            | fxt::mtuple_apply_append([](int x) { return x * 2; })
            | fxt::mtuple_apply_append([&side_effect](int x, int y) {
                side_effect = x + y;
            })
            | fxt::mtuple_apply_append([](int x, int y) { return x + y; });

        REQUIRE(result.has_value());
        auto [x, y, sum] = *result;
        REQUIRE(x == 5);
        REQUIRE(y == 10);
        REQUIRE(sum == 15);
        REQUIRE(side_effect == 15);
    }
}

TEST_CASE("apply - fxt::optional with regular return values", "[apply]")
{
    SECTION("apply function to two-element tuple")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(7)
            | fxt::mtuple_append(8)
            | fxt::mtuple_apply_append([](int a, int b) { return a * b; });

        REQUIRE(result.has_value());
        auto [a, b, product] = *result;
        REQUIRE(a == 7);
        REQUIRE(b == 8);
        REQUIRE(product == 56);
    }

    SECTION("apply function returning different type")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(99)
            | fxt::mtuple_apply_append([](int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        auto [x, str] = *result;
        REQUIRE(x == 99);
        REQUIRE(str == "99");
    }

    SECTION("chain multiple apply operations")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(2)
            | fxt::mtuple_append(3)
            | fxt::mtuple_apply_append([](int a, int b) { return a * b; })
            | fxt::mtuple_apply_append([](int a, int b, int product) { return product + 10; });

        REQUIRE(result.has_value());
        auto [a, b, product, sum] = *result;
        REQUIRE(a == 2);
        REQUIRE(b == 3);
        REQUIRE(product == 6);
        REQUIRE(sum == 16);
    }

    SECTION("apply with mixed types")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(100)
            | fxt::mtuple_append(std::string{"test"})
            | fxt::mtuple_apply_append([](int x, const std::string& s) {
                return s + ":" + std::to_string(x);
            });

        REQUIRE(result.has_value());
        auto [x, s, message] = *result;
        REQUIRE(x == 100);
        REQUIRE(s == "test");
        REQUIRE(message == "test:100");
    }
}

TEST_CASE("apply - fxt::optional with empty propagation", "[apply]")
{
    SECTION("empty initial optional")
    {
        auto result = fxt::optional<std::tuple<>>{}
            | fxt::mtuple_append(1)
            | fxt::mtuple_append(2)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("empty propagates through multiple apply operations")
    {
        auto result = fxt::optional<std::tuple<>>{}
            | fxt::mtuple_append(1)
            | fxt::mtuple_apply_append([](int x) { return x * 2; })
            | fxt::mtuple_apply_append([](int x, int y) { return x + y; });

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("apply - fxt::optional with monadic return values", "[apply]")
{
    SECTION("apply function returning optional (success)")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(16.0)
            | fxt::mtuple_apply_append([](double x) -> fxt::optional<double> {
                if (x < 0.0) {
                    return fxt::nullopt;
                }
                return std::sqrt(x);
            });

        REQUIRE(result.has_value());
        auto [x, root] = *result;
        REQUIRE(x == 16.0);
        REQUIRE(root == 4.0);
    }

    SECTION("apply function returning optional (empty)")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(-16.0)
            | fxt::mtuple_apply_append([](double x) -> fxt::optional<double> {
                if (x < 0.0) {
                    return fxt::nullopt;
                }
                return std::sqrt(x);
            });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("chain monadic apply operations")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(100.0)
            | fxt::mtuple_apply_append([](double x) -> fxt::optional<double> {
                if (x < 0.0) return fxt::nullopt;
                return std::sqrt(x);
            })
            | fxt::mtuple_apply_append([](double x, double root) -> fxt::optional<double> {
                if (root == 0.0) return fxt::nullopt;
                return x / root;
            });

        REQUIRE(result.has_value());
        auto [x, root, quotient] = *result;
        REQUIRE(x == 100.0);
        REQUIRE(root == 10.0);
        REQUIRE(quotient == 10.0);
    }
}

TEST_CASE("apply - fxt::optional with void return", "[apply]")
{
    SECTION("apply void function for side effects")
    {
        int side_effect = 0;

        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(50)
            | fxt::mtuple_append(25)
            | fxt::mtuple_apply_append([&side_effect](int a, int b) {
                side_effect = a - b;
            });

        REQUIRE(result.has_value());
        auto [a, b] = *result;
        REQUIRE(a == 50);
        REQUIRE(b == 25);
        REQUIRE(side_effect == 25);
    }

    SECTION("void function with empty propagates empty")
    {
        int side_effect = 0;

        auto result = fxt::optional<std::tuple<>>{}
            | fxt::mtuple_append(1)
            | fxt::mtuple_apply_append([&side_effect](int x) {
                side_effect = x * 2;
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);  // Should not be called
    }

    SECTION("chain void and regular apply operations")
    {
        int side_effect = 0;

        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(10)
            | fxt::mtuple_apply_append([](int x) { return x / 2; })
            | fxt::mtuple_apply_append([&side_effect](int x, int y) {
                side_effect = x * y;
            })
            | fxt::mtuple_apply_append([](int x, int y) { return x * y; });

        REQUIRE(result.has_value());
        auto [x, y, product] = *result;
        REQUIRE(x == 10);
        REQUIRE(y == 5);
        REQUIRE(product == 50);
        REQUIRE(side_effect == 50);
    }
}

TEST_CASE("apply - complex pipelines", "[apply]")
{
    SECTION("complex expected pipeline")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(5)
            | fxt::mtuple_append(3)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; })
            | fxt::mtuple_apply_append([](int a, int b, int sum) { return a * b; })
            | fxt::mtuple_apply_append([](int a, int b, int sum, int product) {
                return (sum + product) / 2.0;
            });

        REQUIRE(result.has_value());
        auto [a, b, sum, product, avg] = *result;
        REQUIRE(a == 5);
        REQUIRE(b == 3);
        REQUIRE(sum == 8);
        REQUIRE(product == 15);
        REQUIRE(avg == 11.5);
    }

    SECTION("complex optional pipeline")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(10)
            | fxt::mtuple_append(2)
            | fxt::mtuple_apply_append([](int a, int b) { return a / b; })
            | fxt::mtuple_apply_append([](int a, int b, int quotient) { return quotient * quotient; })
            | fxt::mtuple_apply_append([](int a, int b, int quotient, int squared) {
                return std::to_string(squared);
            });

        REQUIRE(result.has_value());
        auto [a, b, quotient, squared, str] = *result;
        REQUIRE(a == 10);
        REQUIRE(b == 2);
        REQUIRE(quotient == 5);
        REQUIRE(squared == 25);
        REQUIRE(str == "25");
    }
}

TEST_CASE("apply - const correctness", "[apply]")
{
    SECTION("apply with const references")
    {
        const std::string s1 = "hello";
        const std::string s2 = "world";

        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(s1)
            | fxt::mtuple_append(s2)
            | fxt::mtuple_apply_append([](const std::string& a, const std::string& b) {
                return a + " " + b;
            });

        REQUIRE(result.has_value());
        auto [a, b, message] = *result;
        REQUIRE(a == "hello");
        REQUIRE(b == "world");
        REQUIRE(message == "hello world");
    }
}

TEST_CASE("apply - type deduction", "[apply]")
{
    SECTION("auto deduction with expected")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(42)
            | fxt::mtuple_apply_append([](int x) { return std::to_string(x); })
            | fxt::mtuple_apply_append([](int x, const std::string& s) { return s.length(); });

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::expected<std::tuple<int, std::string, size_t>, std::string>>);
    }

    SECTION("auto deduction with optional")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(100)
            | fxt::mtuple_apply_append([](int x) { return static_cast<double>(x); })
            | fxt::mtuple_apply_append([](int x, double d) { return x + static_cast<int>(d); });

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::optional<std::tuple<int, double, int>>>);
    }
}

TEST_CASE("apply - edge cases", "[apply]")
{
    SECTION("apply to empty tuple (expected)")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_apply_append([]() { return 42; });

        REQUIRE(result.has_value());
        auto [value] = *result;
        REQUIRE(value == 42);
    }

    SECTION("apply to empty tuple (optional)")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_apply_append([]() { return 99; });

        REQUIRE(result.has_value());
        auto [value] = *result;
        REQUIRE(value == 99);
    }

    SECTION("apply with zero result")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(5)
            | fxt::mtuple_append(5)
            | fxt::mtuple_apply_append([](int a, int b) { return a - b; });

        REQUIRE(result.has_value());
        auto [a, b, diff] = *result;
        REQUIRE(diff == 0);
    }

    SECTION("apply with false result")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(5)
            | fxt::mtuple_append(10)
            | fxt::mtuple_apply_append([](int a, int b) { return a > b; });

        REQUIRE(result.has_value());
        auto [a, b, comparison] = *result;
        REQUIRE(comparison == false);
    }
}

TEST_CASE("apply - multiple values computed", "[apply]")
{
    SECTION("compute sum and product in sequence")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::mtuple_append(6)
            | fxt::mtuple_append(7)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; })
            | fxt::mtuple_apply_append([](int a, int b, int sum) { return a * b; });

        REQUIRE(result.has_value());
        auto [a, b, sum, product] = *result;
        REQUIRE(a == 6);
        REQUIRE(b == 7);
        REQUIRE(sum == 13);
        REQUIRE(product == 42);
    }
}

// ============================================================================
// Tests for fxt::mapply_append with fxt::flat_tuple
// ============================================================================

TEST_CASE("mapply - fxt::flat_tuple with fxt::expected", "[apply][flat_tuple]")
{
    SECTION("basic apply with flat_tuple")
    {
        auto result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(2)
            | fxt::mtuple_append(3)
            | fxt::mtuple_apply_append([](int a, int b) { return a * b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 2);
        REQUIRE(fxt::get<1>(*result) == 3);
        REQUIRE(fxt::get<2>(*result) == 6);
    }

    SECTION("chain multiple operations on flat_tuple")
    {
        auto result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(10.0)
            | fxt::mtuple_append(5.0)
            | fxt::mtuple_apply_append([](double a, double b) { return a / b; })
            | fxt::mtuple_apply_append([](double a, double b, double quotient) { return quotient + 1.0; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10.0);
        REQUIRE(fxt::get<1>(*result) == 5.0);
        REQUIRE(fxt::get<2>(*result) == 2.0);
        REQUIRE(fxt::get<3>(*result) == 3.0);
    }

    SECTION("flat_tuple with mixed types")
    {
        auto result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(42)
            | fxt::mtuple_append(std::string{"test"})
            | fxt::mtuple_apply_append([](int x, const std::string& s) {
                return s + ":" + std::to_string(x);
            });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 42);
        REQUIRE(fxt::get<1>(*result) == "test");
        REQUIRE(fxt::get<2>(*result) == "test:42");
    }

    SECTION("error propagation with flat_tuple")
    {
        auto result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::unexpected("error")}
            | fxt::mtuple_append(1)
            | fxt::mtuple_append(2)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("void-returning function with flat_tuple")
    {
        int side_effect = 0;

        auto result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(5)
            | fxt::mtuple_append(10)
            | fxt::mtuple_apply_append([&side_effect](int a, int b) {
                side_effect = a + b;
            });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 5);
        REQUIRE(fxt::get<1>(*result) == 10);
        REQUIRE(side_effect == 15);
    }

    SECTION("monadic function returning expected with flat_tuple")
    {
        auto result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(10.0)
            | fxt::mtuple_append(2.0)
            | fxt::mtuple_apply_append([](double a, double b) -> fxt::expected<double, std::string> {
                if (b == 0.0) return fxt::unexpected("Division by zero");
                return a / b;
            });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 10.0);
        REQUIRE(fxt::get<1>(*result) == 2.0);
        REQUIRE(fxt::get<2>(*result) == 5.0);
    }
}

TEST_CASE("mapply - fxt::flat_tuple with fxt::optional", "[apply][flat_tuple]")
{
    SECTION("basic apply with flat_tuple")
    {
        auto result = fxt::optional<fxt::flat_tuple<>>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(7)
            | fxt::mtuple_append(8)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 7);
        REQUIRE(fxt::get<1>(*result) == 8);
        REQUIRE(fxt::get<2>(*result) == 15);
    }

    SECTION("chain multiple operations on flat_tuple")
    {
        auto result = fxt::optional<fxt::flat_tuple<>>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(100)
            | fxt::mtuple_append(10)
            | fxt::mtuple_apply_append([](int a, int b) { return a / b; })
            | fxt::mtuple_apply_append([](int a, int b, int quotient) { return quotient * 2; });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 100);
        REQUIRE(fxt::get<1>(*result) == 10);
        REQUIRE(fxt::get<2>(*result) == 10);
        REQUIRE(fxt::get<3>(*result) == 20);
    }

    SECTION("empty propagation with flat_tuple")
    {
        auto result = fxt::optional<fxt::flat_tuple<>>{}
            | fxt::mtuple_append(1)
            | fxt::mtuple_append(2)
            | fxt::mtuple_apply_append([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("monadic function returning optional with flat_tuple")
    {
        auto result = fxt::optional<fxt::flat_tuple<>>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(16.0)
            | fxt::mtuple_apply_append([](double x) -> fxt::optional<double> {
                if (x < 0.0) return fxt::nullopt;
                return std::sqrt(x);
            });

        REQUIRE(result.has_value());
        REQUIRE(fxt::get<0>(*result) == 16.0);
        REQUIRE(fxt::get<1>(*result) == 4.0);
    }
}

// ============================================================================
// Tests for fxt::apply with fxt::tuple (direct call, no pipe)
// ============================================================================

TEST_CASE("apply - fxt::tuple direct call (no pipe)", "[apply][tuple][direct]")
{
    SECTION("basic apply with two elements")
    {
        auto tuple = fxt::make_tuple(5, 10);
        auto result = fxt::apply([](int a, int b) { return a + b; }, tuple);

        REQUIRE(result == 15);
    }

    SECTION("apply with three elements")
    {
        auto tuple = fxt::make_tuple(2, 3, 4);
        auto result = fxt::apply([](int a, int b, int c) { return a * b * c; }, tuple);

        REQUIRE(result == 24);
    }

    SECTION("apply with mixed types")
    {
        auto tuple = fxt::make_tuple(42, std::string{"hello"});
        auto result = fxt::apply([](int x, const std::string& s) {
            return s + " " + std::to_string(x);
        }, tuple);

        REQUIRE(result == "hello 42");
    }

    SECTION("apply with different return type")
    {
        auto tuple = fxt::make_tuple(3.14, 2.0);
        auto result = fxt::apply([](double a, double b) { return a * b; }, tuple);

        REQUIRE(result == Catch::Approx(6.28));
    }

    SECTION("apply with rvalue tuple")
    {
        auto result = fxt::apply([](int a, int b) { return a - b; },
                                 fxt::make_tuple(10, 3));

        REQUIRE(result == 7);
    }

    SECTION("apply void-returning function")
    {
        int side_effect = 0;
        auto tuple = fxt::make_tuple(5, 7);

        fxt::apply([&side_effect](int a, int b) {
            side_effect = a * b;
        }, tuple);

        REQUIRE(side_effect == 35);
    }

    SECTION("apply with const tuple")
    {
        const auto tuple = fxt::make_tuple(100, 50);
        auto result = fxt::apply([](int a, int b) { return a / b; }, tuple);

        REQUIRE(result == 2);
    }
}

// ============================================================================
// Tests for fxt::apply with fxt::tuple (with pipe operator)
// ============================================================================

TEST_CASE("apply - fxt::tuple with pipe operator", "[apply][tuple][pipe]")
{
    SECTION("basic apply with pipe")
    {
        auto tuple = fxt::make_tuple(8, 4);
        auto result = tuple | fxt::apply([](int a, int b) { return a + b; });

        REQUIRE(result == 12);
    }

    SECTION("apply with three elements")
    {
        auto tuple = fxt::make_tuple(1, 2, 3);
        auto result = tuple | fxt::apply([](int a, int b, int c) { return a + b + c; });

        REQUIRE(result == 6);
    }

    SECTION("apply with mixed types")
    {
        auto tuple = fxt::make_tuple(std::string{"Length:"}, 42);
        auto result = tuple | fxt::apply([](const std::string& prefix, int value) {
            return prefix + " " + std::to_string(value);
        });

        REQUIRE(result == "Length: 42");
    }

    SECTION("chained operations with pipe")
    {
        auto tuple = fxt::make_tuple(10, 5);
        auto result = tuple | fxt::apply([](int a, int b) { return a - b; });

        REQUIRE(result == 5);
    }

    SECTION("apply with temporary tuple")
    {
        auto result = fxt::make_tuple(6, 7) | fxt::apply([](int a, int b) { return a * b; });

        REQUIRE(result == 42);
    }

    SECTION("void-returning function with pipe")
    {
        int side_effect = 0;
        auto tuple = fxt::make_tuple(100, std::string{"meters"});

        tuple | fxt::apply([&side_effect](int distance, const std::string& unit) {
            side_effect = distance;
        });

        REQUIRE(side_effect == 100);
    }

    SECTION("apply with const tuple")
    {
        const auto tuple = fxt::make_tuple(20, 4);
        auto result = tuple | fxt::apply([](int a, int b) { return a / b; });

        REQUIRE(result == 5);
    }
}

// ============================================================================
// Tests for fxt::apply with fxt::flat_tuple (direct call, no pipe)
// ============================================================================

TEST_CASE("apply - fxt::flat_tuple direct call (no pipe)", "[apply][flat_tuple][direct]")
{
    SECTION("basic apply with two elements")
    {
        auto ft = fxt::make_flat_tuple(3.0, 4.0);
        auto result = fxt::apply([](double a, double b) {
            return std::sqrt(a * a + b * b);
        }, ft);

        REQUIRE(result == Catch::Approx(5.0));
    }

    SECTION("apply with multiple elements")
    {
        auto ft = fxt::make_flat_tuple(1, 2, 3, 4, 5);
        auto result = fxt::apply([](int a, int b, int c, int d, int e) {
            return a + b + c + d + e;
        }, ft);

        REQUIRE(result == 15);
    }

    SECTION("apply with mixed types")
    {
        auto ft = fxt::make_flat_tuple(42, 3.14, std::string{"pi"});
        std::string captured;

        fxt::apply([&captured](int i, double d, const std::string& s) {
            captured = s + ":" + std::to_string(i) + ":" + std::to_string(d);
        }, ft);

        REQUIRE(captured.find("pi:42:3.14") != std::string::npos);
    }

    SECTION("apply with different return type")
    {
        auto ft = fxt::make_flat_tuple(10, 20, 30);
        auto result = fxt::apply([](int a, int b, int c) {
            return (a + b + c) / 3.0;
        }, ft);

        REQUIRE(result == Catch::Approx(20.0));
    }

    SECTION("apply with rvalue flat_tuple")
    {
        auto result = fxt::apply([](int a, int b) { return a * b; },
                                 fxt::make_flat_tuple(6, 7));

        REQUIRE(result == 42);
    }

    SECTION("void-returning function")
    {
        int side_effect = 0;
        auto ft = fxt::make_flat_tuple(8, 9);

        fxt::apply([&side_effect](int a, int b) {
            side_effect = a + b;
        }, ft);

        REQUIRE(side_effect == 17);
    }

    SECTION("apply with const flat_tuple")
    {
        const auto ft = fxt::make_flat_tuple(100, 25);
        auto result = fxt::apply([](int a, int b) { return a - b; }, ft);

        REQUIRE(result == 75);
    }
}

// ============================================================================
// Tests for fxt::apply with fxt::flat_tuple (with pipe operator)
// ============================================================================

TEST_CASE("apply - fxt::flat_tuple with pipe operator", "[apply][flat_tuple][pipe]")
{
    SECTION("basic apply with pipe")
    {
        auto ft = fxt::make_flat_tuple(8.0, 2.0);
        auto result = ft | fxt::apply([](double a, double b) { return a / b; });

        REQUIRE(result == Catch::Approx(4.0));
    }

    SECTION("apply with multiple elements")
    {
        auto ft = fxt::make_flat_tuple(10.0, 20.0, 30.0);
        auto result = ft | fxt::apply([](double a, double b, double c) {
            return (a + b + c) / 3.0;
        });

        REQUIRE(result == Catch::Approx(20.0));
    }

    SECTION("apply with computation")
    {
        auto ft = fxt::make_flat_tuple(5, 6, 7, 8);
        auto result = ft | fxt::apply([](int a, int b, int c, int d) {
            return a * b * c * d;
        });

        REQUIRE(result == 1680);
    }

    SECTION("apply with temporary flat_tuple")
    {
        auto result = fxt::make_flat_tuple(3, 4)
            | fxt::apply([](int a, int b) { return a + b; });

        REQUIRE(result == 7);
    }

    SECTION("void-returning function with pipe")
    {
        int side_effect = 0;
        auto ft = fxt::make_flat_tuple(15, 3);

        ft | fxt::apply([&side_effect](int a, int b) {
            side_effect = a / b;
        });

        REQUIRE(side_effect == 5);
    }

    SECTION("apply with const flat_tuple")
    {
        const auto ft = fxt::make_flat_tuple(50, 10);
        auto result = ft | fxt::apply([](int a, int b) { return a - b; });

        REQUIRE(result == 40);
    }

    SECTION("apply with string concatenation")
    {
        auto ft = fxt::make_flat_tuple(std::string{"Hello"}, std::string{" "}, std::string{"World"});
        auto result = ft | fxt::apply([](const std::string& a, const std::string& b, const std::string& c) {
            return a + b + c;
        });

        REQUIRE(result == "Hello World");
    }
}

// ============================================================================
// Tests comparing fxt::tuple vs fxt::flat_tuple
// ============================================================================

TEST_CASE("apply - comparison between tuple and flat_tuple", "[apply][comparison]")
{
    SECTION("same computation with both types - direct call")
    {
        auto regular_tuple = fxt::make_tuple(12, 4);
        auto flat_tuple = fxt::make_flat_tuple(12, 4);

        auto result_regular = fxt::apply([](int a, int b) { return a + b; }, regular_tuple);
        auto result_flat = fxt::apply([](int a, int b) { return a + b; }, flat_tuple);

        REQUIRE(result_regular == 16);
        REQUIRE(result_flat == 16);
        REQUIRE(result_regular == result_flat);
    }

    SECTION("same computation with both types - pipe operator")
    {
        auto regular_tuple = fxt::make_tuple(10, 5);
        auto flat_tuple = fxt::make_flat_tuple(10, 5);

        auto result_regular = regular_tuple | fxt::apply([](int a, int b) { return a * b; });
        auto result_flat = flat_tuple | fxt::apply([](int a, int b) { return a * b; });

        REQUIRE(result_regular == 50);
        REQUIRE(result_flat == 50);
        REQUIRE(result_regular == result_flat);
    }

    SECTION("complex computation with both types")
    {
        auto regular_tuple = fxt::make_tuple(3.0, 4.0);
        auto flat_tuple = fxt::make_flat_tuple(3.0, 4.0);

        auto compute = [](double a, double b) { return std::sqrt(a * a + b * b); };

        auto result_regular = fxt::apply(compute, regular_tuple);
        auto result_flat = fxt::apply(compute, flat_tuple);

        REQUIRE(result_regular == Catch::Approx(5.0));
        REQUIRE(result_flat == Catch::Approx(5.0));
        REQUIRE(result_regular == Catch::Approx(result_flat));
    }
}

// ============================================================================
// Advanced integration tests
// ============================================================================

TEST_CASE("apply - advanced integration tests", "[apply][advanced]")
{
    SECTION("combining mapply and apply - expected with flat_tuple")
    {
        auto monadic_result = fxt::expected<fxt::flat_tuple<>, std::string>{fxt::flat_tuple<>{}}
            | fxt::mtuple_append(3)
            | fxt::mtuple_append(4)
            | fxt::mtuple_apply_append([](int a, int b) { return a * b; });

        REQUIRE(monadic_result.has_value());

        auto extracted = *monadic_result;
        auto final_result = extracted | fxt::apply([](int a, int b, int product) {
            return a + b + product;
        });

        REQUIRE(final_result == 19);
    }

    SECTION("combining mapply and apply - optional with tuple")
    {
        auto monadic_result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::mtuple_append(10)
            | fxt::mtuple_append(5)
            | fxt::mtuple_apply_append([](int a, int b) { return a - b; });

        REQUIRE(monadic_result.has_value());

        auto extracted = *monadic_result;
        auto final_result = fxt::apply([](int a, int b, int diff) {
            return a + b + diff;
        }, extracted);

        REQUIRE(final_result == 20);
    }
}

// ============================================================================
// Tests for fxt::mapply (REPLACE semantics - tuple is replaced by result)
// ============================================================================

TEST_CASE("mapply - expected: plain return replaces tuple", "[apply][mapply]")
{
    SECTION("lvalue expected")
    {
        auto exp = fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(3, 4)};
        auto result = exp | fxt::mapply([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 7);
        STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::expected<int, std::string>>);
    }

    SECTION("rvalue expected")
    {
        auto result = fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(5, 6)}
                    | fxt::mapply([](int a, int b) { return a * b; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 30);
    }

    SECTION("error propagates without calling the function")
    {
        bool called = false;
        auto result = fxt::expected<std::tuple<int, int>, std::string>{fxt::unexpected("err")}
                    | fxt::mapply([&called](int, int) { called = true; return 0; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "err");
        REQUIRE_FALSE(called);
    }
}

TEST_CASE("mapply - expected: monadic return is flattened (and_then)", "[apply][mapply]")
{
    SECTION("success case")
    {
        auto result = fxt::expected<std::tuple<double, double>, std::string>{std::make_tuple(10.0, 2.0)}
                    | fxt::mapply([](double a, double b) -> fxt::expected<double, std::string> {
                          if (b == 0.0) return fxt::unexpected(std::string{"div by zero"});
                          return a / b;
                      });

        REQUIRE(result.has_value());
        REQUIRE(*result == 5.0);
    }

    SECTION("function returns error - propagated")
    {
        auto result = fxt::expected<std::tuple<double, double>, std::string>{std::make_tuple(10.0, 0.0)}
                    | fxt::mapply([](double a, double b) -> fxt::expected<double, std::string> {
                          if (b == 0.0) return fxt::unexpected(std::string{"div by zero"});
                          return a / b;
                      });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "div by zero");
    }
}

TEST_CASE("mapply - expected (lvalue): void return - Case 2b regression", "[apply][mapply]")
{
    // This case was broken: expected_like<TArg> rejected the lvalue reference type,
    // so `lvalue_exp | mapply(void_fn)` failed to compile. Fixed by using
    // expected_like<std::remove_cvref_t<TArg>>.
    SECTION("void function called on lvalue expected")
    {
        int side_effect = 0;
        auto exp = fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(3, 4)};

        auto result = exp | fxt::mapply([&side_effect](int a, int b) { side_effect = a + b; });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 7);
    }

    SECTION("void function not called on lvalue expected holding error")
    {
        int side_effect = 0;
        auto exp = fxt::expected<std::tuple<int, int>, std::string>{fxt::unexpected("bad")};

        auto result = exp | fxt::mapply([&side_effect](int a, int b) { side_effect = a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);
        REQUIRE(result.error() == "bad");
    }

    SECTION("void function called on rvalue expected (pre-existing)")
    {
        int side_effect = 0;
        auto result = fxt::expected<std::tuple<int, int>, std::string>{std::make_tuple(10, 20)}
                    | fxt::mapply([&side_effect](int a, int b) { side_effect = a + b; });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 30);
    }
}

TEST_CASE("mapply - optional: plain return replaces tuple", "[apply][mapply]")
{
    SECTION("has value")
    {
        auto result = fxt::optional<std::tuple<int, int>>{std::make_tuple(5, 6)}
                    | fxt::mapply([](int a, int b) { return a * b; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 30);
        STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::optional<int>>);
    }

    SECTION("empty propagates without calling the function")
    {
        bool called = false;
        auto result = fxt::optional<std::tuple<int, int>>{}
                    | fxt::mapply([&called](int, int) { called = true; return 0; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(called);
    }
}

TEST_CASE("mapply - optional: void return", "[apply][mapply]")
{
    SECTION("has value - side effect runs")
    {
        int side_effect = 0;
        auto opt = fxt::optional<std::tuple<int, int>>{std::make_tuple(7, 8)};

        auto result = opt | fxt::mapply([&side_effect](int a, int b) { side_effect = a + b; });

        REQUIRE(result.has_value());
        REQUIRE(side_effect == 15);
    }

    SECTION("empty - side effect does not run")
    {
        int side_effect = 0;
        auto result = fxt::optional<std::tuple<int, int>>{}
                    | fxt::mapply([&side_effect](int a, int b) { side_effect = a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);
    }
}


