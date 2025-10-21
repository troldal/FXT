//
// Created by kenne on 21/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::when - Direct usage", "[when][variant]")
{
    SECTION("Execute side effect when variant holds matching type")
    {
        fxt::variant<int, double, std::string> v{42};
        int captured = 0;

        fxt::when<int>(v, [&](int x) { captured = x; });

        REQUIRE(captured == 42);
    }

    SECTION("Skip side effect when variant holds different type")
    {
        fxt::variant<int, double, std::string> v{42};
        bool executed = false;

        fxt::when<double>(v, [&](double) { executed = true; });

        REQUIRE_FALSE(executed);
    }

    SECTION("Execute side effect for double type")
    {
        fxt::variant<int, double, std::string> v{3.14};
        double captured = 0.0;

        fxt::when<double>(v, [&](double d) { captured = d; });

        REQUIRE(captured == 3.14);
    }

    SECTION("Execute side effect for string type")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};
        std::string captured;

        fxt::when<std::string>(v, [&](const std::string& s) { captured = s; });

        REQUIRE(captured == "hello");
    }

    SECTION("Variant is returned unchanged")
    {
        fxt::variant<int, double, std::string> v{42};

        auto& result = fxt::when<int>(v, [](int) {});

        REQUIRE(std::get<int>(result) == 42);
    }

    SECTION("Multiple when calls can be chained")
    {
        fxt::variant<int, double, std::string> v{42};
        int count = 0;

        fxt::when<int>(v, [&](int) { count++; });
        fxt::when<int>(v, [&](int) { count++; });
        fxt::when<double>(v, [&](double) { count++; });

        REQUIRE(count == 2);
    }
}

TEST_CASE("fxt::when - Pipeline usage", "[when][variant][pipe]")
{
    SECTION("Execute side effect using pipe operator")
    {
        fxt::variant<int, double, std::string> v{42};
        int captured = 0;

        v | fxt::when<int>([&](int x) { captured = x; });

        REQUIRE(captured == 42);
    }

    SECTION("Chain multiple when calls with pipe operator")
    {
        fxt::variant<int, double, std::string> v{42};
        int int_count = 0;
        int double_count = 0;

        v | fxt::when<int>([&](int) { int_count++; })
          | fxt::when<double>([&](double) { double_count++; })
          | fxt::when<int>([&](int) { int_count++; });

        REQUIRE(int_count == 2);
        REQUIRE(double_count == 0);
    }

    SECTION("Pipeline returns variant unchanged")
    {
        fxt::variant<int, double, std::string> v{100};

        auto result = v | fxt::when<int>([](int) {})
                        | fxt::when<double>([](double) {});

        REQUIRE(std::get<int>(result) == 100);
    }

    SECTION("Works with const variants")
    {
        const fxt::variant<int, double, std::string> v{42};
        int captured = 0;

        v | fxt::when<int>([&](int x) { captured = x; });

        REQUIRE(captured == 42);
    }
}

TEST_CASE("fxt::mwhen with optional<variant> - Direct usage", "[mwhen][variant][optional]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Execute side effect when optional has value and type matches")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};
        int captured = 0;

        fxt::mwhen<int>(opt, [&](int x) { captured = x; });

        REQUIRE(captured == 42);
    }

    SECTION("Skip side effect when optional has value but type doesn't match")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};
        bool executed = false;

        fxt::mwhen<double>(opt, [&](double) { executed = true; });

        REQUIRE_FALSE(executed);
    }

    SECTION("Skip side effect when optional is empty")
    {
        fxt::optional<MyVariant> opt{};
        bool executed = false;

        fxt::mwhen<int>(opt, [&](int) { executed = true; });

        REQUIRE_FALSE(executed);
    }

    SECTION("Execute side effect for double in optional")
    {
        fxt::optional<MyVariant> opt{MyVariant{3.14}};
        double captured = 0.0;

        fxt::mwhen<double>(opt, [&](double d) { captured = d; });

        REQUIRE(captured == 3.14);
    }

    SECTION("Optional is returned unchanged")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};

        auto& result = fxt::mwhen<int>(opt, [](int) {});

        REQUIRE(result.has_value());
        REQUIRE(std::get<int>(*result) == 42);
    }
}

