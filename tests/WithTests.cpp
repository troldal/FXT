#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

TEST_CASE("fxt::with applicative — entry point (Case 1: plain callable | with(ex))", "[with][expected][applicative]")
{
    SECTION("single-argument function, value present")
    {
        auto square = [](double x) { return x * x; };
        auto result = fxt::curry(square)
            | fxt::with(fxt::expected<double, std::string>{5.0});

        REQUIRE(result.has_value());
        REQUIRE(*result == 25.0);
    }

    SECTION("single-argument function, error propagated")
    {
        auto square = [](double x) { return x * x; };
        auto result = fxt::curry(square)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("bad input")});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "bad input");
    }

    SECTION("two-argument function, first step yields partial application")
    {
        auto add = [](double a, double b) { return a + b; };
        auto partial = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{10.0});

        // Intermediate: expected<Func, string> holding a partially-applied add
        REQUIRE(partial.has_value());

        // Completing the pipeline
        auto result = partial | fxt::with(fxt::expected<double, std::string>{5.0});
        REQUIRE(result.has_value());
        REQUIRE(*result == 15.0);
    }
}

TEST_CASE("fxt::with applicative — full pipeline (fxt::curry + multiple with())", "[with][expected][applicative]")
{
    SECTION("two-argument function, all valid")
    {
        auto add = [](double a, double b) { return a + b; };
        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{5.0});

        REQUIRE(result.has_value());
        REQUIRE(*result == 15.0);
    }

    SECTION("three-argument function, all valid")
    {
        auto sum3 = [](double a, double b, double c) { return a + b + c; };
        auto result = fxt::curry(sum3)
            | fxt::with(fxt::expected<double, std::string>{1.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::with(fxt::expected<double, std::string>{3.0});

        REQUIRE(result.has_value());
        REQUIRE(*result == 6.0);
    }

    SECTION("four-argument function, all valid")
    {
        auto sum4 = [](double a, double b, double c, double d) { return a + b + c + d; };
        auto result = fxt::curry(sum4)
            | fxt::with(fxt::expected<double, std::string>{1.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::with(fxt::expected<double, std::string>{3.0})
            | fxt::with(fxt::expected<double, std::string>{4.0});

        REQUIRE(result.has_value());
        REQUIRE(*result == 10.0);
    }

    SECTION("result type correctly deduced — int computation")
    {
        auto multiply = [](int a, int b) { return a * b; };
        auto result = fxt::curry(multiply)
            | fxt::with(fxt::expected<int, std::string>{6})
            | fxt::with(fxt::expected<int, std::string>{7});

        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("result type correctly deduced — string concatenation")
    {
        auto concat = [](const std::string& a, const std::string& b) { return a + b; };
        auto result = fxt::curry(concat)
            | fxt::with(fxt::expected<std::string, std::string>{"Hello, "})
            | fxt::with(fxt::expected<std::string, std::string>{"world!"});

        REQUIRE(result.has_value());
        REQUIRE(*result == "Hello, world!");
    }
}

TEST_CASE("fxt::with applicative — error propagation", "[with][expected][applicative]")
{
    SECTION("error in first argument of two-step chain")
    {
        auto add = [](double a, double b) { return a + b; };
        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("first bad")})
            | fxt::with(fxt::expected<double, std::string>{5.0});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "first bad");
    }

    SECTION("error in second argument of two-step chain")
    {
        auto add = [](double a, double b) { return a + b; };
        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("second bad")});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "second bad");
    }

    SECTION("error in first argument of three-step chain")
    {
        auto sum3 = [](double a, double b, double c) { return a + b + c; };
        auto result = fxt::curry(sum3)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("first error")})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::with(fxt::expected<double, std::string>{3.0});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "first error");
    }

    SECTION("error in middle argument of three-step chain")
    {
        auto sum3 = [](double a, double b, double c) { return a + b + c; };
        auto result = fxt::curry(sum3)
            | fxt::with(fxt::expected<double, std::string>{1.0})
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("middle error")})
            | fxt::with(fxt::expected<double, std::string>{3.0});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "middle error");
    }

    SECTION("error in last argument of three-step chain")
    {
        auto sum3 = [](double a, double b, double c) { return a + b + c; };
        auto result = fxt::curry(sum3)
            | fxt::with(fxt::expected<double, std::string>{1.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("last error")});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "last error");
    }

    SECTION("first error wins when multiple arguments are invalid")
    {
        auto sum3 = [](double a, double b, double c) { return a + b + c; };
        auto result = fxt::curry(sum3)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("first error")})
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("second error")})
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("third error")});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "first error");
    }
}

