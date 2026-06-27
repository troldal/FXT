#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::holds_alternative with fxt::variant", "[holds_alternative][variant]")
{
    SECTION("Returns true for the currently held alternative")
    {
        fxt::variant<int, double, std::string> v{42};
        REQUIRE(fxt::holds_alternative<int>(v));
    }

    SECTION("Returns false for alternatives not currently held")
    {
        fxt::variant<int, double, std::string> v{42};
        REQUIRE_FALSE(fxt::holds_alternative<double>(v));
        REQUIRE_FALSE(fxt::holds_alternative<std::string>(v));
    }

    SECTION("Correctly identifies each alternative")
    {
        fxt::variant<int, double, std::string> v_int{42};
        fxt::variant<int, double, std::string> v_double{3.14};
        fxt::variant<int, double, std::string> v_string{std::string{"hello"}};

        REQUIRE(fxt::holds_alternative<int>(v_int));
        REQUIRE(fxt::holds_alternative<double>(v_double));
        REQUIRE(fxt::holds_alternative<std::string>(v_string));
    }

    SECTION("Reflects reassignment")
    {
        fxt::variant<int, double, std::string> v{42};
        REQUIRE(fxt::holds_alternative<int>(v));

        v = 3.14;
        REQUIRE_FALSE(fxt::holds_alternative<int>(v));
        REQUIRE(fxt::holds_alternative<double>(v));

        v = std::string{"test"};
        REQUIRE_FALSE(fxt::holds_alternative<double>(v));
        REQUIRE(fxt::holds_alternative<std::string>(v));
    }

    SECTION("Works with const variant")
    {
        const fxt::variant<int, double, std::string> v{3.14};
        REQUIRE(fxt::holds_alternative<double>(v));
        REQUIRE_FALSE(fxt::holds_alternative<int>(v));
    }
}

TEST_CASE("fxt::holds_alternative with pipe operator", "[holds_alternative][variant][pipe]")
{
    SECTION("Returns true for the currently held alternative via pipe")
    {
        fxt::variant<int, double, std::string> v{42};
        // Extra parens prevent Catch2's expression decomposition from intercepting |
        REQUIRE((v | fxt::holds_alternative<int>()));
    }

    SECTION("Returns false for alternatives not held via pipe")
    {
        fxt::variant<int, double, std::string> v{42};
        REQUIRE_FALSE((v | fxt::holds_alternative<double>()));
        REQUIRE_FALSE((v | fxt::holds_alternative<std::string>()));
    }

    SECTION("Pipe form works for all alternatives")
    {
        fxt::variant<int, double, std::string> v_int{42};
        fxt::variant<int, double, std::string> v_double{3.14};
        fxt::variant<int, double, std::string> v_string{std::string{"hello"}};

        REQUIRE((v_int    | fxt::holds_alternative<int>()));
        REQUIRE((v_double | fxt::holds_alternative<double>()));
        REQUIRE((v_string | fxt::holds_alternative<std::string>()));
    }

    SECTION("Pipe form works with const variant")
    {
        const fxt::variant<int, double, std::string> v{std::string{"hi"}};
        REQUIRE((v | fxt::holds_alternative<std::string>()));
        REQUIRE_FALSE((v | fxt::holds_alternative<int>()));
    }

    SECTION("Pipe form returns bool")
    {
        fxt::variant<int, double> v{42};
        auto result = v | fxt::holds_alternative<int>();
        STATIC_REQUIRE(std::is_same_v<decltype(result), bool>);
    }
}

TEST_CASE("fxt::mholds_alternative with fxt::optional<fxt::variant>", "[mholds_alternative][optional][variant]")
{
    SECTION("Returns true when optional has value and variant holds the alternative")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};
        REQUIRE(fxt::mholds_alternative<int>(opt));
    }

    SECTION("Returns false when variant holds a different alternative")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};
        REQUIRE_FALSE(fxt::mholds_alternative<double>(opt));
        REQUIRE_FALSE(fxt::mholds_alternative<std::string>(opt));
    }

    SECTION("Returns false when optional is empty")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{};
        REQUIRE_FALSE(fxt::mholds_alternative<int>(opt));
        REQUIRE_FALSE(fxt::mholds_alternative<double>(opt));
    }

    SECTION("Works for each alternative")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt_int{fxt::variant<int, double, std::string>{42}};
        fxt::optional<fxt::variant<int, double, std::string>> opt_double{fxt::variant<int, double, std::string>{3.14}};
        fxt::optional<fxt::variant<int, double, std::string>> opt_string{fxt::variant<int, double, std::string>{std::string{"hello"}}};

        REQUIRE(fxt::mholds_alternative<int>(opt_int));
        REQUIRE(fxt::mholds_alternative<double>(opt_double));
        REQUIRE(fxt::mholds_alternative<std::string>(opt_string));
    }
}

