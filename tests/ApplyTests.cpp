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
            | fxt::append(5)
            | fxt::append(10)
            | fxt::apply([](int a, int b) { return a + b; });

        REQUIRE(result.has_value());
        auto [a, b, sum] = *result;
        REQUIRE(a == 5);
        REQUIRE(b == 10);
        REQUIRE(sum == 15);
    }

    SECTION("apply function returning different type")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::append(42)
            | fxt::apply([](int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        auto [x, str] = *result;
        REQUIRE(x == 42);
        REQUIRE(str == "42");
    }

    SECTION("chain multiple apply operations")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::append(3)
            | fxt::append(4)
            | fxt::apply([](int a, int b) { return a + b; })
            | fxt::apply([](int a, int b, int sum) { return sum * 2; });

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
            | fxt::append(42)
            | fxt::append(std::string{"hello"})
            | fxt::apply([](int x, const std::string& s) {
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
            | fxt::append(1)
            | fxt::append(2)
            | fxt::apply([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("error propagates through multiple apply operations")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{fxt::unexpected("initial error")}
            | fxt::append(1)
            | fxt::apply([](int x) { return x * 2; })
            | fxt::apply([](int x, int y) { return x + y; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("apply - fxt::expected with monadic return values", "[apply]")
{
    SECTION("apply function returning expected (success)")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::append(10.0)
            | fxt::append(2.0)
            | fxt::apply([](double a, double b) -> fxt::expected<double, std::string> {
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
            | fxt::append(10.0)
            | fxt::append(0.0)
            | fxt::apply([](double a, double b) -> fxt::expected<double, std::string> {
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
            | fxt::append(100.0)
            | fxt::append(4.0)
            | fxt::apply([](double a, double b) -> fxt::expected<double, std::string> {
                if (b == 0.0) return fxt::unexpected("Division by zero");
                return a / b;
            })
            | fxt::apply([](double a, double b, double quotient) -> fxt::expected<double, std::string> {
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
            | fxt::append(42)
            | fxt::append(100)
            | fxt::apply([&side_effect](int a, int b) {
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
            | fxt::append(1)
            | fxt::apply([&side_effect](int x) {
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
            | fxt::append(5)
            | fxt::apply([](int x) { return x * 2; })
            | fxt::apply([&side_effect](int x, int y) {
                side_effect = x + y;
            })
            | fxt::apply([](int x, int y) { return x + y; });

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
            | fxt::append(7)
            | fxt::append(8)
            | fxt::apply([](int a, int b) { return a * b; });

        REQUIRE(result.has_value());
        auto [a, b, product] = *result;
        REQUIRE(a == 7);
        REQUIRE(b == 8);
        REQUIRE(product == 56);
    }

    SECTION("apply function returning different type")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(99)
            | fxt::apply([](int x) { return std::to_string(x); });

        REQUIRE(result.has_value());
        auto [x, str] = *result;
        REQUIRE(x == 99);
        REQUIRE(str == "99");
    }

    SECTION("chain multiple apply operations")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(2)
            | fxt::append(3)
            | fxt::apply([](int a, int b) { return a * b; })
            | fxt::apply([](int a, int b, int product) { return product + 10; });

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
            | fxt::append(100)
            | fxt::append(std::string{"test"})
            | fxt::apply([](int x, const std::string& s) {
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
            | fxt::append(1)
            | fxt::append(2)
            | fxt::apply([](int a, int b) { return a + b; });

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("empty propagates through multiple apply operations")
    {
        auto result = fxt::optional<std::tuple<>>{}
            | fxt::append(1)
            | fxt::apply([](int x) { return x * 2; })
            | fxt::apply([](int x, int y) { return x + y; });

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("apply - fxt::optional with monadic return values", "[apply]")
{
    SECTION("apply function returning optional (success)")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(16.0)
            | fxt::apply([](double x) -> fxt::optional<double> {
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
            | fxt::append(-16.0)
            | fxt::apply([](double x) -> fxt::optional<double> {
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
            | fxt::append(100.0)
            | fxt::apply([](double x) -> fxt::optional<double> {
                if (x < 0.0) return fxt::nullopt;
                return std::sqrt(x);
            })
            | fxt::apply([](double x, double root) -> fxt::optional<double> {
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
            | fxt::append(50)
            | fxt::append(25)
            | fxt::apply([&side_effect](int a, int b) {
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
            | fxt::append(1)
            | fxt::apply([&side_effect](int x) {
                side_effect = x * 2;
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(side_effect == 0);  // Should not be called
    }

    SECTION("chain void and regular apply operations")
    {
        int side_effect = 0;

        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(10)
            | fxt::apply([](int x) { return x / 2; })
            | fxt::apply([&side_effect](int x, int y) {
                side_effect = x * y;
            })
            | fxt::apply([](int x, int y) { return x * y; });

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
            | fxt::append(5)
            | fxt::append(3)
            | fxt::apply([](int a, int b) { return a + b; })
            | fxt::apply([](int a, int b, int sum) { return a * b; })
            | fxt::apply([](int a, int b, int sum, int product) {
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
            | fxt::append(10)
            | fxt::append(2)
            | fxt::apply([](int a, int b) { return a / b; })
            | fxt::apply([](int a, int b, int quotient) { return quotient * quotient; })
            | fxt::apply([](int a, int b, int quotient, int squared) {
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
            | fxt::append(s1)
            | fxt::append(s2)
            | fxt::apply([](const std::string& a, const std::string& b) {
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
            | fxt::append(42)
            | fxt::apply([](int x) { return std::to_string(x); })
            | fxt::apply([](int x, const std::string& s) { return s.length(); });

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::expected<std::tuple<int, std::string, size_t>, std::string>>);
    }

    SECTION("auto deduction with optional")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(100)
            | fxt::apply([](int x) { return static_cast<double>(x); })
            | fxt::apply([](int x, double d) { return x + static_cast<int>(d); });

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::optional<std::tuple<int, double, int>>>);
    }
}

TEST_CASE("apply - edge cases", "[apply]")
{
    SECTION("apply to empty tuple (expected)")
    {
        auto result = fxt::expected<std::tuple<>, std::string>{std::tuple{}}
            | fxt::apply([]() { return 42; });

        REQUIRE(result.has_value());
        auto [value] = *result;
        REQUIRE(value == 42);
    }

    SECTION("apply to empty tuple (optional)")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::apply([]() { return 99; });

        REQUIRE(result.has_value());
        auto [value] = *result;
        REQUIRE(value == 99);
    }

    SECTION("apply with zero result")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(5)
            | fxt::append(5)
            | fxt::apply([](int a, int b) { return a - b; });

        REQUIRE(result.has_value());
        auto [a, b, diff] = *result;
        REQUIRE(diff == 0);
    }

    SECTION("apply with false result")
    {
        auto result = fxt::optional<std::tuple<>>{std::tuple{}}
            | fxt::append(5)
            | fxt::append(10)
            | fxt::apply([](int a, int b) { return a > b; });

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
            | fxt::append(6)
            | fxt::append(7)
            | fxt::apply([](int a, int b) { return a + b; })
            | fxt::apply([](int a, int b, int sum) { return a * b; });

        REQUIRE(result.has_value());
        auto [a, b, sum, product] = *result;
        REQUIRE(a == 6);
        REQUIRE(b == 7);
        REQUIRE(sum == 13);
        REQUIRE(product == 42);
    }
}
