//
// Test suite for fxt::tuple_append
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt/tuple/TupleAppend.hpp>
#include <fxt/tuple/Tuple.hpp>
#include <fxt/tuple/FlatTuple.hpp>
#include <string>

// ============================================================================
// Tests for fxt::tuple (std::tuple) - Direct Call
// ============================================================================

TEST_CASE("tuple_append - fxt::tuple direct call with single value", "[tuple_append][tuple][direct]")
{
    SECTION("append int to empty tuple")
    {
        auto t = fxt::tuple{};
        auto result = fxt::tuple_append(t, 42);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("append int to tuple with one element")
    {
        auto t = fxt::tuple{1};
        auto result = fxt::tuple_append(t, 2);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
    }

    SECTION("append string to tuple with int")
    {
        auto t = fxt::tuple{42};
        auto result = fxt::tuple_append(t, std::string{"hello"});

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == "hello");
    }

    SECTION("append double to tuple with mixed types")
    {
        auto t = fxt::tuple{42, std::string{"world"}};
        auto result = fxt::tuple_append(t, 3.14);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == "world");
        REQUIRE(fxt::get<2>(result) == 3.14);
    }

    SECTION("append bool to complex tuple")
    {
        auto t = fxt::tuple{1, 2.5, std::string{"test"}};
        auto result = fxt::tuple_append(t, true);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2.5);
        REQUIRE(fxt::get<2>(result) == "test");
        REQUIRE(fxt::get<3>(result) == true);
    }
}

TEST_CASE("tuple_append - fxt::tuple direct call with multiple values", "[tuple_append][tuple][direct][variadic]")
{
    SECTION("append two ints to tuple")
    {
        auto t = fxt::tuple{1};
        auto result = fxt::tuple_append(t, 2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("append three values of different types")
    {
        auto t = fxt::tuple{std::string{"start"}};
        auto result = fxt::tuple_append(t, 42, 3.14, true);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == "start");
        REQUIRE(fxt::get<1>(result) == 42);
        REQUIRE(fxt::get<2>(result) == 3.14);
        REQUIRE(fxt::get<3>(result) == true);
    }

    SECTION("append five ints")
    {
        auto t = fxt::tuple{1, 2};
        auto result = fxt::tuple_append(t, 3, 4, 5, 6, 7);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 7);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
        REQUIRE(fxt::get<5>(result) == 6);
        REQUIRE(fxt::get<6>(result) == 7);
    }

    SECTION("append to empty tuple")
    {
        auto t = fxt::tuple{};
        auto result = fxt::tuple_append(t, 1, 2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }
}

// ============================================================================
// Tests for fxt::tuple - Pipe Operator
// ============================================================================

TEST_CASE("tuple_append - fxt::tuple pipe operator with single value", "[tuple_append][tuple][pipe]")
{
    SECTION("append int using pipe")
    {
        auto t = fxt::tuple{1, 2, 3};
        auto result = t | fxt::tuple_append(4);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
    }

    SECTION("append string using pipe")
    {
        auto t = fxt::tuple{42};
        auto result = t | fxt::tuple_append(std::string{"test"});

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == "test");
    }

    SECTION("chain multiple single appends")
    {
        auto result = fxt::tuple{1}
            | fxt::tuple_append(2)
            | fxt::tuple_append(3)
            | fxt::tuple_append(4);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
    }

    SECTION("chain appends with different types")
    {
        auto result = fxt::tuple{std::string{"hello"}}
            | fxt::tuple_append(42)
            | fxt::tuple_append(3.14)
            | fxt::tuple_append(true);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == "hello");
        REQUIRE(fxt::get<1>(result) == 42);
        REQUIRE(fxt::get<2>(result) == 3.14);
        REQUIRE(fxt::get<3>(result) == true);
    }
}

