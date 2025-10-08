//
// Test suite for fxt::append
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <memory>
#include <string>
#include <tuple>

TEST_CASE("append - fxt::expected with plain values", "[append]")
{
    SECTION("append int to tuple")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto result = exp | fxt::append(100);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }

    SECTION("append string to tuple")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto result = exp | fxt::append(std::string{"hello"});

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "hello");
    }

    SECTION("append double to tuple")
    {
        auto exp = fxt::expected<std::tuple<int, std::string>, std::string>{std::tuple{42, "test"}};
        auto result = exp | fxt::append(3.14);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "test");
        REQUIRE(std::get<2>(*result) == 3.14);
    }

    SECTION("chain multiple appends")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}};
        auto result = exp
            | fxt::append(2)
            | fxt::append(3)
            | fxt::append(4);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
    }

    SECTION("append different types")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto result = exp
            | fxt::append(std::string{"hello"})
            | fxt::append(3.14)
            | fxt::append(true);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "hello");
        REQUIRE(std::get<2>(*result) == 3.14);
        REQUIRE(std::get<3>(*result) == true);
    }
}

TEST_CASE("append - fxt::expected with error propagation", "[append]")
{
    SECTION("error in initial expected")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{fxt::unexpected("error")};
        auto result = exp | fxt::append(100);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("error propagates through chain")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{fxt::unexpected("initial error")};
        auto result = exp
            | fxt::append(1)
            | fxt::append(2)
            | fxt::append(3);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("append - fxt::expected with expected values", "[append]")
{
    SECTION("append successful expected to tuple")
    {
        auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto exp_value = fxt::expected<std::string, std::string>{"hello"};
        auto result = exp_tuple | fxt::append(exp_value);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "hello");
    }

    SECTION("append expected with error")
    {
        auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto exp_value = fxt::expected<std::string, std::string>{fxt::unexpected("value error")};
        auto result = exp_tuple | fxt::append(exp_value);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "value error");
    }

    SECTION("both have errors - first error wins")
    {
        auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{fxt::unexpected("tuple error")};
        auto exp_value = fxt::expected<std::string, std::string>{fxt::unexpected("value error")};
        auto result = exp_tuple | fxt::append(exp_value);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "value error");
    }

    SECTION("chain multiple expected values")
    {
        auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}};
        auto exp_val1 = fxt::expected<int, std::string>{2};
        auto exp_val2 = fxt::expected<int, std::string>{3};
        auto exp_val3 = fxt::expected<int, std::string>{4};

        auto result = exp_tuple
            | fxt::append(exp_val1)
            | fxt::append(exp_val2)
            | fxt::append(exp_val3);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
    }
}

TEST_CASE("append - fxt::optional with plain values", "[append]")
{
    SECTION("append int to tuple")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto result = opt | fxt::append(100);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }

    SECTION("append string to tuple")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto result = opt | fxt::append(std::string{"world"});

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "world");
    }

    SECTION("append double to tuple")
    {
        auto opt = fxt::optional<std::tuple<int, std::string>>{std::tuple{42, "test"}};
        auto result = opt | fxt::append(2.71);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "test");
        REQUIRE(std::get<2>(*result) == 2.71);
    }

    SECTION("chain multiple appends")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{10}};
        auto result = opt
            | fxt::append(20)
            | fxt::append(30)
            | fxt::append(40);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 10);
        REQUIRE(std::get<1>(*result) == 20);
        REQUIRE(std::get<2>(*result) == 30);
        REQUIRE(std::get<3>(*result) == 40);
    }

    SECTION("append different types")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{100}};
        auto result = opt
            | fxt::append(std::string{"optional"})
            | fxt::append(1.41)
            | fxt::append(false);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 100);
        REQUIRE(std::get<1>(*result) == "optional");
        REQUIRE(std::get<2>(*result) == 1.41);
        REQUIRE(std::get<3>(*result) == false);
    }
}

