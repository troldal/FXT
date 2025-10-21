//
// Created by kenne on 21/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::index with fxt::variant", "[index][variant]")
{
    SECTION("Get index of first alternative (int)")
    {
        fxt::variant<int, double, std::string> v{42};
        auto idx = fxt::index(v);

        REQUIRE(idx == 0);
    }

    SECTION("Get index of second alternative (double)")
    {
        fxt::variant<int, double, std::string> v{3.14};
        auto idx = fxt::index(v);

        REQUIRE(idx == 1);
    }

    SECTION("Get index of third alternative (string)")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};
        auto idx = fxt::index(v);

        REQUIRE(idx == 2);
    }

    SECTION("Index changes when variant is reassigned")
    {
        fxt::variant<int, double, std::string> v{42};
        REQUIRE(fxt::index(v) == 0);

        v = 3.14;
        REQUIRE(fxt::index(v) == 1);

        v = std::string{"test"};
        REQUIRE(fxt::index(v) == 2);
    }
}

TEST_CASE("fxt::index with pipe operator", "[index][variant][pipe]")
{
    SECTION("Get index using pipe operator - first alternative")
    {
        fxt::variant<int, double, std::string> v{42};
        auto idx = v | fxt::index();

        REQUIRE(idx == 0);
    }

    SECTION("Get index using pipe operator - second alternative")
    {
        fxt::variant<int, double, std::string> v{3.14};
        auto idx = v | fxt::index();

        REQUIRE(idx == 1);
    }

    SECTION("Get index using pipe operator - third alternative")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};
        auto idx = v | fxt::index();

        REQUIRE(idx == 2);
    }

    SECTION("Pipe operator works with const variants")
    {
        const fxt::variant<int, double, std::string> v{3.14};
        auto idx = v | fxt::index();

        REQUIRE(idx == 1);
    }
}

TEST_CASE("fxt::mindex with fxt::optional<fxt::variant>", "[mindex][optional][variant]")
{
    SECTION("Get index from optional containing variant - direct call")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};
        auto result = fxt::mindex(opt);

        REQUIRE(result.has_value());
        REQUIRE(*result == 0);
    }

    SECTION("Get index from optional with different alternatives")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt1{fxt::variant<int, double, std::string>{42}};
        fxt::optional<fxt::variant<int, double, std::string>> opt2{fxt::variant<int, double, std::string>{3.14}};
        fxt::optional<fxt::variant<int, double, std::string>> opt3{fxt::variant<int, double, std::string>{std::string{"test"}}};

        auto idx1 = fxt::mindex(opt1);
        auto idx2 = fxt::mindex(opt2);
        auto idx3 = fxt::mindex(opt3);

        REQUIRE(idx1.has_value());
        REQUIRE(*idx1 == 0);

        REQUIRE(idx2.has_value());
        REQUIRE(*idx2 == 1);

        REQUIRE(idx3.has_value());
        REQUIRE(*idx3 == 2);
    }

    SECTION("Empty optional returns empty optional")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{};
        auto result = fxt::mindex(opt);

        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("fxt::mindex with fxt::optional and pipe operator", "[mindex][optional][variant][pipe]")
{
    SECTION("Get index using pipe operator")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
        auto result = opt | fxt::mindex();

        REQUIRE(result.has_value());
        REQUIRE(*result == 1);
    }

    SECTION("Empty optional with pipe operator returns empty")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{};
        auto result = opt | fxt::mindex();

        REQUIRE_FALSE(result.has_value());
    }

    SECTION("Chain mindex with transform")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{std::string{"test"}}};
        auto result = opt
                    | fxt::mindex()
                    | fxt::transform([](std::size_t idx) { return idx * 100; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 200);  // index 2 * 100
    }
}

