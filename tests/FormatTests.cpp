//
// Created by kenne on 28/06/2026.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <format>
#include <sstream>
#include <string>

// ============================================================================
// std::format — fxt::optional
// ============================================================================

TEST_CASE("std::format for fxt::optional", "[format][optional]")
{
    SECTION("engaged optional formats as the contained value")
    {
        fxt::optional<int> opt = 42;
        REQUIRE(std::format("{}", opt) == "42");
    }

    SECTION("disengaged optional formats as 'nullopt'")
    {
        fxt::optional<int> opt = fxt::nullopt;
        REQUIRE(std::format("{}", opt) == "nullopt");
    }

    SECTION("format spec is forwarded to the value's formatter")
    {
        fxt::optional<double> opt = 3.14159;
        REQUIRE(std::format("{:.2f}", opt) == "3.14");
    }

    SECTION("format spec is ignored for a disengaged optional")
    {
        fxt::optional<double> opt = fxt::nullopt;
        REQUIRE(std::format("{:.2f}", opt) == "nullopt");
    }

    SECTION("optional<string> formats the string value")
    {
        fxt::optional<std::string> opt = "hello";
        REQUIRE(std::format("{}", opt) == "hello");
    }

    SECTION("optional<string> — empty yields 'nullopt'")
    {
        fxt::optional<std::string> opt;
        REQUIRE(std::format("{}", opt) == "nullopt");
    }

    SECTION("optional used inside a larger format string")
    {
        fxt::optional<int> opt = 7;
        REQUIRE(std::format("value={}", opt) == "value=7");
    }
}

// ============================================================================
// std::format — fxt::expected (non-void value)
// ============================================================================

TEST_CASE("std::format for fxt::expected (non-void)", "[format][expected]")
{
    SECTION("expected with value formats as the contained value")
    {
        fxt::expected<int, std::string> exp = 99;
        REQUIRE(std::format("{}", exp) == "99");
    }

    SECTION("expected with error formats as 'unexpected(<error>)'")
    {
        fxt::expected<int, std::string> exp = fxt::unexpected<std::string>{"oops"};
        REQUIRE(std::format("{}", exp) == "unexpected(oops)");
    }

    SECTION("format spec is forwarded to the value formatter")
    {
        fxt::expected<double, std::string> exp = 2.71828;
        REQUIRE(std::format("{:.2f}", exp) == "2.72");
    }

    SECTION("format spec is ignored on the error path")
    {
        fxt::expected<double, std::string> exp = fxt::unexpected<std::string>{"nan"};
        REQUIRE(std::format("{:.2f}", exp) == "unexpected(nan)");
    }

    SECTION("expected with integer error code")
    {
        fxt::expected<std::string, int> exp = fxt::unexpected<int>{404};
        REQUIRE(std::format("{}", exp) == "unexpected(404)");
    }

    SECTION("expected used inside a larger format string")
    {
        fxt::expected<int, std::string> exp = 5;
        REQUIRE(std::format("result={}", exp) == "result=5");
    }
}

// ============================================================================
// std::format — fxt::expected<void, E>
// ============================================================================

TEST_CASE("std::format for fxt::expected<void, E>", "[format][expected][void]")
{
    SECTION("void expected with value formats as 'void'")
    {
        fxt::expected<void, std::string> exp;
        REQUIRE(std::format("{}", exp) == "void");
    }

    SECTION("void expected with error formats as 'unexpected(<error>)'")
    {
        fxt::expected<void, std::string> exp = fxt::unexpected<std::string>{"failed"};
        REQUIRE(std::format("{}", exp) == "unexpected(failed)");
    }
}

// ============================================================================
// operator<< — fxt::optional
// ============================================================================

TEST_CASE("operator<< for fxt::optional", "[format][ostream][optional]")
{
    using fxt::operator<<;

    SECTION("engaged optional streams the contained value")
    {
        fxt::optional<int> opt = 42;
        std::ostringstream oss;
        oss << opt;
        REQUIRE(oss.str() == "42");
    }

    SECTION("disengaged optional streams 'nullopt'")
    {
        fxt::optional<int> opt;
        std::ostringstream oss;
        oss << opt;
        REQUIRE(oss.str() == "nullopt");
    }

    SECTION("optional<string> streams the string value")
    {
        fxt::optional<std::string> opt = "hello";
        std::ostringstream oss;
        oss << opt;
        REQUIRE(oss.str() == "hello");
    }

    SECTION("can be chained with other stream insertions")
    {
        fxt::optional<int> opt = 7;
        std::ostringstream oss;
        oss << "v=" << opt << "!";
        REQUIRE(oss.str() == "v=7!");
    }
}

// ============================================================================
// operator<< — fxt::expected (non-void)
// ============================================================================

TEST_CASE("operator<< for fxt::expected (non-void)", "[format][ostream][expected]")
{
    using fxt::operator<<;

    SECTION("expected with value streams the contained value")
    {
        fxt::expected<int, std::string> exp = 99;
        std::ostringstream oss;
        oss << exp;
        REQUIRE(oss.str() == "99");
    }

    SECTION("expected with error streams 'unexpected(<error>)'")
    {
        fxt::expected<int, std::string> exp = fxt::unexpected<std::string>{"oops"};
        std::ostringstream oss;
        oss << exp;
        REQUIRE(oss.str() == "unexpected(oops)");
    }

    SECTION("can be chained with other stream insertions")
    {
        fxt::expected<int, std::string> exp = 3;
        std::ostringstream oss;
        oss << "r=" << exp;
        REQUIRE(oss.str() == "r=3");
    }
}

// Note: operator<< for expected<void, E> is not provided because the
// expected_like concept (which probes transform with a 1-arg lambda) does not
// match std::expected<void, E> — its transform requires a 0-arg callable.
// Use std::format("{}", exp) for void expected output instead.
