//
// Test suite for fxt::tuple_prepend
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>

// ============================================================================
// Tests for fxt::tuple (std::tuple) - Direct Call
// ============================================================================

TEST_CASE("tuple_prepend - fxt::tuple direct call with single value", "[tuple_prepend][tuple][direct]")
{
    SECTION("prepend int to empty tuple")
    {
        auto t = fxt::tuple<>{};
        auto result = fxt::tuple_prepend(t, 42);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 42);
    }

    SECTION("prepend int to tuple with one element")
    {
        auto t = fxt::tuple<int>{1};
        auto result = fxt::tuple_prepend(t, 2);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 1);
    }

    SECTION("prepend string to tuple with int")
    {
        auto t = fxt::tuple<int>{42};
        auto result = fxt::tuple_prepend(t, std::string{"hello"});

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == "hello");
        REQUIRE(fxt::get<1>(result) == 42);
    }

    SECTION("prepend double to tuple with mixed types")
    {
        auto t = fxt::tuple<int, std::string>{42, std::string{"world"}};
        auto result = fxt::tuple_prepend(t, 3.14);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 3.14);
        REQUIRE(fxt::get<1>(result) == 42);
        REQUIRE(fxt::get<2>(result) == "world");
    }

    SECTION("prepend bool to complex tuple")
    {
        auto t = fxt::tuple<int, double, std::string>{1, 2.5, std::string{"test"}};
        auto result = fxt::tuple_prepend(t, true);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == true);
        REQUIRE(fxt::get<1>(result) == 1);
        REQUIRE(fxt::get<2>(result) == 2.5);
        REQUIRE(fxt::get<3>(result) == "test");
    }
}

TEST_CASE("tuple_prepend - fxt::tuple direct call with multiple values", "[tuple_prepend][tuple][direct][variadic]")
{
    SECTION("prepend two ints to tuple")
    {
        auto t = fxt::tuple<int>{1};
        auto result = fxt::tuple_prepend(t, 2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 3);
        REQUIRE(fxt::get<2>(result) == 1);
    }

    SECTION("prepend three values of different types")
    {
        auto t = fxt::tuple<std::string>{std::string{"end"}};
        auto result = fxt::tuple_prepend(t, 42, 3.14, true);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 3.14);
        REQUIRE(fxt::get<2>(result) == true);
        REQUIRE(fxt::get<3>(result) == "end");
    }

    SECTION("prepend five ints")
    {
        auto t = fxt::tuple<int, int>{1, 2};
        auto result = fxt::tuple_prepend(t, 3, 4, 5, 6, 7);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 7);
        REQUIRE(fxt::get<0>(result) == 3);
        REQUIRE(fxt::get<1>(result) == 4);
        REQUIRE(fxt::get<2>(result) == 5);
        REQUIRE(fxt::get<3>(result) == 6);
        REQUIRE(fxt::get<4>(result) == 7);
        REQUIRE(fxt::get<5>(result) == 1);
        REQUIRE(fxt::get<6>(result) == 2);
    }

    SECTION("prepend to empty tuple")
    {
        auto t = fxt::tuple<>{};
        auto result = fxt::tuple_prepend(t, 1, 2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }
}

// ============================================================================
// Tests for fxt::tuple (std::tuple) - Pipe Operator
// ============================================================================

TEST_CASE("tuple_prepend - fxt::tuple pipe operator with single value", "[tuple_prepend][tuple][pipe]")
{
    SECTION("prepend int using pipe")
    {
        auto t = fxt::tuple<int, int>{1, 2};
        auto result = t | fxt::tuple_prepend(0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 0);
        REQUIRE(fxt::get<1>(result) == 1);
        REQUIRE(fxt::get<2>(result) == 2);
    }

    SECTION("prepend string using pipe")
    {
        auto t = fxt::tuple<int>{42};
        auto result = t | fxt::tuple_prepend(std::string{"header"});

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == "header");
        REQUIRE(fxt::get<1>(result) == 42);
    }

    SECTION("chain multiple prepends")
    {
        auto t = fxt::tuple<int>{5};
        auto result = t | fxt::tuple_prepend(4) | fxt::tuple_prepend(3) | fxt::tuple_prepend(2) | fxt::tuple_prepend(1);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
    }
}

