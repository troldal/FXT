//
// Created by kenne on 21/10/2025.
//

#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::transform_when - Direct usage", "[transform_when][variant]")
{
    SECTION("Transform value when variant holds matching type")
    {
        fxt::variant<int, double, std::string> v{42};

        fxt::transform_when<int>(v, [](int x) { return x * 2; });

        REQUIRE(std::get<int>(v) == 84);
    }

    SECTION("Skip transformation when variant holds different type")
    {
        fxt::variant<int, double, std::string> v{42};

        fxt::transform_when<double>(v, [](double d) { return d * 3.0; });

        REQUIRE(std::get<int>(v) == 42); // Unchanged
    }

    SECTION("Transform double value")
    {
        fxt::variant<int, double, std::string> v{3.14};

        fxt::transform_when<double>(v, [](double d) { return d * 2.0; });

        REQUIRE(std::get<double>(v) == 3.14 * 2.0);
    }

    SECTION("Transform string value")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};

        fxt::transform_when<std::string>(v, [](std::string s) { return s + " world"; });

        REQUIRE(std::get<std::string>(v) == "hello world");
    }

    SECTION("Multiple transformations on same type")
    {
        fxt::variant<int, double, std::string> v{10};

        fxt::transform_when<int>(v, [](int x) { return x * 2; });
        fxt::transform_when<int>(v, [](int x) { return x + 5; });

        REQUIRE(std::get<int>(v) == 25); // (10 * 2) + 5
    }

    SECTION("Variant is returned for chaining")
    {
        fxt::variant<int, double, std::string> v{42};

        auto& result = fxt::transform_when<int>(v, [](int x) { return x * 2; });

        REQUIRE(&result == &v);
        REQUIRE(std::get<int>(result) == 84);
    }
}

TEST_CASE("fxt::transform_when - Pipeline usage", "[transform_when][variant][pipe]")
{
    SECTION("Transform value using pipe operator")
    {
        fxt::variant<int, double, std::string> v{42};

        v | fxt::transform_when<int>([](int x) { return x * 2; });

        REQUIRE(std::get<int>(v) == 84);
    }

    SECTION("Chain multiple transform_when calls")
    {
        fxt::variant<int, double, std::string> v{10};

        v | fxt::transform_when<int>([](int x) { return x * 2; })
          | fxt::transform_when<double>([](double d) { return d * 3.0; })
          | fxt::transform_when<int>([](int x) { return x + 5; });

        REQUIRE(std::get<int>(v) == 25); // (10 * 2) + 5, double skipped
    }

    SECTION("Transform different types in pipeline")
    {
        fxt::variant<int, double, std::string> v{3.14};

        v | fxt::transform_when<int>([](int x) { return x * 10; })
          | fxt::transform_when<double>([](double d) { return d * 2.0; })
          | fxt::transform_when<std::string>([](std::string s) { return s + "!"; });

        REQUIRE(std::get<double>(v) == 3.14 * 2.0);
    }

    SECTION("Complex transformation pipeline")
    {
        fxt::variant<int, double, std::string> v{100};

        v | fxt::transform_when<int>([](int x) { return x + 50; })
          | fxt::transform_when<int>([](int x) { return x * 2; })
          | fxt::transform_when<int>([](int x) { return x - 10; });

        REQUIRE(std::get<int>(v) == 290); // ((100 + 50) * 2) - 10
    }
}

TEST_CASE("fxt::transform_when - Mixed with when", "[transform_when][when][variant][pipe]")
{
    SECTION("Combine transform_when and when for logging")
    {
        fxt::variant<int, double, std::string> v{50};
        int logged_value = 0;

        v | fxt::transform_when<int>([](int x) { return x * 2; })
          | fxt::when<int>([&](int x) { logged_value = x; })
          | fxt::transform_when<int>([](int x) { return x + 10; });

        REQUIRE(std::get<int>(v) == 110); // (50 * 2) + 10
        REQUIRE(logged_value == 100);      // Logged after first transform
    }
}

