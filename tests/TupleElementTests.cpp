//
// Test suite for fxt::tuple_element and fxt::tuple_element_t
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <type_traits>

// ============================================================================
// Tests for fxt::tuple (std::tuple)
// ============================================================================

TEST_CASE("tuple_element - fxt::tuple basic types", "[tuple_element][tuple]")
{
    SECTION("single element tuple")
    {
        using T = fxt::tuple<int>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, int>);
        static_assert(std::is_same_v<typename fxt::tuple_element<0, T>::type, int>);
    }

    SECTION("two element tuple")
    {
        using T = fxt::tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, double>);
    }

    SECTION("three element tuple with mixed types")
    {
        using T = fxt::tuple<int, std::string, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, std::string>);
        static_assert(std::is_same_v<fxt::tuple_element_t<2, T>, double>);
    }

    SECTION("tuple with bool and char")
    {
        using T = fxt::tuple<bool, char, int, double, std::string>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, bool>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, char>);
        static_assert(std::is_same_v<fxt::tuple_element_t<2, T>, int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<3, T>, double>);
        static_assert(std::is_same_v<fxt::tuple_element_t<4, T>, std::string>);
    }
}

TEST_CASE("tuple_element - fxt::tuple with cv-qualifiers", "[tuple_element][tuple][cv]")
{
    SECTION("const tuple")
    {
        using T = const fxt::tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, const int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, const double>);
    }

    SECTION("volatile tuple")
    {
        using T = volatile fxt::tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, volatile int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, volatile double>);
    }

    SECTION("const volatile tuple")
    {
        using T = const volatile fxt::tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, const volatile int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, const volatile double>);
    }
}

// ============================================================================
// Tests for fxt::flat_tuple
// ============================================================================

TEST_CASE("tuple_element - fxt::flat_tuple basic types", "[tuple_element][flat_tuple]")
{
    SECTION("single element flat_tuple")
    {
        using T = fxt::flat_tuple<int>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, int>);
        static_assert(std::is_same_v<typename fxt::tuple_element<0, T>::type, int>);
    }

    SECTION("two element flat_tuple")
    {
        using T = fxt::flat_tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, double>);
    }

    SECTION("three element flat_tuple")
    {
        using T = fxt::flat_tuple<double, double, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, double>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, double>);
        static_assert(std::is_same_v<fxt::tuple_element_t<2, T>, double>);
    }

    SECTION("flat_tuple with multiple types")
    {
        using T = fxt::flat_tuple<int, float, double, long, char>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, float>);
        static_assert(std::is_same_v<fxt::tuple_element_t<2, T>, double>);
        static_assert(std::is_same_v<fxt::tuple_element_t<3, T>, long>);
        static_assert(std::is_same_v<fxt::tuple_element_t<4, T>, char>);
    }
}

TEST_CASE("tuple_element - fxt::flat_tuple with cv-qualifiers", "[tuple_element][flat_tuple][cv]")
{
    SECTION("const flat_tuple")
    {
        using T = const fxt::flat_tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, const int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, const double>);
    }

    SECTION("volatile flat_tuple")
    {
        using T = volatile fxt::flat_tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, volatile int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, volatile double>);
    }

    SECTION("const volatile flat_tuple")
    {
        using T = const volatile fxt::flat_tuple<int, double>;
        static_assert(std::is_same_v<fxt::tuple_element_t<0, T>, const volatile int>);
        static_assert(std::is_same_v<fxt::tuple_element_t<1, T>, const volatile double>);
    }
}

// ============================================================================
// Compatibility tests - ensure fxt::tuple_element matches std::tuple_element
// ============================================================================