TEST_CASE("tuple_prepend - fxt::tuple pipe operator with multiple values", "[tuple_prepend][tuple][pipe][variadic]")
{
    SECTION("prepend two values using pipe")
    {
        auto t = fxt::tuple<int>{3};
        auto result = t | fxt::tuple_prepend(1, 2);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
    }

    SECTION("prepend mixed types using pipe")
    {
        auto t = fxt::tuple<bool>{true};
        auto result = t | fxt::tuple_prepend(std::string{"test"}, 42, 3.14);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == "test");
        REQUIRE(fxt::get<1>(result) == 42);
        REQUIRE(fxt::get<2>(result) == 3.14);
        REQUIRE(fxt::get<3>(result) == true);
    }

    SECTION("chain prepends with different numbers of values")
    {
        auto t = fxt::tuple<std::string>{std::string{"end"}};
        auto result = t | fxt::tuple_prepend(5, 6) | fxt::tuple_prepend(3.0, 4.0) | fxt::tuple_prepend(1, 2);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 7);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3.0);
        REQUIRE(fxt::get<3>(result) == 4.0);
        REQUIRE(fxt::get<4>(result) == 5);
        REQUIRE(fxt::get<5>(result) == 6);
        REQUIRE(fxt::get<6>(result) == "end");
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Direct Call
// ============================================================================

TEST_CASE("tuple_prepend - fxt::flat_tuple direct call with single value", "[tuple_prepend][flat_tuple][direct]")
{
    SECTION("prepend double to empty flat_tuple")
    {
        auto t = fxt::flat_tuple<>{};
        auto result = fxt::tuple_prepend(t, 3.14);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 1);
        REQUIRE(fxt::get<0>(result) == 3.14);
    }

    SECTION("prepend double to flat_tuple with one element")
    {
        auto t = fxt::flat_tuple<double>{1.0};
        auto result = fxt::tuple_prepend(t, 2.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 2);
        REQUIRE(fxt::get<0>(result) == 2.0);
        REQUIRE(fxt::get<1>(result) == 1.0);
    }

    SECTION("prepend int to flat_tuple with doubles")
    {
        auto t = fxt::flat_tuple<double, double>{1.5, 2.5};
        auto result = fxt::tuple_prepend(t, 42);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 42);
        REQUIRE(fxt::get<1>(result) == 1.5);
        REQUIRE(fxt::get<2>(result) == 2.5);
    }

    SECTION("prepend string to flat_tuple")
    {
        auto t = fxt::flat_tuple<int, int>{10, 20};
        auto result = fxt::tuple_prepend(t, std::string{"prefix"});

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == "prefix");
        REQUIRE(fxt::get<1>(result) == 10);
        REQUIRE(fxt::get<2>(result) == 20);
    }
}

