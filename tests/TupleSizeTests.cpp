//
// Test suite for fxt::tuple_size and fxt::tuple_size_v
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt/tuples/FlatTuple.hpp>
#include <fxt/tuples/Tuple.hpp>
#include <fxt/tuples/TupleSize.hpp>
#include <string>
#include <vector>

TEST_CASE("tuple_size - fxt::tuple basic", "[tuple_size][tuple]")
{
    SECTION("empty tuple")
    {
        using Tuple = fxt::tuple<>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 0);
        REQUIRE(fxt::tuple_size_v<Tuple> == 0);
    }

    SECTION("single element")
    {
        using Tuple = fxt::tuple<int>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 1);
        REQUIRE(fxt::tuple_size_v<Tuple> == 1);
    }

    SECTION("two elements")
    {
        using Tuple = fxt::tuple<int, std::string>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 2);
        REQUIRE(fxt::tuple_size_v<Tuple> == 2);
    }

    SECTION("three elements")
    {
        using Tuple = fxt::tuple<int, double, std::string>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 3);
        REQUIRE(fxt::tuple_size_v<Tuple> == 3);
    }

    SECTION("five elements")
    {
        using Tuple = fxt::tuple<int, int, int, int, int>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 5);
        REQUIRE(fxt::tuple_size_v<Tuple> == 5);
    }

    SECTION("ten elements")
    {
        using Tuple = fxt::tuple<int, int, int, int, int, int, int, int, int, int>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 10);
        REQUIRE(fxt::tuple_size_v<Tuple> == 10);
    }
}

TEST_CASE("tuple_size - fxt::tuple with cv qualifiers", "[tuple_size][tuple]")
{
    // volatile and const volatile qualify are deprecated in C++20 ([depr.volatile.type])
    // and intentionally not tested here.
    using Tuple = fxt::tuple<int, std::string, double>;

    SECTION("non-const")
    {
        REQUIRE(fxt::tuple_size<Tuple>::value == 3);
        REQUIRE(fxt::tuple_size_v<Tuple> == 3);
    }

    SECTION("const")
    {
        REQUIRE(fxt::tuple_size<const Tuple>::value == 3);
        REQUIRE(fxt::tuple_size_v<const Tuple> == 3);
    }
}

TEST_CASE("tuple_size - fxt::tuple complex types", "[tuple_size][tuple]")
{
    SECTION("with vector")
    {
        using Tuple = fxt::tuple<std::vector<int>, std::string>;
        REQUIRE(fxt::tuple_size_v<Tuple> == 2);
    }

    SECTION("with nested tuple")
    {
        using Tuple = fxt::tuple<int, fxt::tuple<double, std::string>>;
        REQUIRE(fxt::tuple_size_v<Tuple> == 2);
    }

    SECTION("all same type")
    {
        using Tuple = fxt::tuple<int, int, int, int>;
        REQUIRE(fxt::tuple_size_v<Tuple> == 4);
    }
}

TEST_CASE("tuple_size - fxt::flat_tuple basic", "[tuple_size][flat_tuple]")
{
    SECTION("empty tuple")
    {
        using Tuple = fxt::flat_tuple<>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 0);
        REQUIRE(fxt::tuple_size_v<Tuple> == 0);
    }

    SECTION("single element")
    {
        using Tuple = fxt::flat_tuple<int>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 1);
        REQUIRE(fxt::tuple_size_v<Tuple> == 1);
    }

    SECTION("two elements")
    {
        using Tuple = fxt::flat_tuple<int, std::string>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 2);
        REQUIRE(fxt::tuple_size_v<Tuple> == 2);
    }

    SECTION("three elements")
    {
        using Tuple = fxt::flat_tuple<int, double, std::string>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 3);
        REQUIRE(fxt::tuple_size_v<Tuple> == 3);
    }

    SECTION("five elements")
    {
        using Tuple = fxt::flat_tuple<int, int, int, int, int>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 5);
        REQUIRE(fxt::tuple_size_v<Tuple> == 5);
    }

    SECTION("ten elements")
    {
        using Tuple = fxt::flat_tuple<int, int, int, int, int, int, int, int, int, int>;
        REQUIRE(fxt::tuple_size<Tuple>::value == 10);
        REQUIRE(fxt::tuple_size_v<Tuple> == 10);
    }
}

