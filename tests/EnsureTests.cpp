//
// Created by kenne on 28/06/2026.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::ensure with fxt::expected", "[ensure][expected]")
{
    SECTION("passes value through when predicate holds")
    {
        fxt::expected<int, std::string> exp = 42;
        auto result = exp | fxt::ensure([](int x) { return x > 0; }, std::string{"must be positive"});
        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("returns unexpected(err) when predicate fails")
    {
        fxt::expected<int, std::string> exp = -3;
        auto result = exp | fxt::ensure([](int x) { return x > 0; }, std::string{"must be positive"});
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "must be positive");
    }

    SECTION("propagates existing error without calling predicate")
    {
        bool called = false;
        fxt::expected<int, std::string> exp = fxt::unexpected<std::string>{"prior error"};
        auto result = exp | fxt::ensure([&called](int) { called = true; return true; },
                                        std::string{"new error"});
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "prior error");
        REQUIRE_FALSE(called);
    }

    SECTION("stored adaptor can be applied multiple times")
    {
        auto positive = fxt::ensure([](int x) { return x > 0; }, std::string{"not positive"});

        REQUIRE( (fxt::expected<int, std::string>{  5} | positive).has_value());
        REQUIRE(!(fxt::expected<int, std::string>{ -1} | positive).has_value());
        REQUIRE( (fxt::expected<int, std::string>{ -1} | positive).error() == "not positive");
        REQUIRE( (fxt::expected<int, std::string>{  1} | positive).has_value());
    }

    SECTION("chained ensures form a validation pipeline — first failure wins")
    {
        auto validate = [](int x) {
            return fxt::expected<int, std::string>{ x }
                 | fxt::ensure([](int v) { return v > 0;   }, std::string{"must be positive"})
                 | fxt::ensure([](int v) { return v < 100; }, std::string{"must be < 100"})
                 | fxt::ensure([](int v) { return v % 2 == 0; }, std::string{"must be even"});
        };

        auto r1 = validate(42);
        REQUIRE(r1.has_value());
        REQUIRE(*r1 == 42);

        auto r2 = validate(-1);
        REQUIRE_FALSE(r2.has_value());
        REQUIRE(r2.error() == "must be positive");   // first gate fires

        auto r3 = validate(200);
        REQUIRE_FALSE(r3.has_value());
        REQUIRE(r3.error() == "must be < 100");      // second gate fires

        auto r4 = validate(7);
        REQUIRE_FALSE(r4.has_value());
        REQUIRE(r4.error() == "must be even");        // third gate fires
    }

    SECTION("subsequent ensures are skipped after a failure")
    {
        int call_count = 0;
        auto count = fxt::ensure([&call_count](int x) { ++call_count; return x > 0; },
                                  std::string{"err"});

        auto result = fxt::expected<int, std::string>{ -1 }
                    | count    // fails → call_count == 1
                    | count;   // skipped

        REQUIRE_FALSE(result.has_value());
        REQUIRE(call_count == 1);
    }

    SECTION("chained with transform on the success path")
    {
        fxt::expected<int, std::string> exp = 8;
        auto result = exp
                    | fxt::ensure([](int x) { return x % 2 == 0; }, std::string{"not even"})
                    | fxt::transform([](int x) { return x * x; });
        REQUIRE(result.has_value());
        REQUIRE(*result == 64);
    }

    SECTION("chained with transform — ensure failure skips transform")
    {
        fxt::expected<int, std::string> exp = 7;
        auto result = exp
                    | fxt::ensure([](int x) { return x % 2 == 0; }, std::string{"not even"})
                    | fxt::transform([](int x) { return x * x; });
        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not even");
    }

    SECTION("chained with and_then")
    {
        fxt::expected<int, std::string> exp = 10;
        auto result = exp
                    | fxt::ensure([](int x) { return x > 0; }, std::string{"not positive"})
                    | fxt::and_then([](int x) -> fxt::expected<std::string, std::string> {
                          return "value is " + std::to_string(x);
                      });
        REQUIRE(result.has_value());
        REQUIRE(*result == "value is 10");
    }

    SECTION("rvalue expected — value is moved through")
    {
        auto make = []() -> fxt::expected<int, std::string> { return 99; };
        auto result = make() | fxt::ensure([](int x) { return x > 0; }, std::string{"err"});
        REQUIRE(result.has_value());
        REQUIRE(*result == 99);
    }
}