TEST_CASE("append - fxt::optional with empty propagation", "[append]")
{
    SECTION("empty initial optional")
    {
        auto opt = fxt::optional<std::tuple<int>>{};
        auto result = opt | fxt::append(100);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("empty propagates through chain")
    {
        auto opt = fxt::optional<std::tuple<int>>{};
        auto result = opt
            | fxt::append(1)
            | fxt::append(2)
            | fxt::append(3);

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("append - fxt::optional with optional values", "[append]")
{
    SECTION("append successful optional to tuple")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto opt_value = fxt::optional<std::string>{"hello"};
        auto result = opt_tuple | fxt::append(opt_value);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "hello");
    }

    SECTION("append empty optional")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto opt_value = fxt::optional<std::string>{};
        auto result = opt_tuple | fxt::append(opt_value);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("both empty")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{};
        auto opt_value = fxt::optional<std::string>{};
        auto result = opt_tuple | fxt::append(opt_value);

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("chain multiple optional values")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto opt_val1 = fxt::optional<int>{2};
        auto opt_val2 = fxt::optional<int>{3};
        auto opt_val3 = fxt::optional<int>{4};

        auto result = opt_tuple
            | fxt::append(opt_val1)
            | fxt::append(opt_val2)
            | fxt::append(opt_val3);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
    }

    SECTION("chain with one empty optional")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto opt_val1 = fxt::optional<int>{2};
        auto opt_val2 = fxt::optional<int>{};  // empty
        auto opt_val3 = fxt::optional<int>{4};

        auto result = opt_tuple
            | fxt::append(opt_val1)
            | fxt::append(opt_val2)
            | fxt::append(opt_val3);

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("append - mixed plain and monadic values (expected)", "[append]")
{
    SECTION("append plain then expected value")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}};
        auto exp_value = fxt::expected<int, std::string>{2};

        auto result = exp
            | fxt::append(100)
            | fxt::append(exp_value);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 100);
        REQUIRE(std::get<2>(*result) == 2);
    }

    SECTION("append expected then plain value")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}};
        auto exp_value = fxt::expected<int, std::string>{2};

        auto result = exp
            | fxt::append(exp_value)
            | fxt::append(100);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 100);
    }

    SECTION("alternate plain and expected values")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}};
        auto exp_val1 = fxt::expected<int, std::string>{2};
        auto exp_val2 = fxt::expected<int, std::string>{4};

        auto result = exp
            | fxt::append(exp_val1)
            | fxt::append(3)
            | fxt::append(exp_val2)
            | fxt::append(5);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
        REQUIRE(std::get<4>(*result) == 5);
    }
}

TEST_CASE("append - mixed plain and monadic values (optional)", "[append]")
{
    SECTION("append plain then optional value")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto opt_value = fxt::optional<int>{2};

        auto result = opt
            | fxt::append(100)
            | fxt::append(opt_value);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 100);
        REQUIRE(std::get<2>(*result) == 2);
    }

    SECTION("append optional then plain value")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto opt_value = fxt::optional<int>{2};

        auto result = opt
            | fxt::append(opt_value)
            | fxt::append(100);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 100);
    }

    SECTION("alternate plain and optional values")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto opt_val1 = fxt::optional<int>{2};
        auto opt_val2 = fxt::optional<int>{4};

        auto result = opt
            | fxt::append(opt_val1)
            | fxt::append(3)
            | fxt::append(opt_val2)
            | fxt::append(5);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
        REQUIRE(std::get<4>(*result) == 5);
    }
}

TEST_CASE("append - rvalue references (expected)", "[append]")
{
    SECTION("append with rvalue expected")
    {
        auto result = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}}
            | fxt::append(fxt::expected<int, std::string>{100});

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }

    SECTION("chain with rvalue temporaries")
    {
        auto result = fxt::expected<std::tuple<int>, std::string>{std::tuple{1}}
            | fxt::append(2)
            | fxt::append(fxt::expected<int, std::string>{3})
            | fxt::append(4);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
    }
}

TEST_CASE("append - rvalue references (optional)", "[append]")
{
    SECTION("append with rvalue optional")
    {
        auto result = fxt::optional<std::tuple<int>>{std::tuple{42}}
            | fxt::append(fxt::optional<int>{100});

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }

    SECTION("chain with rvalue temporaries")
    {
        auto result = fxt::optional<std::tuple<int>>{std::tuple{1}}
            | fxt::append(2)
            | fxt::append(fxt::optional<int>{3})
            | fxt::append(4);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
        REQUIRE(std::get<3>(*result) == 4);
    }
}