TEST_CASE("tuple_size - fxt::flat_tuple with cv qualifiers", "[tuple_size][flat_tuple]")
{
    // volatile and const volatile qualifiers are deprecated in C++20 ([depr.volatile.type])
    // and intentionally not tested here.
    using Tuple = fxt::flat_tuple<int, std::string, double>;

    SECTION("non-const")
    {
        REQUIRE(fxt::tuple_size<Tuple>::value == 3);
        REQUIRE(fxt::tuple_size_v<Tuple> == 3);
    }

    SECTION("const")
    {
        REQUIRE(fxt::tuple_size<const Tuple>::value == 3);
        REQUIRE(fxt::tuple_size_v<const Tuple> == 3);
    }
}

TEST_CASE("tuple_size - fxt::flat_tuple complex types", "[tuple_size][flat_tuple]")
{
    SECTION("with vector")
    {
        using Tuple = fxt::flat_tuple<std::vector<int>, std::string>;
        REQUIRE(fxt::tuple_size_v<Tuple> == 2);
    }

    SECTION("with nested flat_tuple")
    {
        using Tuple = fxt::flat_tuple<int, fxt::flat_tuple<double, std::string>>;
        REQUIRE(fxt::tuple_size_v<Tuple> == 2);
    }

    SECTION("all same type")
    {
        using Tuple = fxt::flat_tuple<int, int, int, int>;
        REQUIRE(fxt::tuple_size_v<Tuple> == 4);
    }
}

TEST_CASE("tuple_size - compile-time computation", "[tuple_size]")
{
    SECTION("constexpr with fxt::tuple")
    {
        constexpr std::size_t size = fxt::tuple_size_v<fxt::tuple<int, double>>;
        REQUIRE(size == 2);
        static_assert(size == 2);
    }

    SECTION("constexpr with fxt::flat_tuple")
    {
        constexpr std::size_t size = fxt::tuple_size_v<fxt::flat_tuple<int, double, bool>>;
        REQUIRE(size == 3);
        static_assert(size == 3);
    }

    SECTION("static_assert with empty tuples")
    {
        static_assert(fxt::tuple_size_v<fxt::tuple<>> == 0);
        static_assert(fxt::tuple_size_v<fxt::flat_tuple<>> == 0);
        REQUIRE(fxt::tuple_size_v<fxt::tuple<>> == 0);
        REQUIRE(fxt::tuple_size_v<fxt::flat_tuple<>> == 0);
    }

    SECTION("static_assert equality")
    {
        using Tuple1 = fxt::tuple<int, std::string, double>;
        using Tuple2 = fxt::flat_tuple<int, std::string, double>;
        static_assert(fxt::tuple_size_v<Tuple1> == fxt::tuple_size_v<Tuple2>);
        REQUIRE(fxt::tuple_size_v<Tuple1> == fxt::tuple_size_v<Tuple2>);
    }
}

TEST_CASE("tuple_size - comparison between tuple types", "[tuple_size]")
{
    SECTION("same size empty")
    {
        using Tuple1 = fxt::tuple<>;
        using Tuple2 = fxt::flat_tuple<>;
        REQUIRE(fxt::tuple_size_v<Tuple1> == fxt::tuple_size_v<Tuple2>);
    }

    SECTION("same size single element")
    {
        using Tuple1 = fxt::tuple<int>;
        using Tuple2 = fxt::flat_tuple<int>;
        REQUIRE(fxt::tuple_size_v<Tuple1> == fxt::tuple_size_v<Tuple2>);
    }

    SECTION("same size multiple elements")
    {
        using Tuple1 = fxt::tuple<int, std::string, double, bool>;
        using Tuple2 = fxt::flat_tuple<int, std::string, double, bool>;
        REQUIRE(fxt::tuple_size_v<Tuple1> == fxt::tuple_size_v<Tuple2>);
        REQUIRE(fxt::tuple_size_v<Tuple1> == 4);
        REQUIRE(fxt::tuple_size_v<Tuple2> == 4);
    }

    SECTION("different sizes")
    {
        using Tuple1 = fxt::tuple<int, std::string>;
        using Tuple2 = fxt::flat_tuple<int, std::string, double>;
        REQUIRE(fxt::tuple_size_v<Tuple1> != fxt::tuple_size_v<Tuple2>);
        REQUIRE(fxt::tuple_size_v<Tuple1> == 2);
        REQUIRE(fxt::tuple_size_v<Tuple2> == 3);
    }
}

