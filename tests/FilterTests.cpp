//
// Created by kenne on 28/06/2026.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::filter with fxt::optional", "[filter][optional]")
{
    SECTION("passes value through when predicate holds")
    {
        fxt::optional<int> opt = 4;
        auto result = opt | fxt::filter([](int x) { return x % 2 == 0; });
        REQUIRE(result.has_value());
        REQUIRE(*result == 4);
    }

    SECTION("returns nullopt when predicate fails")
    {
        fxt::optional<int> opt = 3;
        auto result = opt | fxt::filter([](int x) { return x % 2 == 0; });
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("propagates disengaged optional without calling predicate")
    {
        bool called = false;
        fxt::optional<int> opt = fxt::nullopt;
        auto result = opt | fxt::filter([&called](int) { called = true; return true; });
        REQUIRE_FALSE(result.has_value());
        REQUIRE_FALSE(called);
    }

    SECTION("stored adaptor can be applied multiple times")
    {
        auto positive = fxt::filter([](int x) { return x > 0; });
        REQUIRE((fxt::optional<int>{  5} | positive).has_value());
        REQUIRE_FALSE((fxt::optional<int>{ -1} | positive).has_value());
        REQUIRE_FALSE((fxt::optional<int>{} | positive).has_value());
    }

    SECTION("piped chaining with other adaptors")
    {
        fxt::optional<int> opt = 6;
        auto result = opt
                    | fxt::filter([](int x) { return x % 2 == 0; })
                    | fxt::transform([](int x) { return x * 10; });
        REQUIRE(result.has_value());
        REQUIRE(*result == 60);
    }
}

TEST_CASE("fxt::filter with fxt::expected", "[filter][expected]")
{
    SECTION("passes value through when predicate holds")
    {
        fxt::expected<int, std::string> exp = 4;
        auto result = exp | fxt::filter([](int x) { return x % 2 == 0; }, std::string{"not even"});
        REQUIRE(result.has_value());
        REQUIRE(*result == 4);
    }

    SECTION("returns unexpected(err) when predicate fails")
    {
        fxt::expected<int, std::string> exp = 3;
        auto result = exp | fxt::filter([](int x) { return x % 2 == 0; }, std::string{"not even"});
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not even");
    }

    SECTION("propagates existing error without calling predicate")
    {
        bool called = false;
        fxt::expected<int, std::string> exp = fxt::unexpected<std::string>{"prior error"};
        auto result = exp | fxt::filter([&called](int) { called = true; return true; }, std::string{"new error"});
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "prior error");
        REQUIRE_FALSE(called);
    }

    SECTION("stored adaptor can be applied multiple times")
    {
        auto positive = fxt::filter([](int x) { return x > 0; }, std::string{"not positive"});
        REQUIRE((fxt::expected<int, std::string>{ 5} | positive).has_value());
        REQUIRE_FALSE((fxt::expected<int, std::string>{-1} | positive).has_value());
        REQUIRE((fxt::expected<int, std::string>{-1} | positive).error() == "not positive");
    }

    SECTION("piped chaining with other adaptors")
    {
        fxt::expected<int, std::string> exp = 6;
        auto result = exp
                    | fxt::filter([](int x) { return x % 2 == 0; }, std::string{"not even"})
                    | fxt::transform([](int x) { return x * 10; });
        REQUIRE(result.has_value());
        REQUIRE(*result == 60);
    }
}
