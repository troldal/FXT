/*
    AsArrayTests - Unit tests for fxt::as_array
*/

#include "../Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>

TEST_CASE("as_array - Convert fxt::tuple to std::array", "[as_array][tuple]")
{
    SECTION("Convert homogeneous tuple to array")
    {
        auto tpl = fxt::make_tuple(1, 2, 3, 4);
        auto arr = fxt::as_array<int>(tpl);

        REQUIRE(arr.size() == 4);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
        REQUIRE(arr[3] == 4);
    }

    SECTION("Convert heterogeneous tuple to array with type conversion")
    {
        auto tpl = fxt::make_tuple(1, 2.5, 3);
        auto arr = fxt::as_array<double>(tpl);

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1.0);
        REQUIRE(arr[1] == 2.5);
        REQUIRE(arr[2] == 3.0);
    }

    SECTION("Convert empty tuple")
    {
        auto tpl = fxt::make_tuple();
        auto arr = fxt::as_array<int>(tpl);

        REQUIRE(arr.size() == 0);
    }

    SECTION("Convert single element tuple")
    {
        auto tpl = fxt::make_tuple(42);
        auto arr = fxt::as_array<int>(tpl);

        REQUIRE(arr.size() == 1);
        REQUIRE(arr[0] == 42);
    }
}

TEST_CASE("as_array - Convert fxt::flat_tuple to std::array", "[as_array][flat_tuple]")
{
    SECTION("Convert homogeneous flat_tuple to array")
    {
        auto tpl = fxt::make_flat_tuple(1.0, 2.0, 3.0);
        auto arr = fxt::as_array<double>(tpl);

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1.0);
        REQUIRE(arr[1] == 2.0);
        REQUIRE(arr[2] == 3.0);
    }

    SECTION("Convert heterogeneous flat_tuple to array with type conversion")
    {
        auto tpl = fxt::make_flat_tuple(1, 2.5, 3.7);
        auto arr = fxt::as_array<int>(tpl);

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }
}

TEST_CASE("as_array - Curried version", "[as_array][curry]")
{
    SECTION("Use curried version with tuple")
    {
        auto tpl = fxt::make_tuple(10, 20, 30);
        auto arr = tpl | fxt::as_array<double>();

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 10.0);
        REQUIRE(arr[1] == 20.0);
        REQUIRE(arr[2] == 30.0);
    }

    SECTION("Use curried version with flat_tuple")
    {
        auto tpl = fxt::make_flat_tuple(1.1, 2.2, 3.3);
        auto arr = tpl | fxt::as_array<int>();

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SECTION("Chain with other operations")
    {
        auto result = fxt::make_tuple(1, 2, 3)
            | fxt::as_array<double>();

        REQUIRE(result.size() == 3);
        REQUIRE(result[0] == 1.0);
        REQUIRE(result[1] == 2.0);
        REQUIRE(result[2] == 3.0);
    }
}

TEST_CASE("as_array - Type conversions", "[as_array][conversion]")
{
    SECTION("Convert to larger type")
    {
        auto tpl = fxt::make_tuple(1, 2, 3);
        auto arr = fxt::as_array<long long>(tpl);

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1LL);
        REQUIRE(arr[1] == 2LL);
        REQUIRE(arr[2] == 3LL);
    }

    SECTION("Convert floating point to integral (truncation)")
    {
        auto tpl = fxt::make_tuple(1.7, 2.3, 3.9);
        auto arr = fxt::as_array<int>(tpl);

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }
}

TEST_CASE("as_array - Move semantics", "[as_array][move]")
{
    SECTION("Convert from rvalue tuple")
    {
        auto arr = fxt::as_array<int>(fxt::make_tuple(1, 2, 3));

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1);
        REQUIRE(arr[1] == 2);
        REQUIRE(arr[2] == 3);
    }

    SECTION("Convert from rvalue flat_tuple")
    {
        auto arr = fxt::as_array<double>(fxt::make_flat_tuple(1.0, 2.0, 3.0));

        REQUIRE(arr.size() == 3);
        REQUIRE(arr[0] == 1.0);
        REQUIRE(arr[1] == 2.0);
        REQUIRE(arr[2] == 3.0);
    }
}