TEST_CASE("fxt::mwhen with optional<variant> - Pipeline usage", "[mwhen][variant][optional][pipe]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Execute side effect using pipe operator")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};
        int captured = 0;

        opt | fxt::mwhen<int>([&](int x) { captured = x; });

        REQUIRE(captured == 42);
    }

    SECTION("Chain multiple mwhen calls with pipe operator")
    {
        fxt::optional<MyVariant> opt{MyVariant{std::string{"hello"}}};
        int int_count = 0;
        int string_count = 0;

        opt | fxt::mwhen<int>([&](int) { int_count++; })
            | fxt::mwhen<std::string>([&](const std::string&) { string_count++; })
            | fxt::mwhen<double>([&](double) {});

        REQUIRE(int_count == 0);
        REQUIRE(string_count == 1);
    }

    SECTION("Pipeline with empty optional")
    {
        fxt::optional<MyVariant> opt{};
        bool executed = false;

        opt | fxt::mwhen<int>([&](int) { executed = true; })
            | fxt::mwhen<double>([&](double) { executed = true; });

        REQUIRE_FALSE(executed);
    }
}

TEST_CASE("fxt::mwhen with expected<variant> - Direct usage", "[mwhen][variant][expected]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Execute side effect when expected has value and type matches")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};
        int captured = 0;

        fxt::mwhen<int>(exp, [&](int x) { captured = x; });

        REQUIRE(captured == 42);
    }

    SECTION("Skip side effect when expected has value but type doesn't match")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};
        bool executed = false;

        fxt::mwhen<double>(exp, [&](double) { executed = true; });

        REQUIRE_FALSE(executed);
    }

    SECTION("Skip side effect when expected has error")
    {
        fxt::expected<MyVariant, std::string> exp{fxt::unexpected("error")};
        bool executed = false;

        fxt::mwhen<int>(exp, [&](int) { executed = true; });

        REQUIRE_FALSE(executed);
    }

    SECTION("Expected is returned unchanged")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};

        auto& result = fxt::mwhen<int>(exp, [](int) {});

        REQUIRE(result.has_value());
        REQUIRE(std::get<int>(*result) == 42);
    }
}

TEST_CASE("fxt::mwhen with expected<variant> - Pipeline usage", "[mwhen][variant][expected][pipe]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Execute side effect using pipe operator")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{3.14}};
        double captured = 0.0;

        exp | fxt::mwhen<double>([&](double d) { captured = d; });

        REQUIRE(captured == 3.14);
    }

    SECTION("Chain multiple mwhen calls with pipe operator")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};
        int count = 0;

        exp | fxt::mwhen<int>([&](int) { count++; })
            | fxt::mwhen<double>([&](double) { count++; })
            | fxt::mwhen<int>([&](int) { count++; });

        REQUIRE(count == 2);
    }

    SECTION("Pipeline with expected error")
    {
        fxt::expected<MyVariant, std::string> exp{fxt::unexpected("error")};
        bool executed = false;

        exp | fxt::mwhen<int>([&](int) { executed = true; })
            | fxt::mwhen<double>([&](double) { executed = true; });

        REQUIRE_FALSE(executed);
        REQUIRE(exp.error() == "error");
    }
}

TEST_CASE("fxt::when - Complex scenarios", "[when][variant]")
{
    SECTION("Multiple side effects on same type")
    {
        fxt::variant<int, double, std::string> v{42};
        int sum = 0;

        v | fxt::when<int>([&](int x) { sum += x; })
          | fxt::when<int>([&](int x) { sum += x * 2; })
          | fxt::when<int>([&](int x) { sum += x * 3; });

        REQUIRE(sum == 42 + 84 + 126); // 42 + 42*2 + 42*3 = 252
    }

    SECTION("Combining when with other operations")
    {
        fxt::variant<int, double, std::string> v{42};
        int logged_value = 0;

        auto result = v | fxt::when<int>([&](int x) { logged_value = x; })
                        | fxt::visit([](auto&& val) -> std::string {
                            using T = std::decay_t<decltype(val)>;
                            if constexpr (std::is_same_v<T, std::string>) {
                                return val;
                            } else {
                                return std::to_string(val);
                            }
                        });

        REQUIRE(logged_value == 42);
        REQUIRE(result == "42");
    }
}