TEST_CASE("tuple_element - compatibility with std::tuple_element", "[tuple_element][compatibility]")
{
    SECTION("fxt::tuple behaves like std::tuple")
    {
        using FxtTuple = fxt::tuple<int, double, std::string>;
        using StdTuple = std::tuple<int, double, std::string>;

        static_assert(std::is_same_v<
            fxt::tuple_element_t<0, FxtTuple>,
            std::tuple_element_t<0, StdTuple>
        >);

        static_assert(std::is_same_v<
            fxt::tuple_element_t<1, FxtTuple>,
            std::tuple_element_t<1, StdTuple>
        >);

        static_assert(std::is_same_v<
            fxt::tuple_element_t<2, FxtTuple>,
            std::tuple_element_t<2, StdTuple>
        >);
    }

    SECTION("const qualification matches")
    {
        using FxtTuple = const fxt::tuple<int, double>;
        using StdTuple = const std::tuple<int, double>;

        static_assert(std::is_same_v<
            fxt::tuple_element_t<0, FxtTuple>,
            std::tuple_element_t<0, StdTuple>
        >);

        static_assert(std::is_same_v<
            fxt::tuple_element_t<1, FxtTuple>,
            std::tuple_element_t<1, StdTuple>
        >);
    }
}

// ============================================================================
// Mixed usage tests
// ============================================================================

TEST_CASE("tuple_element - practical usage scenarios", "[tuple_element][practical]")
{
    SECTION("use with decltype")
    {
        fxt::tuple<int, double, std::string> t{42, 3.14, "hello"};
        using FirstType = fxt::tuple_element_t<0, decltype(t)>;
        using SecondType = fxt::tuple_element_t<1, decltype(t)>;
        using ThirdType = fxt::tuple_element_t<2, decltype(t)>;

        static_assert(std::is_same_v<FirstType, int>);
        static_assert(std::is_same_v<SecondType, double>);
        static_assert(std::is_same_v<ThirdType, std::string>);
    }

    SECTION("use with flat_tuple and decltype")
    {
        fxt::flat_tuple<double, double, double> ft{1.0, 2.0, 3.0};
        using FirstType = fxt::tuple_element_t<0, decltype(ft)>;
        using SecondType = fxt::tuple_element_t<1, decltype(ft)>;
        using ThirdType = fxt::tuple_element_t<2, decltype(ft)>;

        static_assert(std::is_same_v<FirstType, double>);
        static_assert(std::is_same_v<SecondType, double>);
        static_assert(std::is_same_v<ThirdType, double>);
    }

    SECTION("type extraction from template parameter")
    {
        using MyTuple = fxt::tuple<int, float, double, char, bool>;

        // Extract middle element
        using MiddleType = fxt::tuple_element_t<2, MyTuple>;
        static_assert(std::is_same_v<MiddleType, double>);

        // Extract last element
        using LastType = fxt::tuple_element_t<4, MyTuple>;
        static_assert(std::is_same_v<LastType, bool>);
    }
}

// ============================================================================
// Runtime tests to ensure the types actually work
// ============================================================================

TEST_CASE("tuple_element - runtime type verification", "[tuple_element][runtime]")
{
    SECTION("fxt::tuple runtime check")
    {
        fxt::tuple<int, double, std::string> t{42, 3.14, "test"};

        // Use tuple_element_t to declare variables
        fxt::tuple_element_t<0, decltype(t)> first = fxt::get<0>(t);
        fxt::tuple_element_t<1, decltype(t)> second = fxt::get<1>(t);
        fxt::tuple_element_t<2, decltype(t)> third = fxt::get<2>(t);

        REQUIRE(first == 42);
        REQUIRE(second == 3.14);
        REQUIRE(third == "test");
    }

    SECTION("fxt::flat_tuple runtime check")
    {
        fxt::flat_tuple<int, int, int> ft{10, 20, 30};

        // Use tuple_element_t to declare variables
        fxt::tuple_element_t<0, decltype(ft)> first = fxt::get<0>(ft);
        fxt::tuple_element_t<1, decltype(ft)> second = fxt::get<1>(ft);
        fxt::tuple_element_t<2, decltype(ft)> third = fxt::get<2>(ft);

        REQUIRE(first == 10);
        REQUIRE(second == 20);
        REQUIRE(third == 30);
    }
}

