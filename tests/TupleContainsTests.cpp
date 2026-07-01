//
// Test suite for fxt::tuple_contains / fxt::tuple_contains_v and
// fxt::tuple_type_count / fxt::tuple_type_count_v
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <array>
#include <string>
#include <utility>

TEST_CASE("tuple_contains - fxt::tuple", "[tuple_contains][tuple]")
{
    using T = fxt::tuple<int, double, std::string>;

    SECTION("contains each element type")
    {
        STATIC_REQUIRE(fxt::tuple_contains_v<int, T>);
        STATIC_REQUIRE(fxt::tuple_contains_v<double, T>);
        STATIC_REQUIRE(fxt::tuple_contains_v<std::string, T>);
    }

    SECTION("does not contain an unrelated type")
    {
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<float, T>);
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<char, T>);
    }

    SECTION("single-element tuple")
    {
        STATIC_REQUIRE(fxt::tuple_contains_v<int, fxt::tuple<int>>);
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<double, fxt::tuple<int>>);
    }
}

TEST_CASE("tuple_contains - fxt::flat_tuple", "[tuple_contains][flat_tuple]")
{
    using FT = fxt::flat_tuple<int, double>;

    SECTION("contains each element type")
    {
        STATIC_REQUIRE(fxt::tuple_contains_v<int, FT>);
        STATIC_REQUIRE(fxt::tuple_contains_v<double, FT>);
    }

    SECTION("does not contain an unrelated type")
    {
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<std::string, FT>);
    }
}

TEST_CASE("tuple_contains - works with any fxt::tuple_like type", "[tuple_contains][tuple_like]")
{
    SECTION("std::pair")
    {
        using P = std::pair<int, double>;
        STATIC_REQUIRE(fxt::tuple_contains_v<int, P>);
        STATIC_REQUIRE(fxt::tuple_contains_v<double, P>);
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<float, P>);
    }

    SECTION("std::array")
    {
        using A = std::array<int, 3>;
        STATIC_REQUIRE(fxt::tuple_contains_v<int, A>);
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<double, A>);
    }
}

TEST_CASE("tuple_contains - cv-qualified tuple types", "[tuple_contains][cv]")
{
    SECTION("const tuple")
    {
        STATIC_REQUIRE(fxt::tuple_contains_v<int, const fxt::tuple<int, double>>);
    }

    SECTION("reference to tuple")
    {
        STATIC_REQUIRE(fxt::tuple_contains_v<int, fxt::tuple<int, double>&>);
        STATIC_REQUIRE(fxt::tuple_contains_v<double, const fxt::tuple<int, double>&>);
    }
}

TEST_CASE("tuple_type_count - counts occurrences of a type", "[tuple_type_count][tuple]")
{
    SECTION("unique elements each count once")
    {
        using T = fxt::tuple<int, double, std::string>;
        STATIC_REQUIRE(fxt::tuple_type_count_v<int, T> == 1);
        STATIC_REQUIRE(fxt::tuple_type_count_v<double, T> == 1);
    }

    SECTION("duplicate elements are counted, not just detected")
    {
        using T = fxt::tuple<int, int, double, int>;
        STATIC_REQUIRE(fxt::tuple_type_count_v<int, T> == 3);
        STATIC_REQUIRE(fxt::tuple_type_count_v<double, T> == 1);
    }

    SECTION("absent type counts zero")
    {
        using T = fxt::tuple<int, double>;
        STATIC_REQUIRE(fxt::tuple_type_count_v<std::string, T> == 0);
    }

    SECTION("tuple_contains is equivalent to count > 0")
    {
        using T = fxt::tuple<int, int, double>;
        STATIC_REQUIRE(fxt::tuple_contains_v<int, T> == (fxt::tuple_type_count_v<int, T> > 0));
        STATIC_REQUIRE(fxt::tuple_contains_v<float, T> == (fxt::tuple_type_count_v<float, T> > 0));
    }
}

// A generic function constrained on tuple_contains_v, exercising it as a
// requires-clause (the primary intended use case for the trait).
namespace
{
    template<typename Tuple>
        requires fxt::tuple_contains_v<std::string, Tuple>
    constexpr bool has_a_string(const Tuple&) { return true; }
}

TEST_CASE("tuple_contains - usable as a requires-clause constraint", "[tuple_contains][constraint]")
{
    SECTION("constrained function is callable when the tuple contains the type")
    {
        fxt::tuple<int, std::string> t{1, "hi"};
        REQUIRE(has_a_string(t));
    }

    SECTION("constrained overload set correctly excludes non-matching tuples")
    {
        // Overload resolution / SFINAE friendliness: a tuple lacking std::string
        // must not satisfy the constraint. Checked at compile time via the trait
        // directly, since instantiating has_a_string() on a non-matching type
        // would be a compile error by design.
        using NoString = fxt::tuple<int, double>;
        STATIC_REQUIRE_FALSE(fxt::tuple_contains_v<std::string, NoString>);
    }
}