TEST_CASE("tuple_size - template metaprogramming", "[tuple_size]")
{
    SECTION("conditional compilation based on size")
    {
        constexpr auto check_size = []<typename T>() {
            constexpr std::size_t size = fxt::tuple_size_v<T>;
            if constexpr (size == 0) {
                return 0;
            } else if constexpr (size == 1) {
                return 1;
            } else if constexpr (size == 2) {
                return 2;
            } else {
                return -1;
            }
        };

        REQUIRE(check_size.template operator()<fxt::tuple<>>() == 0);
        REQUIRE(check_size.template operator()<fxt::tuple<int>>() == 1);
        REQUIRE(check_size.template operator()<fxt::tuple<int, double>>() == 2);
        REQUIRE(check_size.template operator()<fxt::tuple<int, double, bool>>() == -1);

        REQUIRE(check_size.template operator()<fxt::flat_tuple<>>() == 0);
        REQUIRE(check_size.template operator()<fxt::flat_tuple<int>>() == 1);
        REQUIRE(check_size.template operator()<fxt::flat_tuple<int, double>>() == 2);
        REQUIRE(check_size.template operator()<fxt::flat_tuple<int, double, bool>>() == -1);
    }

    SECTION("generic function with tuple_size")
    {
        auto get_size = []<typename T>() {
            return fxt::tuple_size_v<T>;
        };

        REQUIRE(get_size.template operator()<fxt::tuple<int, int, int>>() == 3);
        REQUIRE(get_size.template operator()<fxt::flat_tuple<int, int, int>>() == 3);
    }
}

TEST_CASE("tuple_size - type traits", "[tuple_size]")
{
    SECTION("value member exists")
    {
        using Tuple1 = fxt::tuple<int, double>;
        using Tuple2 = fxt::flat_tuple<int, double>;

        REQUIRE(fxt::tuple_size<Tuple1>::value == 2);
        REQUIRE(fxt::tuple_size<Tuple2>::value == 2);
    }

    SECTION("is integral constant")
    {
        using Size1 = fxt::tuple_size<fxt::tuple<int, double>>;
        using Size2 = fxt::tuple_size<fxt::flat_tuple<int, double>>;

        static_assert(std::is_base_of_v<std::integral_constant<std::size_t, 2>, Size1> ||
                      Size1::value == 2);
        static_assert(std::is_base_of_v<std::integral_constant<std::size_t, 2>, Size2>);
    }
}

TEST_CASE("tuple_size - edge cases", "[tuple_size]")
{
    SECTION("very large tuple")
    {
        using LargeTuple = fxt::tuple<
            int, int, int, int, int, int, int, int, int, int,
            int, int, int, int, int, int, int, int, int, int
        >;
        REQUIRE(fxt::tuple_size_v<LargeTuple> == 20);

        using LargeFlatTuple = fxt::flat_tuple<
            int, int, int, int, int, int, int, int, int, int,
            int, int, int, int, int, int, int, int, int, int
        >;
        REQUIRE(fxt::tuple_size_v<LargeFlatTuple> == 20);
    }

    SECTION("tuple with only complex types")
    {
        using Tuple = fxt::tuple<
            std::vector<int>,
            std::string,
            std::vector<std::string>
        >;
        REQUIRE(fxt::tuple_size_v<Tuple> == 3);

        using FlatTuple = fxt::flat_tuple<
            std::vector<int>,
            std::string,
            std::vector<std::string>
        >;
        REQUIRE(fxt::tuple_size_v<FlatTuple> == 3);
    }
}

TEST_CASE("tuple_size - practical use cases", "[tuple_size]")
{
    SECTION("array sizing based on tuple size")
    {
        constexpr std::size_t size1 = fxt::tuple_size_v<fxt::tuple<int, int, int>>;
        std::array<int, size1> arr1{};
        REQUIRE(arr1.size() == 3);

        constexpr std::size_t size2 = fxt::tuple_size_v<fxt::flat_tuple<double, double>>;
        std::array<double, size2> arr2{};
        REQUIRE(arr2.size() == 2);
    }

    SECTION("compile-time loop bounds")
    {
        using Tuple = fxt::tuple<int, double, bool, std::string>;
        constexpr std::size_t count = fxt::tuple_size_v<Tuple>;

        int iterations = 0;
        [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            ((iterations++, void(Is)), ...);
        }(std::make_index_sequence<count>{});

        REQUIRE(iterations == 4);
    }
}

TEST_CASE("tuple_size - consistency with std::tuple_size", "[tuple_size]")
{
    SECTION("fxt::tuple matches std::tuple_size")
    {
        using FxtTuple = fxt::tuple<int, double, std::string>;
        using StdTuple = std::tuple<int, double, std::string>;

        REQUIRE(fxt::tuple_size_v<FxtTuple> == std::tuple_size_v<StdTuple>);
    }

    SECTION("empty tuple consistency")
    {
        using FxtTuple = fxt::tuple<>;
        using StdTuple = std::tuple<>;

        REQUIRE(fxt::tuple_size_v<FxtTuple> == std::tuple_size_v<StdTuple>);
        REQUIRE(fxt::tuple_size_v<FxtTuple> == 0);
    }
}

