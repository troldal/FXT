//
// Test suite for fxt::zip
//
// zip is variadic and constrained on monad_like, which is what clang-cl's
// Microsoft-ABI mangler could not handle ("cannot mangle this pack expansion
// yet"): these tests are also what keeps it compiling there.
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <memory>
#include <optional>
#include <string>

TEST_CASE("zip - expected values", "[zip][expected]")
{
    using E = std::string;

    SECTION("all values present gives a tuple of them, in order")
    {
        auto a = fxt::expected<int, E>{1};
        auto b = fxt::expected<double, E>{2.5};
        auto c = fxt::expected<std::string, E>{"three"};

        auto zipped = fxt::zip(a, b, c);

        REQUIRE(zipped.has_value());
        REQUIRE(fxt::get<0>(*zipped) == 1);
        REQUIRE(fxt::get<1>(*zipped) == 2.5);
        REQUIRE(fxt::get<2>(*zipped) == "three");
    }

    SECTION("a single value gives a one-element tuple")
    {
        auto zipped = fxt::zip(fxt::expected<int, E>{7});

        REQUIRE(zipped.has_value());
        REQUIRE(fxt::tuple_size_v<std::remove_cvref_t<decltype(*zipped)>> == 1);
        REQUIRE(fxt::get<0>(*zipped) == 7);
    }

    SECTION("the first error wins")
    {
        auto a = fxt::expected<int, E>{1};
        auto b = fxt::expected<double, E>{fxt::unexpected<E>{"second"}};
        auto c = fxt::expected<std::string, E>{fxt::unexpected<E>{"third"}};

        auto zipped = fxt::zip(a, b, c);

        REQUIRE_FALSE(zipped.has_value());
        REQUIRE(zipped.error() == "second");
    }

    SECTION("an error in the first argument wins too")
    {
        auto zipped = fxt::zip(fxt::expected<int, E>{fxt::unexpected<E>{"first"}},
                               fxt::expected<int, E>{2});

        REQUIRE_FALSE(zipped.has_value());
        REQUIRE(zipped.error() == "first");
    }

    SECTION("rvalues are moved, so move-only values zip")
    {
        auto zipped = fxt::zip(fxt::expected<std::unique_ptr<int>, E>{std::make_unique<int>(4)},
                               fxt::expected<int, E>{5});

        REQUIRE(zipped.has_value());
        REQUIRE(*fxt::get<0>(*zipped) == 4);
        REQUIRE(fxt::get<1>(*zipped) == 5);
    }
}

TEST_CASE("zip - optional values", "[zip][optional]")
{
    SECTION("all present")
    {
        auto zipped = fxt::zip(std::optional<int>{1}, std::optional<char>{'b'});

        REQUIRE(zipped.has_value());
        REQUIRE(fxt::get<0>(*zipped) == 1);
        REQUIRE(fxt::get<1>(*zipped) == 'b');
    }

    SECTION("one absent gives nothing")
    {
        auto zipped = fxt::zip(std::optional<int>{1}, std::optional<char>{});

        REQUIRE_FALSE(zipped.has_value());
    }
}

TEST_CASE("zip - combined with mapply", "[zip][mapply]")
{
    using E = std::string;

    auto sum = fxt::zip(fxt::expected<int, E>{2}, fxt::expected<int, E>{3})
             | fxt::mapply([](int a, int b) { return a + b; });

    REQUIRE(sum.has_value());
    REQUIRE(*sum == 5);
}