TEST_CASE("fxt::mtransform_when with optional<variant> - Direct usage", "[mtransform_when][variant][optional]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Transform value when optional has value and type matches")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};

        fxt::mtransform_when<int>(opt, [](int x) { return x * 2; });

        REQUIRE(opt.has_value());
        REQUIRE(std::get<int>(*opt) == 84);
    }

    SECTION("Skip transformation when optional has value but type doesn't match")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};

        fxt::mtransform_when<double>(opt, [](double d) { return d * 3.0; });

        REQUIRE(std::get<int>(*opt) == 42); // Unchanged
    }

    SECTION("Skip transformation when optional is empty")
    {
        fxt::optional<MyVariant> opt{};

        fxt::mtransform_when<int>(opt, [](int x) { return x * 2; });

        REQUIRE_FALSE(opt.has_value());
    }

    SECTION("Transform double in optional")
    {
        fxt::optional<MyVariant> opt{MyVariant{3.14}};

        fxt::mtransform_when<double>(opt, [](double d) { return d * 2.0; });

        REQUIRE(std::get<double>(*opt) == 3.14 * 2.0);
    }

    SECTION("Transform string in optional")
    {
        fxt::optional<MyVariant> opt{MyVariant{std::string{"hello"}}};

        fxt::mtransform_when<std::string>(opt, [](std::string s) { return s + " world"; });

        REQUIRE(std::get<std::string>(*opt) == "hello world");
    }

    SECTION("Multiple transformations on same type in optional")
    {
        fxt::optional<MyVariant> opt{MyVariant{10}};

        fxt::mtransform_when<int>(opt, [](int x) { return x * 2; });
        fxt::mtransform_when<int>(opt, [](int x) { return x + 5; });

        REQUIRE(std::get<int>(*opt) == 25); // (10 * 2) + 5
    }
}

TEST_CASE("fxt::mtransform_when with optional<variant> - Pipeline usage", "[mtransform_when][variant][optional][pipe]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Transform value using pipe operator")
    {
        fxt::optional<MyVariant> opt{MyVariant{42}};

        opt | fxt::mtransform_when<int>([](int x) { return x * 2; });

        REQUIRE(std::get<int>(*opt) == 84);
    }

    SECTION("Chain multiple mtransform_when calls")
    {
        fxt::optional<MyVariant> opt{MyVariant{100}};

        opt | fxt::mtransform_when<int>([](int x) { return x + 25; })
            | fxt::mtransform_when<double>([](double d) { return d * 2.0; })
            | fxt::mtransform_when<int>([](int x) { return x * 4; });

        REQUIRE(std::get<int>(*opt) == 500); // (100 + 25) * 4
    }

    SECTION("Pipeline with empty optional")
    {
        fxt::optional<MyVariant> opt{};

        opt | fxt::mtransform_when<int>([](int x) { return x * 2; })
            | fxt::mtransform_when<double>([](double d) { return d * 3.0; });

        REQUIRE_FALSE(opt.has_value());
    }

    SECTION("Complex transformation pipeline")
    {
        fxt::optional<MyVariant> opt{MyVariant{50}};

        opt | fxt::mtransform_when<int>([](int x) { return x * 2; })
            | fxt::mtransform_when<int>([](int x) { return x + 100; })
            | fxt::mtransform_when<int>([](int x) { return x / 2; });

        REQUIRE(std::get<int>(*opt) == 100); // ((50 * 2) + 100) / 2
    }
}

TEST_CASE("fxt::mtransform_when with expected<variant> - Direct usage", "[mtransform_when][variant][expected]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Transform value when expected has value and type matches")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};

        fxt::mtransform_when<int>(exp, [](int x) { return x * 2; });

        REQUIRE(exp.has_value());
        REQUIRE(std::get<int>(*exp) == 84);
    }

    SECTION("Skip transformation when expected has value but type doesn't match")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};

        fxt::mtransform_when<double>(exp, [](double d) { return d * 3.0; });

        REQUIRE(std::get<int>(*exp) == 42); // Unchanged
    }

    SECTION("Skip transformation when expected has error")
    {
        fxt::expected<MyVariant, std::string> exp{fxt::unexpected("error")};

        fxt::mtransform_when<int>(exp, [](int x) { return x * 2; });

        REQUIRE_FALSE(exp.has_value());
        REQUIRE(exp.error() == "error");
    }

    SECTION("Transform double in expected")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{3.14}};

        fxt::mtransform_when<double>(exp, [](double d) { return d * 2.0; });

        REQUIRE(std::get<double>(*exp) == 3.14 * 2.0);
    }

    SECTION("Multiple transformations on same type in expected")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{10}};

        fxt::mtransform_when<int>(exp, [](int x) { return x * 2; });
        fxt::mtransform_when<int>(exp, [](int x) { return x + 5; });

        REQUIRE(std::get<int>(*exp) == 25); // (10 * 2) + 5
    }
}