TEST_CASE("fxt::mindex with fxt::expected<fxt::variant, E>", "[mindex][expected][variant]")
{
    SECTION("Get index from expected containing variant - direct call")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{42}};
        auto result = fxt::mindex(exp);

        REQUIRE(result.has_value());
        REQUIRE(*result == 0);
    }

    SECTION("Get index from expected with different alternatives")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp1{fxt::variant<int, double, std::string>{42}};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp2{fxt::variant<int, double, std::string>{3.14}};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp3{fxt::variant<int, double, std::string>{std::string{"test"}}};

        auto idx1 = fxt::mindex(exp1);
        auto idx2 = fxt::mindex(exp2);
        auto idx3 = fxt::mindex(exp3);

        REQUIRE(idx1.has_value());
        REQUIRE(*idx1 == 0);

        REQUIRE(idx2.has_value());
        REQUIRE(*idx2 == 1);

        REQUIRE(idx3.has_value());
        REQUIRE(*idx3 == 2);
    }

    SECTION("Expected with error returns expected with error")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("error occurred")};
        auto result = fxt::mindex(exp);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error occurred");
    }
}

TEST_CASE("fxt::mindex with fxt::expected and pipe operator", "[mindex][expected][variant][pipe]")
{
    SECTION("Get index using pipe operator")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{3.14}};
        auto result = exp | fxt::mindex();

        REQUIRE(result.has_value());
        REQUIRE(*result == 1);
    }

    SECTION("Expected with error using pipe operator preserves error")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("error")};
        auto result = exp | fxt::mindex();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error");
    }

    SECTION("Chain mindex with transform")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{std::string{"test"}}};
        auto result = exp
                    | fxt::mindex()
                    | fxt::transform([](std::size_t idx) { return std::to_string(idx); });

        REQUIRE(result.has_value());
        REQUIRE(*result == "2");
    }

    SECTION("Chain mindex with transform - error propagates")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("initial error")};
        auto result = exp
                    | fxt::mindex()
                    | fxt::transform([](std::size_t idx) { return std::to_string(idx); });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "initial error");
    }
}

TEST_CASE("fxt::index with binary variant", "[index][variant]")
{
    SECTION("Binary variant - first alternative")
    {
        fxt::variant<int, double> v{42};
        REQUIRE(fxt::index(v) == 0);
    }

    SECTION("Binary variant - second alternative")
    {
        fxt::variant<int, double> v{3.14};
        REQUIRE(fxt::index(v) == 1);
    }
}

TEST_CASE("fxt::index with single-type variant", "[index][variant]")
{
    SECTION("Single type variant always has index 0")
    {
        fxt::variant<int> v{42};
        REQUIRE(fxt::index(v) == 0);
    }
}

TEST_CASE("fxt::mindex complex chaining scenarios", "[mindex][variant][pipe]")
{
    SECTION("Chain multiple operations on optional<variant>")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};

        auto result = opt
                    | fxt::mindex()
                    | fxt::transform([](std::size_t idx) { return idx + 10; })
                    | fxt::transform([](std::size_t val) { return val * 2; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 20);  // (0 + 10) * 2
    }

    SECTION("Chain multiple operations on expected<variant>")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{std::string{"test"}}};

        auto result = exp
                    | fxt::mindex()
                    | fxt::transform([](std::size_t idx) { return idx == 2; })
                    | fxt::transform([](bool b) { return b ? "correct" : "incorrect"; });

        REQUIRE(result.has_value());
        REQUIRE(std::string{*result} == "correct");
    }
}

TEST_CASE("fxt::index type deduction", "[index][variant]")
{
    SECTION("Index returns size_t")
    {
        fxt::variant<int, double, std::string> v{42};
        auto idx = fxt::index(v);

        STATIC_REQUIRE(std::is_same_v<decltype(idx), std::size_t>);
    }

    SECTION("Pipe operator index returns size_t")
    {
        fxt::variant<int, double, std::string> v{42};
        auto idx = v | fxt::index();

        STATIC_REQUIRE(std::is_same_v<decltype(idx), std::size_t>);
    }
}

TEST_CASE("fxt::mindex type deduction", "[mindex][variant]")
{
    SECTION("mindex with optional returns optional<size_t>")
    {
        fxt::optional<fxt::variant<int, double>> opt{fxt::variant<int, double>{42}};
        auto result = fxt::mindex(opt);

        STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::optional<std::size_t>>);
    }

    SECTION("mindex with expected returns expected<size_t, E>")
    {
        fxt::expected<fxt::variant<int, double>, std::string> exp{fxt::variant<int, double>{42}};
        auto result = fxt::mindex(exp);

        STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::expected<std::size_t, std::string>>);
    }
}