TEST_CASE("tuple_prepend - fxt::flat_tuple direct call with multiple values", "[tuple_prepend][flat_tuple][direct][variadic]")
{
    SECTION("prepend two doubles")
    {
        auto t = fxt::flat_tuple<double>{3.0};
        auto result = fxt::tuple_prepend(t, 1.0, 2.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("prepend three ints")
    {
        auto t = fxt::flat_tuple<int, int>{4, 5};
        auto result = fxt::tuple_prepend(t, 1, 2, 3);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1);
        REQUIRE(fxt::get<1>(result) == 2);
        REQUIRE(fxt::get<2>(result) == 3);
        REQUIRE(fxt::get<3>(result) == 4);
        REQUIRE(fxt::get<4>(result) == 5);
    }

    SECTION("prepend to empty flat_tuple")
    {
        auto t = fxt::flat_tuple<>{};
        auto result = fxt::tuple_prepend(t, 10, 20, 30);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 10);
        REQUIRE(fxt::get<1>(result) == 20);
        REQUIRE(fxt::get<2>(result) == 30);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple - Pipe Operator
// ============================================================================

TEST_CASE("tuple_prepend - fxt::flat_tuple pipe operator with single value", "[tuple_prepend][flat_tuple][pipe]")
{
    SECTION("prepend double using pipe")
    {
        auto t = fxt::flat_tuple<double, double>{2.0, 3.0};
        auto result = t | fxt::tuple_prepend(1.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 3);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
    }

    SECTION("chain multiple prepends on flat_tuple")
    {
        auto t = fxt::flat_tuple<int>{5};
        auto result = t | fxt::tuple_prepend(4) | fxt::tuple_prepend(3) | fxt::tuple_prepend(2);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 4);
        REQUIRE(fxt::get<0>(result) == 2);
        REQUIRE(fxt::get<1>(result) == 3);
        REQUIRE(fxt::get<2>(result) == 4);
        REQUIRE(fxt::get<3>(result) == 5);
    }
}

TEST_CASE("tuple_prepend - fxt::flat_tuple pipe operator with multiple values", "[tuple_prepend][flat_tuple][pipe][variadic]")
{
    SECTION("prepend three doubles using pipe")
    {
        auto t = fxt::flat_tuple<double, double>{4.0, 5.0};
        auto result = t | fxt::tuple_prepend(1.0, 2.0, 3.0);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 5);
        REQUIRE(fxt::get<0>(result) == 1.0);
        REQUIRE(fxt::get<1>(result) == 2.0);
        REQUIRE(fxt::get<2>(result) == 3.0);
        REQUIRE(fxt::get<3>(result) == 4.0);
        REQUIRE(fxt::get<4>(result) == 5.0);
    }

    SECTION("complex chaining with mixed value counts")
    {
        auto t = fxt::flat_tuple<int>{10};
        auto result = t | fxt::tuple_prepend(8, 9) | fxt::tuple_prepend(7) | fxt::tuple_prepend(5, 6);

        REQUIRE(fxt::tuple_size_v<decltype(result)> == 6);
        REQUIRE(fxt::get<0>(result) == 5);
        REQUIRE(fxt::get<1>(result) == 6);
        REQUIRE(fxt::get<2>(result) == 7);
        REQUIRE(fxt::get<3>(result) == 8);
        REQUIRE(fxt::get<4>(result) == 9);
        REQUIRE(fxt::get<5>(result) == 10);
    }
}

// ============================================================================
// Tests for Type Preservation
// ============================================================================

TEST_CASE("tuple_prepend - type preservation", "[tuple_prepend][types]")
{
    SECTION("fxt::tuple remains fxt::tuple")
    {
        auto t = fxt::tuple<int>{1};
        auto result = fxt::tuple_prepend(t, 2);

        REQUIRE(fxt::impl::is_fxt_tuple_v<decltype(result)>);
        REQUIRE(!fxt::impl::is_flat_tuple_v<decltype(result)>);
    }

    SECTION("fxt::flat_tuple remains fxt::flat_tuple")
    {
        auto t = fxt::flat_tuple<int>{1};
        auto result = fxt::tuple_prepend(t, 2);

        REQUIRE(!fxt::impl::is_fxt_tuple_v<decltype(result)>);
        REQUIRE(fxt::impl::is_flat_tuple_v<decltype(result)>);
    }

    SECTION("prepended values are correctly typed")
    {
        auto t = fxt::tuple<int>{1};
        auto result = fxt::tuple_prepend(t, 2.5, std::string{"test"});

        static_assert(std::is_same_v<fxt::tuple_element_t<0, decltype(result)>, double>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, decltype(result)>, std::string>);
        static_assert(std::is_same_v<fxt::tuple_element_t<2, decltype(result)>, int>);
    }
}

// ============================================================================
// Tests for Move Semantics
// ============================================================================

TEST_CASE("tuple_prepend - move semantics", "[tuple_prepend][move]")
{
    SECTION("prepend rvalue string")
    {
        auto t = fxt::tuple<int>{42};
        auto result = fxt::tuple_prepend(t, std::string{"moved"});

        REQUIRE(fxt::get<0>(result) == "moved");
        REQUIRE(fxt::get<1>(result) == 42);
    }

    SECTION("prepend to rvalue tuple")
    {
        auto result = fxt::tuple_prepend(fxt::tuple<int>{42}, std::string{"test"});

        REQUIRE(fxt::get<0>(result) == "test");
        REQUIRE(fxt::get<1>(result) == 42);
    }
}