TEST_CASE("fxt::with applicative — mixed value types", "[with][expected][applicative]")
{
    SECTION("int and string arguments")
    {
        auto format = [](int n, const std::string& label) {
            return label + ": " + std::to_string(n);
        };
        auto result = fxt::curry(format)
            | fxt::with(fxt::expected<int, std::string>{42})
            | fxt::with(fxt::expected<std::string, std::string>{"answer"});

        REQUIRE(result.has_value());
        REQUIRE(*result == "answer: 42");
    }

    SECTION("int and string with error on string side")
    {
        auto format = [](int n, const std::string& label) {
            return label + ": " + std::to_string(n);
        };
        auto result = fxt::curry(format)
            | fxt::with(fxt::expected<int, std::string>{42})
            | fxt::with(fxt::expected<std::string, std::string>{fxt::unexpected<std::string>("missing label")});

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "missing label");
    }
}

TEST_CASE("fxt::with applicative — xlthermo-style validation pipeline", "[with][expected][applicative]")
{
    // Simulates the primary intended use case: validating Excel function arguments
    // before passing them to a pure computation function.

    auto validate_range = [](double v, double lo, double hi, const std::string& name)
        -> fxt::expected<double, std::string>
    {
        if (v < lo || v > hi)
            return fxt::unexpected<std::string>(name + " out of range");
        return v;
    };

    auto compute_enthalpy = [](double pressure, double temperature, double quality) {
        return pressure * 0.1 + temperature * 2.0 + quality * 500.0;
    };

    SECTION("all arguments valid")
    {
        auto result = fxt::curry(compute_enthalpy)
            | fxt::with(validate_range(1.5, 0.1, 10.0, "pressure"))
            | fxt::with(validate_range(300.0, 273.0, 647.0, "temperature"))
            | fxt::with(validate_range(0.8, 0.0, 1.0, "quality"));

        REQUIRE(result.has_value());
        REQUIRE(*result == Catch::Approx(1.5 * 0.1 + 300.0 * 2.0 + 0.8 * 500.0));
    }

    SECTION("pressure out of range")
    {
        auto result = fxt::curry(compute_enthalpy)
            | fxt::with(validate_range(-1.0, 0.1, 10.0, "pressure"))
            | fxt::with(validate_range(300.0, 273.0, 647.0, "temperature"))
            | fxt::with(validate_range(0.8, 0.0, 1.0, "quality"));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "pressure out of range");
    }

    SECTION("temperature out of range")
    {
        auto result = fxt::curry(compute_enthalpy)
            | fxt::with(validate_range(1.5, 0.1, 10.0, "pressure"))
            | fxt::with(validate_range(1000.0, 273.0, 647.0, "temperature"))
            | fxt::with(validate_range(0.8, 0.0, 1.0, "quality"));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "temperature out of range");
    }

    SECTION("quality out of range")
    {
        auto result = fxt::curry(compute_enthalpy)
            | fxt::with(validate_range(1.5, 0.1, 10.0, "pressure"))
            | fxt::with(validate_range(300.0, 273.0, 647.0, "temperature"))
            | fxt::with(validate_range(1.5, 0.0, 1.0, "quality"));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "quality out of range");
    }

    SECTION("first invalid argument error is reported, subsequent arguments not evaluated")
    {
        // Both pressure and quality are invalid; pressure error must be reported.
        auto result = fxt::curry(compute_enthalpy)
            | fxt::with(validate_range(-1.0, 0.1, 10.0, "pressure"))
            | fxt::with(validate_range(300.0, 273.0, 647.0, "temperature"))
            | fxt::with(validate_range(1.5, 0.0, 1.0, "quality"));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "pressure out of range");
    }
}

TEST_CASE("fxt::with applicative — pipeline composition with other adaptors", "[with][expected][applicative]")
{
    SECTION("with() result can be further piped with transform()")
    {
        auto add = [](double a, double b) { return a + b; };
        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{3.0})
            | fxt::with(fxt::expected<double, std::string>{4.0})
            | fxt::transform([](double x) { return x * x; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 49.0);
    }

    SECTION("with() result can be further piped with and_then()")
    {
        auto add = [](double a, double b) { return a + b; };
        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{3.0})
            | fxt::with(fxt::expected<double, std::string>{4.0})
            | fxt::and_then([](double x) -> fxt::expected<std::string, std::string> {
                if (x > 10.0) return fxt::unexpected<std::string>("too large");
                return "result=" + std::to_string(x);
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == "result=7.000000");
    }

    SECTION("with() error propagates through subsequent transform()")
    {
        auto add = [](double a, double b) { return a + b; };
        auto result = fxt::curry(add)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("validation failed")})
            | fxt::with(fxt::expected<double, std::string>{4.0})
            | fxt::transform([](double x) { return x * x; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "validation failed");
    }
}