TEST_CASE("tuple_append - fxt::tuple pipe operator with multiple values", "[tuple_append][tuple][pipe][variadic]")
{
    SECTION("append two values using pipe")
    {
        auto t = fxt::tuple{1};
        auto result = t | fxt::tuple_append(2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("append three mixed types using pipe")
    {
        auto t = fxt::tuple{std::string{"start"}};
        auto result = t | fxt::tuple_append(42, 3.14, true);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == "start");
        REQUIRE(fxt::get<1>(result) == 42);
        REQUIRE(fxt::get<2>(result) == 3.14);
        REQUIRE(fxt::get<3>(result) == true);
    }

    SECTION("chain single and multiple appends")
    {
        auto result = fxt::tuple{std::string{"data"}}
            | fxt::tuple_append(1, 2, 3)
            | fxt::tuple_append(4.5)
            | fxt::tuple_append(6.7, 8.9);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 7);
        REQUIRE(fxt::get<0>(result) == "data");
        REQUIRE(fxt::get<1>(result) == 1);
        REQUIRE(fxt::get<2>(result) == 2);
        REQUIRE(fxt::get<3>(result) == 3);
        REQUIRE(fxt::get<4>(result) == 4.5);
        REQUIRE(fxt::get<5>(result) == 6.7);
        REQUIRE(fxt::get<6>(result) == 8.9);
    }

    SECTION("append four values at once")
    {
        auto t = fxt::tuple{1};
        auto result = t | fxt::tuple_append(2, 3, 4, 5);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Direct Call
// ============================================================================

TEST_CASE("tuple_append - fxt::flat_tuple direct call with single value", "[tuple_append][flat_tuple][direct]")
{
    SECTION("append double to flat_tuple")
    {
        auto t = fxt::flat_tuple<double, double>{1.0, 2.0};
        auto result = fxt::tuple_append(t, 3.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("append int to flat_tuple")
    {
        auto t = fxt::flat_tuple<int>{10};
        auto result = fxt::tuple_append(t, 20);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
    }

    SECTION("append to single element flat_tuple")
    {
        auto t = fxt::flat_tuple<double>{5.0};
        auto result = fxt::tuple_append(t, 10.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 5.0);
        REQUIRE(fxt::get<1>(result) == 10.0);
    }

    SECTION("successive appends")
    {
        auto t1 = fxt::flat_tuple<int>{1};
        auto t2 = fxt::tuple_append(t1, 2);
        auto t3 = fxt::tuple_append(t2, 3);
        auto t4 = fxt::tuple_append(t3, 4);

        REQUIRE(fxt::tuple_size_v<decltype(t4)> == 4);
        REQUIRE(fxt::get<0>(t4) == 1);
        REQUIRE(fxt::get<1>(t4) == 2);
        REQUIRE(fxt::get<2>(t4) == 3);
        REQUIRE(fxt::get<3>(t4) == 4);
    }
}

TEST_CASE("tuple_append - fxt::flat_tuple direct call with multiple values", "[tuple_append][flat_tuple][direct][variadic]")
{
    SECTION("append two doubles")
    {
        auto t = fxt::flat_tuple<double>{1.0};
        auto result = fxt::tuple_append(t, 2.0, 3.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("append three ints")
    {
        auto t = fxt::flat_tuple<int, int>{1, 2};
        auto result = fxt::tuple_append(t, 3, 4, 5);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
    }

    SECTION("append four values")
    {
        auto t = fxt::flat_tuple<int>{10};
        auto result = fxt::tuple_append(t, 20, 30, 40, 50);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 30);
        REQUIRE(fxt::get<3>(result) == 40);
        REQUIRE(fxt::get<4>(result) == 50);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Pipe Operator
// ============================================================================

TEST_CASE("tuple_append - fxt::flat_tuple pipe operator with single value", "[tuple_append][flat_tuple][pipe]")
{
    SECTION("append using pipe")
    {
        auto t = fxt::flat_tuple<double, double>{1.0, 2.0};
        auto result = t | fxt::tuple_append(3.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("chain multiple appends")
    {
        auto result = fxt::flat_tuple<double>{5.0}
            | fxt::tuple_append(10.0)
            | fxt::tuple_append(15.0)
            | fxt::tuple_append(20.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 5.0);
        REQUIRE(fxt::get<1>(result) == 10.0);
        REQUIRE(fxt::get<2>(result) == 15.0);
        REQUIRE(fxt::get<3>(result) == 20.0);
    }

    SECTION("build sequence with pipe")
    {
        auto result = fxt::flat_tuple<int>{1}
            | fxt::tuple_append(2)
            | fxt::tuple_append(3)
            | fxt::tuple_append(4)
            | fxt::tuple_append(5);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
    }
}

TEST_CASE("tuple_append - fxt::flat_tuple pipe operator with multiple values", "[tuple_append][flat_tuple][pipe][variadic]")
{
    SECTION("append two values using pipe")
    {
        auto t = fxt::flat_tuple<double>{0.0};
        auto result = t | fxt::tuple_append(1.0, 2.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 0.0);
        REQUIRE(fxt::get<1>(result) == 1.0);
        REQUIRE(fxt::get<2>(result) == 2.0);
    }

    SECTION("append four values using pipe")
    {
        auto t = fxt::flat_tuple<int>{10};
        auto result = t | fxt::tuple_append(20, 30, 40, 50);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 30);
        REQUIRE(fxt::get<3>(result) == 40);
        REQUIRE(fxt::get<4>(result) == 50);
    }

    SECTION("mix single and multiple appends")
    {
        auto result = fxt::flat_tuple<int>{1}
            | fxt::tuple_append(2, 3)
            | fxt::tuple_append(4)
            | fxt::tuple_append(5, 6, 7);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 7);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
        REQUIRE(fxt::get<5>(result) == 6);
        REQUIRE(fxt::get<6>(result) == 7);
    }
}

// ============================================================================
// Edge Cases and Special Scenarios
// ============================================================================

TEST_CASE("tuple_append - edge cases", "[tuple_append][edge_cases]")
{
    SECTION("append to empty fxt::tuple")
    {
        auto t = fxt::tuple{};
        auto result = fxt::tuple_append(t, 42);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("append multiple to empty fxt::tuple")
    {
        auto t = fxt::tuple{};
        auto result = fxt::tuple_append(t, 1, 2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("const tuple append")
    {
        const auto t = fxt::tuple{1, 2};
        auto result = fxt::tuple_append(t, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("rvalue tuple append")
    {
        auto result = fxt::tuple_append(fxt::tuple{1, 2}, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("const flat_tuple append")
    {
        const auto t = fxt::flat_tuple<int, int>{1, 2};
        auto result = fxt::tuple_append(t, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }
}

// ============================================================================
// Type Preservation Tests
// ============================================================================

TEST_CASE("tuple_append - type preservation", "[tuple_append][types]")
{
    SECTION("int types are preserved")
    {
        auto t = fxt::tuple{1, 2, 3};
        auto result = fxt::tuple_append(t, 4, 5);

        static_assert(std::is_same_v<decltype(fxt::get<0>(result)), int&>);
        static_assert(std::is_same_v<decltype(fxt::get<4>(result)), int&>);
    }

    SECTION("mixed types are preserved")
    {
        auto t = fxt::tuple{42, std::string{"test"}};
        auto result = fxt::tuple_append(t, 3.14, true);

        static_assert(std::is_same_v<decltype(fxt::get<0>(result)), int&>);
        static_assert(std::is_same_v<decltype(fxt::get<1>(result)), std::string&>);
        static_assert(std::is_same_v<decltype(fxt::get<2>(result)), double&>);
        static_assert(std::is_same_v<decltype(fxt::get<3>(result)), bool&>);
    }

    SECTION("flat_tuple types are preserved")
    {
        auto t = fxt::flat_tuple<double, double>{1.0, 2.0};
        auto result = fxt::tuple_append(t, 3.0);

        static_assert(std::is_same_v<decltype(fxt::get<0>(result)), double&>);
        static_assert(std::is_same_v<decltype(fxt::get<2>(result)), double&>);
    }
}

