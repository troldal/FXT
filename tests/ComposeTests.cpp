//
// Test suite for fxt::compose
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <functional>
#include <string>
#include <type_traits>

TEST_CASE("compose - two functions apply right-to-left", "[compose]")
{
    auto negate = [](int x) { return -x; };
    auto square = [](int x) { return x * x; };

    SECTION("compose(f, g)(x) == f(g(x))")
    {
        auto h = fxt::compose(negate, square);
        REQUIRE(h(3) == -9);
        REQUIRE(h(-4) == -16);
    }

    SECTION("argument order matters (composition is not commutative in general)")
    {
        auto h1 = fxt::compose(negate, square);   // negate(square(x))
        auto h2 = fxt::compose(square, negate);   // square(negate(x))
        REQUIRE(h1(3) == -9);
        REQUIRE(h2(3) == 9);
    }
}

TEST_CASE("compose - three or more functions", "[compose]")
{
    auto negate = [](int x) { return -x; };
    auto square = [](int x) { return x * x; };
    auto to_string = [](int x) { return std::to_string(x); };

    SECTION("compose(f, g, h)(x) == f(g(h(x)))")
    {
        auto pipeline = fxt::compose(to_string, negate, square);
        REQUIRE(pipeline(3) == "-9");
    }

    SECTION("four functions")
    {
        auto add_one = [](int x) { return x + 1; };
        auto pipeline = fxt::compose(to_string, negate, square, add_one);
        // add_one(2)=3, square(3)=9, negate(9)=-9, to_string(-9)="-9"
        REQUIRE(pipeline(2) == "-9");
    }
}

TEST_CASE("compose - single function is returned unchanged", "[compose]")
{
    auto negate = [](int x) { return -x; };
    auto same = fxt::compose(negate);
    REQUIRE(same(5) == -5);
}

TEST_CASE("compose - innermost function may take multiple arguments", "[compose]")
{
    auto negate = [](int x) { return -x; };
    auto add = [](int a, int b) { return a + b; };

    auto h = fxt::compose(negate, add);
    REQUIRE(h(3, 4) == -7);
}

TEST_CASE("compose - works with function pointers", "[compose]")
{
    auto negate = [](int x) { return -x; };
    int (*triple_fp)(int) = [](int x) { return x * 3; };

    auto h = fxt::compose(negate, triple_fp);
    REQUIRE(h(2) == -6);
}

TEST_CASE("compose - works with std::function", "[compose]")
{
    std::function<int(int)> negate = [](int x) { return -x; };
    std::function<int(int)> square = [](int x) { return x * x; };

    auto h = fxt::compose(negate, square);
    REQUIRE(h(4) == -16);
}

TEST_CASE("compose - stored adaptor is reusable", "[compose]")
{
    auto negate = [](int x) { return -x; };
    auto square = [](int x) { return x * x; };
    auto h = fxt::compose(negate, square);

    REQUIRE(h(2) == -4);
    REQUIRE(h(3) == -9);
    REQUIRE(h(4) == -16);
}

TEST_CASE("compose - constexpr evaluation", "[compose][constexpr]")
{
    SECTION("two-function composition is usable in a constant expression")
    {
        constexpr auto inc = [](int x) { return x + 1; };
        constexpr auto dbl = [](int x) { return x * 2; };
        constexpr auto h = fxt::compose(dbl, inc);
        STATIC_REQUIRE(h(3) == 8); // dbl(inc(3)) == dbl(4) == 8
    }
}

TEST_CASE("compose - noexcept propagation", "[compose][noexcept]")
{
    SECTION("composition of noexcept callables is noexcept")
    {
        auto f = [](int x) noexcept { return x + 1; };
        auto g = [](int x) noexcept { return x * 2; };
        auto h = fxt::compose(f, g);
        STATIC_REQUIRE(noexcept(h(1)));
    }

    SECTION("composition involving a potentially-throwing callable is not noexcept")
    {
        auto f = [](int x) { return x + 1; };            // may throw (no noexcept)
        auto g = [](int x) noexcept { return x * 2; };
        auto h = fxt::compose(f, g);
        STATIC_REQUIRE_FALSE(noexcept(h(1)));
    }
}

TEST_CASE("compose - mixed types across the chain", "[compose]")
{
    auto length = [](const std::string& s) { return s.size(); };
    auto to_upper_len_message = [](std::size_t n) { return "length=" + std::to_string(n); };

    auto h = fxt::compose(to_upper_len_message, length);
    REQUIRE(h(std::string("hello")) == "length=5");
}