TEST_CASE("fxt::mtransform_when with expected<variant> - Pipeline usage", "[mtransform_when][variant][expected][pipe]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Transform value using pipe operator")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{42}};

        exp | fxt::mtransform_when<int>([](int x) { return x * 2; });

        REQUIRE(std::get<int>(*exp) == 84);
    }

    SECTION("Chain multiple mtransform_when calls")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{50}};

        exp | fxt::mtransform_when<int>([](int x) { return x * 2; })
            | fxt::mtransform_when<double>([](double d) { return d / 2.0; })
            | fxt::mtransform_when<int>([](int x) { return x + 10; });

        REQUIRE(std::get<int>(*exp) == 110); // (50 * 2) + 10
    }

    SECTION("Pipeline with expected error")
    {
        fxt::expected<MyVariant, std::string> exp{fxt::unexpected("error")};

        exp | fxt::mtransform_when<int>([](int x) { return x * 2; })
            | fxt::mtransform_when<double>([](double d) { return d * 3.0; });

        REQUIRE_FALSE(exp.has_value());
        REQUIRE(exp.error() == "error");
    }
}

TEST_CASE("fxt::mtransform_when - Mixed with mwhen", "[mtransform_when][mwhen][variant][pipe]")
{
    using MyVariant = fxt::variant<int, double, std::string>;

    SECTION("Combine mtransform_when and mwhen for logging")
    {
        fxt::optional<MyVariant> opt{MyVariant{50}};
        int logged_value = 0;

        opt | fxt::mtransform_when<int>([](int x) { return x * 2; })
            | fxt::mwhen<int>([&](int x) { logged_value = x; })
            | fxt::mtransform_when<int>([](int x) { return x + 10; });

        REQUIRE(std::get<int>(*opt) == 110);
        REQUIRE(logged_value == 100);
    }

    SECTION("Complex pipeline with both operations")
    {
        fxt::expected<MyVariant, std::string> exp{MyVariant{10}};
        int sum = 0;

        exp | fxt::mwhen<int>([&](int x) { sum += x; })
            | fxt::mtransform_when<int>([](int x) { return x * 2; })
            | fxt::mwhen<int>([&](int x) { sum += x; })
            | fxt::mtransform_when<int>([](int x) { return x + 5; })
            | fxt::mwhen<int>([&](int x) { sum += x; });

        REQUIRE(std::get<int>(*exp) == 25); // ((10 * 2) + 5)
        REQUIRE(sum == 10 + 20 + 25);        // Logged at each step
    }
}

TEST_CASE("fxt::transform_when - String transformations", "[transform_when][variant]")
{
    SECTION("String concatenation")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};

        v | fxt::transform_when<std::string>([](std::string s) { return s + " world"; });

        REQUIRE(std::get<std::string>(v) == "hello world");
    }

    SECTION("String uppercase conversion")
    {
        fxt::variant<int, double, std::string> v{std::string{"hello"}};

        v | fxt::transform_when<std::string>([](std::string s) {
            for (auto& c : s) c = std::toupper(c);
            return s;
        });

        REQUIRE(std::get<std::string>(v) == "HELLO");
    }

    SECTION("Multiple string transformations")
    {
        fxt::variant<int, double, std::string> v{std::string{"test"}};

        v | fxt::transform_when<std::string>([](std::string s) {
                for (auto& c : s) c = std::toupper(c);
                return s;
            })
          | fxt::transform_when<std::string>([](std::string s) {
                return s + "!!!";
            });

        REQUIRE(std::get<std::string>(v) == "TEST!!!");
    }
}

TEST_CASE("fxt::transform_when - Mathematical operations", "[transform_when][variant]")
{
    SECTION("Integer arithmetic operations")
    {
        fxt::variant<int, double, std::string> v{10};

        v | fxt::transform_when<int>([](int x) { return x + 5; })
          | fxt::transform_when<int>([](int x) { return x * 3; })
          | fxt::transform_when<int>([](int x) { return x - 2; });

        REQUIRE(std::get<int>(v) == 43); // ((10 + 5) * 3) - 2
    }

    SECTION("Double arithmetic operations")
    {
        fxt::variant<int, double, std::string> v{2.0};

        v | fxt::transform_when<double>([](double d) { return d * d; })
          | fxt::transform_when<double>([](double d) { return d + 1.0; });

        REQUIRE(std::get<double>(v) == 5.0); // (2.0 * 2.0) + 1.0
    }
}