TEST_CASE("append - move-only types (expected)", "[append]")
{
    SECTION("append unique_ptr to tuple")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto result = std::move(exp) | fxt::append(std::make_unique<int>(100));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(*std::get<1>(*result) == 100);
    }

    SECTION("append expected with unique_ptr")
    {
        auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto exp_ptr = fxt::expected<std::unique_ptr<int>, std::string>{std::make_unique<int>(100)};

        auto result = std::move(exp_tuple) | fxt::append(std::move(exp_ptr));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(*std::get<1>(*result) == 100);
    }
}

TEST_CASE("append - move-only types (optional)", "[append]")
{
    SECTION("append unique_ptr to tuple")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto result = std::move(opt) | fxt::append(std::make_unique<int>(100));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(*std::get<1>(*result) == 100);
    }

    SECTION("append optional with unique_ptr")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto opt_ptr = fxt::optional<std::unique_ptr<int>>{std::make_unique<int>(100)};

        auto result = std::move(opt_tuple) | fxt::append(std::move(opt_ptr));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(*std::get<1>(*result) == 100);
    }
}

TEST_CASE("append - complex types (expected)", "[append]")
{
    SECTION("append vector to tuple")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        std::vector<int> vec{1, 2, 3};
        auto result = exp | fxt::append(vec);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result).size() == 3);
        REQUIRE(std::get<1>(*result)[0] == 1);
    }

    SECTION("append pair to tuple")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto pair = std::make_pair(10, "test");
        auto result = exp | fxt::append(pair);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result).first == 10);
        REQUIRE(std::get<1>(*result).second == "test");
    }
}

TEST_CASE("append - complex types (optional)", "[append]")
{
    SECTION("append vector to tuple")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        std::vector<int> vec{1, 2, 3};
        auto result = opt | fxt::append(vec);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result).size() == 3);
        REQUIRE(std::get<1>(*result)[0] == 1);
    }

    SECTION("append pair to tuple")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto pair = std::make_pair(10, "test");
        auto result = opt | fxt::append(pair);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result).first == 10);
        REQUIRE(std::get<1>(*result).second == "test");
    }
}

TEST_CASE("append - const correctness (expected)", "[append]")
{
    SECTION("append to const expected")
    {
        const auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto result = exp | fxt::append(100);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }

    SECTION("append const expected value")
    {
        auto exp_tuple = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        const auto exp_value = fxt::expected<int, std::string>{100};
        auto result = exp_tuple | fxt::append(exp_value);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }
}

TEST_CASE("append - const correctness (optional)", "[append]")
{
    SECTION("append to const optional")
    {
        const auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto result = opt | fxt::append(100);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }

    SECTION("append const optional value")
    {
        auto opt_tuple = fxt::optional<std::tuple<int>>{std::tuple{42}};
        const auto opt_value = fxt::optional<int>{100};
        auto result = opt_tuple | fxt::append(opt_value);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == 100);
    }
}

TEST_CASE("append - type deduction", "[append]")
{
    SECTION("auto deduction with expected")
    {
        auto exp = fxt::expected<std::tuple<int>, std::string>{std::tuple{42}};
        auto result = exp | fxt::append(100) | fxt::append(std::string{"test"});

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::expected<std::tuple<int, int, std::string>, std::string>>);
    }

    SECTION("auto deduction with optional")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{42}};
        auto result = opt | fxt::append(100) | fxt::append(std::string{"test"});

        REQUIRE(result.has_value());
        static_assert(std::is_same_v<decltype(result), fxt::optional<std::tuple<int, int, std::string>>>);
    }
}

TEST_CASE("append - edge cases", "[append]")
{
    SECTION("append to empty tuple (expected)")
    {
        auto exp = fxt::expected<std::tuple<>, std::string>{std::tuple{}};
        auto result = exp | fxt::append(42);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
    }

    SECTION("append to empty tuple (optional)")
    {
        auto opt = fxt::optional<std::tuple<>>{std::tuple{}};
        auto result = opt | fxt::append(42);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
    }

    SECTION("append zero value")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto result = opt | fxt::append(0);

        REQUIRE(result.has_value());
        REQUIRE(std::get<1>(*result) == 0);
    }

    SECTION("append empty string")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto result = opt | fxt::append(std::string{});

        REQUIRE(result.has_value());
        REQUIRE(std::get<1>(*result) == "");
    }

    SECTION("append false")
    {
        auto opt = fxt::optional<std::tuple<int>>{std::tuple{1}};
        auto result = opt | fxt::append(false);

        REQUIRE(result.has_value());
        REQUIRE(std::get<1>(*result) == false);
    }
}

