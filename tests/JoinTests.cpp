#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>
#include <type_traits>

TEST_CASE("fxt::join — flattens nested expected (call syntax)", "[join][expected][monad]")
{
    using Inner = fxt::expected<int, std::string>;
    using Nested = fxt::expected<Inner, std::string>;

    SECTION("outer value, inner value — flattened to value")
    {
        Nested nested{Inner{42}};
        auto result = fxt::join()(nested);

        static_assert(std::is_same_v<std::remove_cvref_t<decltype(result)>,
                                     fxt::expected<int, std::string>>,
                      "join must collapse expected<expected<T,E>,E> to expected<T,E>");
        REQUIRE(result.has_value());
        REQUIRE(*result == 42);
    }

    SECTION("outer value, inner error — inner error propagated")
    {
        Nested nested{Inner{fxt::unexpected<std::string>("inner failure")}};
        auto result = fxt::join()(nested);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "inner failure");
    }

    SECTION("outer error — outer error propagated")
    {
        Nested nested{fxt::unexpected<std::string>("outer failure")};
        auto result = fxt::join()(nested);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "outer failure");
    }
}

TEST_CASE("fxt::join — flattens nested expected (pipe syntax)", "[join][expected][monad]")
{
    using Inner = fxt::expected<std::string, std::string>;
    using Nested = fxt::expected<Inner, std::string>;

    SECTION("outer value, inner value — flattened to value")
    {
        Nested nested{Inner{"hello"}};
        auto result = nested | fxt::join();

        REQUIRE(result.has_value());
        REQUIRE(*result == "hello");
    }

    SECTION("outer value, inner error — inner error propagated")
    {
        Nested nested{Inner{fxt::unexpected<std::string>("inner bad")}};
        auto result = nested | fxt::join();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "inner bad");
    }

    SECTION("outer error — outer error propagated")
    {
        Nested nested{fxt::unexpected<std::string>("outer bad")};
        auto result = nested | fxt::join();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "outer bad");
    }
}

TEST_CASE("fxt::join — completes a curry/with applicative pipeline", "[join][expected][with][applicative]")
{
    // The primary intended use: a curried callable that itself returns
    // expected<R, E> produces expected<expected<R, E>, E> at the end of a
    // with() chain; join() collapses that into expected<R, E>.
    auto divide = [](double a, double b) -> fxt::expected<double, std::string> {
        if (b == 0.0) return fxt::unexpected<std::string>("division by zero");
        return a / b;
    };

    SECTION("all valid — flattened computed value")
    {
        auto result = fxt::curry(divide)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::join();

        static_assert(std::is_same_v<std::remove_cvref_t<decltype(result)>,
                                     fxt::expected<double, std::string>>,
                      "join must flatten the pipeline result to expected<double, string>");
        REQUIRE(result.has_value());
        REQUIRE(*result == 5.0);
    }

    SECTION("callable fails — inner error surfaces after join")
    {
        auto result = fxt::curry(divide)
            | fxt::with(fxt::expected<double, std::string>{10.0})
            | fxt::with(fxt::expected<double, std::string>{0.0})
            | fxt::join();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "division by zero");
    }

    SECTION("argument invalid — outer error short-circuits before callable runs")
    {
        auto result = fxt::curry(divide)
            | fxt::with(fxt::expected<double, std::string>{fxt::unexpected<std::string>("bad numerator")})
            | fxt::with(fxt::expected<double, std::string>{2.0})
            | fxt::join();

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "bad numerator");
    }
}

TEST_CASE("fxt::join — composes with other adaptors", "[join][expected][monad]")
{
    using Inner = fxt::expected<int, std::string>;
    using Nested = fxt::expected<Inner, std::string>;

    SECTION("join result can be further transformed")
    {
        Nested nested{Inner{20}};
        auto result = nested
            | fxt::join()
            | fxt::transform([](int x) { return x + 1; });

        REQUIRE(result.has_value());
        REQUIRE(*result == 21);
    }

    SECTION("join of an inner error propagates through a later transform")
    {
        Nested nested{Inner{fxt::unexpected<std::string>("boom")}};
        auto result = nested
            | fxt::join()
            | fxt::transform([](int x) { return x + 1; });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "boom");
    }
}

TEST_CASE("fxt::join — preserves value type, including move-only values", "[join][expected][monad]")
{
    SECTION("flattening a string value moves it through unchanged")
    {
        using Inner = fxt::expected<std::string, std::string>;
        using Nested = fxt::expected<Inner, std::string>;

        Nested nested{Inner{std::string(64, 'x')}};
        auto result = std::move(nested) | fxt::join();

        REQUIRE(result.has_value());
        REQUIRE(result->size() == 64);
    }
}
