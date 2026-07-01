//
// Test suite for fxt::is_variant_alternative / fxt::is_variant_alternative_v
// and fxt::variant_alternative_count / fxt::variant_alternative_count_v
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>
#include <variant>

TEST_CASE("is_variant_alternative - fxt::variant", "[is_variant_alternative][variant]")
{
    using V = fxt::variant<int, double, std::string>;

    SECTION("has each alternative type")
    {
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, V>);
        STATIC_REQUIRE(fxt::is_variant_alternative_v<double, V>);
        STATIC_REQUIRE(fxt::is_variant_alternative_v<std::string, V>);
    }

    SECTION("does not have an unrelated type as an alternative")
    {
        STATIC_REQUIRE_FALSE(fxt::is_variant_alternative_v<float, V>);
        STATIC_REQUIRE_FALSE(fxt::is_variant_alternative_v<char, V>);
    }

    SECTION("single-alternative variant")
    {
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, fxt::variant<int>>);
        STATIC_REQUIRE_FALSE(fxt::is_variant_alternative_v<double, fxt::variant<int>>);
    }
}

TEST_CASE("is_variant_alternative - works directly on std::variant", "[is_variant_alternative][std_variant]")
{
    SECTION("std::variant, not just the fxt::variant alias")
    {
        using SV = std::variant<int, double>;
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, SV>);
        STATIC_REQUIRE(fxt::is_variant_alternative_v<double, SV>);
        STATIC_REQUIRE_FALSE(fxt::is_variant_alternative_v<char, SV>);
    }
}

TEST_CASE("is_variant_alternative - cv-qualified and reference variant types", "[is_variant_alternative][cv]")
{
    using V = fxt::variant<int, double>;

    SECTION("const variant")
    {
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, const V>);
    }

    SECTION("reference to variant")
    {
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, V&>);
        STATIC_REQUIRE(fxt::is_variant_alternative_v<double, const V&>);
    }
}

TEST_CASE("variant_alternative_count - counts duplicate alternative types", "[variant_alternative_count][variant]")
{
    SECTION("unique alternatives each count once")
    {
        using V = fxt::variant<int, double, std::string>;
        STATIC_REQUIRE(fxt::variant_alternative_count_v<int, V> == 1);
        STATIC_REQUIRE(fxt::variant_alternative_count_v<double, V> == 1);
    }

    SECTION("duplicate alternatives are counted, not just detected")
    {
        // std::variant permits repeated alternative types (they are only
        // distinguished by index, not by std::get<T>/std::holds_alternative<T>).
        using V = fxt::variant<int, int, double, int>;
        STATIC_REQUIRE(fxt::variant_alternative_count_v<int, V> == 3);
        STATIC_REQUIRE(fxt::variant_alternative_count_v<double, V> == 1);
    }

    SECTION("absent type counts zero")
    {
        using V = fxt::variant<int, double>;
        STATIC_REQUIRE(fxt::variant_alternative_count_v<std::string, V> == 0);
    }

    SECTION("is_variant_alternative is equivalent to count > 0")
    {
        using V = fxt::variant<int, int, double>;
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, V> == (fxt::variant_alternative_count_v<int, V> > 0));
        STATIC_REQUIRE(fxt::is_variant_alternative_v<float, V> == (fxt::variant_alternative_count_v<float, V> > 0));
    }
}

TEST_CASE("is_variant_alternative - type-level question, independent of the held value", "[is_variant_alternative][semantics]")
{
    SECTION("differs from fxt::holds_alternative, which checks the current value")
    {
        using V = fxt::variant<int, std::string>;
        V v{42};

        // The variant TYPE can hold a std::string...
        STATIC_REQUIRE(fxt::is_variant_alternative_v<std::string, V>);
        // ...but this particular instance does not, right now.
        REQUIRE_FALSE(fxt::holds_alternative<std::string>(v));

        // Meanwhile it does currently hold an int, and can hold one.
        STATIC_REQUIRE(fxt::is_variant_alternative_v<int, V>);
        REQUIRE(fxt::holds_alternative<int>(v));
    }
}

// A generic function constrained on is_variant_alternative_v, exercising it as
// a requires-clause (the primary intended use case for the trait).
namespace
{
    template<typename Variant>
        requires fxt::is_variant_alternative_v<std::string, Variant>
    constexpr bool can_hold_a_string(const Variant&) { return true; }
}

TEST_CASE("is_variant_alternative - usable as a requires-clause constraint", "[is_variant_alternative][constraint]")
{
    SECTION("constrained function is callable when the variant can hold the type")
    {
        fxt::variant<int, std::string> v{1};
        REQUIRE(can_hold_a_string(v));
    }

    SECTION("constrained overload set correctly excludes non-matching variants")
    {
        using NoString = fxt::variant<int, double>;
        STATIC_REQUIRE_FALSE(fxt::is_variant_alternative_v<std::string, NoString>);
    }
}
