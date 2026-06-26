//
// Test suite for fxt::tuple_for_each and fxt::mtuple_for_each
//

#include "Catch2/catch_amalgamated.hpp"
#include <fxt.hpp>
#include <string>
#include <vector>

// ============================================================================
// fxt::tuple_for_each — direct call
// ============================================================================

TEST_CASE("tuple_for_each - direct call visits every element in order", "[tuple_for_each]")
{
    SECTION("fxt::tuple")
    {
        std::vector<int> visited;
        auto t = fxt::make_tuple(1, 2, 3);
        fxt::tuple_for_each(t, [&](const auto& x) { visited.push_back(x); });
        REQUIRE(visited == std::vector<int>{1, 2, 3});
    }

    SECTION("fxt::flat_tuple")
    {
        std::vector<int> visited;
        auto ft = fxt::make_flat_tuple(10, 20, 30);
        fxt::tuple_for_each(ft, [&](const auto& x) { visited.push_back(x); });
        REQUIRE(visited == std::vector<int>{10, 20, 30});
    }

    SECTION("mixed types — visits all three elements")
    {
        int count = 0;
        auto t = fxt::make_tuple(42, 3.14, std::string("hi"));
        fxt::tuple_for_each(t, [&](const auto&) { ++count; });
        REQUIRE(count == 3);
    }
}

TEST_CASE("tuple_for_each - direct call with mutable lambda mutates elements", "[tuple_for_each]")
{
    auto t = fxt::make_tuple(1, 2, 3);
    fxt::tuple_for_each(t, [](auto& x) { x *= 10; });
    REQUIRE(fxt::get<0>(t) == 10);
    REQUIRE(fxt::get<1>(t) == 20);
    REQUIRE(fxt::get<2>(t) == 30);
}

TEST_CASE("tuple_for_each - empty tuple visits nothing", "[tuple_for_each]")
{
    int count = 0;
    fxt::tuple<> empty;
    fxt::tuple_for_each(empty, [&](const auto&) { ++count; });
    REQUIRE(count == 0);
}

TEST_CASE("tuple_for_each - accumulates sum via capture", "[tuple_for_each]")
{
    int sum = 0;
    fxt::tuple_for_each(fxt::make_tuple(10, 20, 30), [&](int x) { sum += x; });
    REQUIRE(sum == 60);
}

// ============================================================================
// fxt::tuple_for_each — curried / pipeline form
// ============================================================================

TEST_CASE("tuple_for_each - curried form visits every element", "[tuple_for_each]")
{
    std::vector<int> visited;
    fxt::make_tuple(4, 5, 6)
        | fxt::tuple_for_each([&](int x) { visited.push_back(x); });
    REQUIRE(visited == std::vector<int>{4, 5, 6});
}

TEST_CASE("tuple_for_each - curried form with flat_tuple", "[tuple_for_each]")
{
    int sum = 0;
    fxt::make_flat_tuple(1, 2, 3, 4)
        | fxt::tuple_for_each([&](int x) { sum += x; });
    REQUIRE(sum == 10);
}

TEST_CASE("tuple_for_each - stored adaptor is reusable", "[tuple_for_each]")
{
    int total = 0;
    auto acc = fxt::tuple_for_each([&](int x) { total += x; });

    fxt::make_tuple(1, 2, 3) | acc;
    fxt::make_tuple(10, 20)  | acc;

    REQUIRE(total == 36);
}

TEST_CASE("tuple_for_each - can chain with other tuple operations", "[tuple_for_each]")
{
    std::vector<int> log;
    fxt::make_tuple(1, 2, 3, 4, 5)
        | fxt::tuple_transform([](int x) { return x * 2; })
        | fxt::tuple_for_each([&](int x) { log.push_back(x); });
    REQUIRE(log == std::vector<int>{2, 4, 6, 8, 10});
}

// ============================================================================
// fxt::tuple_for_each — value categories
// ============================================================================

TEST_CASE("tuple_for_each - type-correctness on lvalue tuple", "[tuple_for_each]")
{
    fxt::tuple<int, std::string> t(42, "hello");
    static_assert(std::is_same_v<decltype(fxt::get<0>(t)), int&>);
    fxt::tuple_for_each(t, [](auto& x) { (void)x; });
}

TEST_CASE("tuple_for_each - const tuple elements are const", "[tuple_for_each]")
{
    const fxt::tuple<int, double> t(1, 2.0);
    bool called = false;
    fxt::tuple_for_each(t, [&](const auto& x) {
        static_assert(std::is_const_v<std::remove_reference_t<decltype(x)>>);
        called = true;
        (void)x;
    });
    REQUIRE(called);
}

// ============================================================================
// fxt::mtuple_for_each — expected
// ============================================================================

TEST_CASE("mtuple_for_each - applies fn to elements when expected has value", "[mtuple_for_each]")
{
    std::vector<int> visited;
    auto exp = fxt::expected<fxt::tuple<int, int, int>, std::string>{
        fxt::make_tuple(1, 2, 3)};

    exp | fxt::mtuple_for_each([&](int x) { visited.push_back(x); });

    REQUIRE(visited == std::vector<int>{1, 2, 3});
}

TEST_CASE("mtuple_for_each - does not call fn when expected has error", "[mtuple_for_each]")
{
    int count = 0;
    auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::unexpected("oops")};

    exp | fxt::mtuple_for_each([&](int) { ++count; });

    REQUIRE(count == 0);
}

TEST_CASE("mtuple_for_each - passes monad through so pipeline can continue", "[mtuple_for_each]")
{
    auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::make_tuple(3, 4)};

    auto result = exp
        | fxt::mtuple_for_each([](int) {})        // side-effect step
        | fxt::mtuple_apply([](int a, int b) { return a + b; });

    REQUIRE(result.has_value());
    REQUIRE(*result == 7);
}

TEST_CASE("mtuple_for_each - error propagates through pipeline", "[mtuple_for_each]")
{
    auto exp = fxt::expected<fxt::tuple<int, int>, std::string>{
        fxt::unexpected("error")};

    auto result = exp
        | fxt::mtuple_for_each([](int) {})
        | fxt::mtuple_apply([](int a, int b) { return a + b; });

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "error");
}

// ============================================================================
// fxt::mtuple_for_each — optional
// ============================================================================

TEST_CASE("mtuple_for_each - applies fn when optional has value", "[mtuple_for_each]")
{
    std::vector<int> visited;
    auto opt = fxt::optional<fxt::tuple<int, int>>{fxt::make_tuple(5, 10)};

    opt | fxt::mtuple_for_each([&](int x) { visited.push_back(x); });

    REQUIRE(visited == std::vector<int>{5, 10});
}

TEST_CASE("mtuple_for_each - does not call fn on empty optional", "[mtuple_for_each]")
{
    int count = 0;
    fxt::optional<fxt::tuple<int, int>> opt{};

    opt | fxt::mtuple_for_each([&](int) { ++count; });

    REQUIRE(count == 0);
}

TEST_CASE("mtuple_for_each - works with flat_tuple inside monad", "[mtuple_for_each]")
{
    int sum = 0;
    auto opt = fxt::optional<fxt::flat_tuple<int, int, int>>{
        fxt::make_flat_tuple(1, 2, 3)};

    opt | fxt::mtuple_for_each([&](int x) { sum += x; });

    REQUIRE(sum == 6);
}