TEST_CASE("fxt::mholds_alternative with fxt::optional and pipe operator", "[mholds_alternative][optional][variant][pipe]")
{
    SECTION("Returns true via pipe when optional holds the alternative")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
        REQUIRE((opt | fxt::mholds_alternative<double>()));
    }

    SECTION("Returns false via pipe for a different alternative")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{3.14}};
        REQUIRE_FALSE((opt | fxt::mholds_alternative<int>()));
    }

    SECTION("Returns false via pipe when optional is empty")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{};
        REQUIRE_FALSE((opt | fxt::mholds_alternative<int>()));
    }

    SECTION("Pipe form returns bool")
    {
        fxt::optional<fxt::variant<int, double>> opt{fxt::variant<int, double>{42}};
        auto result = opt | fxt::mholds_alternative<int>();
        STATIC_REQUIRE(std::is_same_v<decltype(result), bool>);
    }

    SECTION("Can be used after mwhen in a pipeline")
    {
        fxt::optional<fxt::variant<int, double, std::string>> opt{fxt::variant<int, double, std::string>{42}};
        bool seen = false;

        bool holds = opt
                   | fxt::mwhen<int>([&](int) { seen = true; })
                   | fxt::mholds_alternative<int>();

        REQUIRE(holds);
        REQUIRE(seen);
    }
}

TEST_CASE("fxt::mholds_alternative with fxt::expected<fxt::variant, E>", "[mholds_alternative][expected][variant]")
{
    SECTION("Returns true when expected has value and variant holds the alternative")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{42}};
        REQUIRE(fxt::mholds_alternative<int>(exp));
    }

    SECTION("Returns false when variant holds a different alternative")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{42}};
        REQUIRE_FALSE(fxt::mholds_alternative<double>(exp));
    }

    SECTION("Returns false when expected holds an error")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("error")};
        REQUIRE_FALSE(fxt::mholds_alternative<int>(exp));
        REQUIRE_FALSE(fxt::mholds_alternative<double>(exp));
    }

    SECTION("Works for each alternative")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_int{fxt::variant<int, double, std::string>{42}};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_double{fxt::variant<int, double, std::string>{3.14}};
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp_string{fxt::variant<int, double, std::string>{std::string{"hello"}}};

        REQUIRE(fxt::mholds_alternative<int>(exp_int));
        REQUIRE(fxt::mholds_alternative<double>(exp_double));
        REQUIRE(fxt::mholds_alternative<std::string>(exp_string));
    }
}

TEST_CASE("fxt::mholds_alternative with fxt::expected and pipe operator", "[mholds_alternative][expected][variant][pipe]")
{
    SECTION("Returns true via pipe when expected holds the alternative")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{std::string{"hi"}}};
        REQUIRE((exp | fxt::mholds_alternative<std::string>()));
    }

    SECTION("Returns false via pipe for a different alternative")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::variant<int, double, std::string>{std::string{"hi"}}};
        REQUIRE_FALSE((exp | fxt::mholds_alternative<int>()));
    }

    SECTION("Returns false via pipe when expected holds an error")
    {
        fxt::expected<fxt::variant<int, double, std::string>, std::string> exp{fxt::unexpected("oops")};
        REQUIRE_FALSE((exp | fxt::mholds_alternative<int>()));
    }

    SECTION("Pipe form returns bool")
    {
        fxt::expected<fxt::variant<int, double>, std::string> exp{fxt::variant<int, double>{3.14}};
        auto result = exp | fxt::mholds_alternative<double>();
        STATIC_REQUIRE(std::is_same_v<decltype(result), bool>);
    }
}
