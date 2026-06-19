#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <string>

// ============================================================================
// Helpers
// ============================================================================

using Err = std::string;

static fxt::expected<int, Err> validate_int(int v)
{
    if (v <= 0) return fxt::unexpected<Err>("not positive: " + std::to_string(v));
    return v;
}

static fxt::expected<double, Err> validate_double(double v)
{
    if (v <= 0.0) return fxt::unexpected<Err>("not positive: " + std::to_string(v));
    return v;
}

// ============================================================================
// fxt::sequence for fxt::tuple
// ============================================================================

TEST_CASE("fxt::sequence for tuple — all elements succeed", "[sequence][tuple][expected]")
{
    SECTION("homogeneous: tuple<expected<int,Err>, expected<int,Err>>")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int, Err>{1},
            fxt::expected<int, Err>{2},
            fxt::expected<int, Err>{3}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
    }

    SECTION("heterogeneous: tuple<expected<int,Err>, expected<string,Err>, expected<double,Err>>")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int,         Err>{42},
            fxt::expected<std::string, Err>{"hello"},
            fxt::expected<double,      Err>{3.14}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "hello");
        REQUIRE(std::get<2>(*result) == Catch::Approx(3.14));
    }

    SECTION("result type is exactly expected<tuple<int, string, double>, Err>")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int,         Err>{1},
            fxt::expected<std::string, Err>{"x"},
            fxt::expected<double,      Err>{1.0}
        );
        auto result = fxt::sequence(std::move(t));
        STATIC_REQUIRE(std::is_same_v<
            decltype(result),
            fxt::expected<fxt::tuple<int, std::string, double>, Err>
        >);
    }
}

TEST_CASE("fxt::sequence for tuple — error short-circuits", "[sequence][tuple][expected]")
{
    SECTION("first element is an error")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int, Err>{fxt::unexpected<Err>("first bad")},
            fxt::expected<int, Err>{2},
            fxt::expected<int, Err>{3}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "first bad");
    }

    SECTION("middle element is an error")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int,         Err>{1},
            fxt::expected<std::string, Err>{fxt::unexpected<Err>("middle bad")},
            fxt::expected<double,      Err>{3.14}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "middle bad");
    }

    SECTION("last element is an error")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int, Err>{1},
            fxt::expected<int, Err>{2},
            fxt::expected<int, Err>{fxt::unexpected<Err>("last bad")}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "last bad");
    }

    SECTION("first error wins when multiple elements are errors")
    {
        auto t = fxt::make_tuple(
            fxt::expected<int, Err>{fxt::unexpected<Err>("error A")},
            fxt::expected<int, Err>{fxt::unexpected<Err>("error B")},
            fxt::expected<int, Err>{fxt::unexpected<Err>("error C")}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error A");
    }
}

TEST_CASE("fxt::sequence for tuple — single element", "[sequence][tuple][expected]")
{
    SECTION("single valid element")
    {
        auto t = fxt::make_tuple(fxt::expected<int, Err>{99});
        auto result = fxt::sequence(std::move(t));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 99);
    }

    SECTION("single error element")
    {
        auto t = fxt::make_tuple(fxt::expected<int, Err>{fxt::unexpected<Err>("solo error")});
        auto result = fxt::sequence(std::move(t));

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "solo error");
    }
}

// Note on empty tuple: fxt::sequence cannot be called on an empty tuple because
// the error type E cannot be deduced from zero elements. This is an expected
// limitation — the function signature uses pattern matching on
// fxt::tuple<fxt::expected<Ts, E>...>, and with Ts... empty there is no E to
// deduce. In practice, traversing/sequencing an empty tuple of expectations is
// a degenerate case that does not arise in real usage.

TEST_CASE("fxt::sequence for tuple — composes with and_then", "[sequence][tuple][expected]")
{
    SECTION("sequence then apply computation")
    {
        auto args = fxt::make_tuple(
            validate_int(3),
            validate_int(4)
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::and_then([](auto validated) -> fxt::expected<int, Err> {
                return std::get<0>(validated) + std::get<1>(validated);
            });

        REQUIRE(result.has_value());
        REQUIRE(*result == 7);
    }

    SECTION("sequence with error propagates through and_then")
    {
        auto args = fxt::make_tuple(
            validate_int(3),
            validate_int(-1)
        );

        auto result = fxt::sequence(std::move(args))
            | fxt::and_then([](auto validated) -> fxt::expected<int, Err> {
                return std::get<0>(validated) + std::get<1>(validated);
            });

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: -1");
    }
}

// ============================================================================
// fxt::traverse for fxt::tuple — free function form
// ============================================================================

TEST_CASE("fxt::traverse for tuple — free function, all succeed", "[traverse][tuple][expected]")
{
    SECTION("homogeneous tuple, single function")
    {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::traverse(t, validate_int);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
    }

    SECTION("homogeneous doubles")
    {
        auto t = fxt::make_tuple(1.5, 2.5, 3.5);
        auto result = fxt::traverse(t, validate_double);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == Catch::Approx(1.5));
        REQUIRE(std::get<1>(*result) == Catch::Approx(2.5));
        REQUIRE(std::get<2>(*result) == Catch::Approx(3.5));
    }
}

TEST_CASE("fxt::traverse for tuple — free function, error short-circuits", "[traverse][tuple][expected]")
{
    SECTION("first element fails")
    {
        auto t = fxt::make_tuple(-1, 2, 3);
        auto result = fxt::traverse(t, validate_int);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: -1");
    }

    SECTION("middle element fails")
    {
        auto t = fxt::make_tuple(1, -5, 3);
        auto result = fxt::traverse(t, validate_int);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: -5");
    }

    SECTION("last element fails")
    {
        auto t = fxt::make_tuple(1, 2, 0);
        auto result = fxt::traverse(t, validate_int);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: 0");
    }
}

TEST_CASE("fxt::traverse for tuple — heterogeneous input types", "[traverse][tuple][expected]")
{
    // A polymorphic lambda that validates any numeric type
    auto validate_positive = [](auto v) -> fxt::expected<std::decay_t<decltype(v)>, Err> {
        if (v <= 0) return fxt::unexpected<Err>("non-positive");
        return v;
    };

    SECTION("int and double elements, all valid")
    {
        auto t = fxt::make_tuple(5, 3.14);
        auto result = fxt::traverse(t, validate_positive);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 5);
        REQUIRE(std::get<1>(*result) == Catch::Approx(3.14));
    }

    SECTION("result type correctly deduced")
    {
        auto t = fxt::make_tuple(1, 2.0, 3);
        auto result = fxt::traverse(t, validate_positive);
        STATIC_REQUIRE(std::is_same_v<
            decltype(result),
            fxt::expected<fxt::tuple<int, double, int>, Err>
        >);
    }

    SECTION("heterogeneous input, error in double element")
    {
        auto t = fxt::make_tuple(5, -3.14);
        auto result = fxt::traverse(t, validate_positive);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "non-positive");
    }
}

// ============================================================================
// fxt::traverse for fxt::tuple — pipe adaptor form
// ============================================================================

TEST_CASE("fxt::traverse for tuple — pipe adaptor, all succeed", "[traverse][tuple][pipe][expected]")
{
    auto t = fxt::make_tuple(1, 2, 3);
    auto result = t | fxt::traverse(validate_int);

    REQUIRE(result.has_value());
    REQUIRE(std::get<0>(*result) == 1);
    REQUIRE(std::get<1>(*result) == 2);
    REQUIRE(std::get<2>(*result) == 3);
}

TEST_CASE("fxt::traverse for tuple — pipe adaptor, error", "[traverse][tuple][pipe][expected]")
{
    auto t = fxt::make_tuple(1, -2, 3);
    auto result = t | fxt::traverse(validate_int);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "not positive: -2");
}

TEST_CASE("fxt::traverse for tuple — pipe adaptor, stored adaptor is reusable", "[traverse][tuple][pipe][expected]")
{
    auto validate = fxt::traverse(validate_int);

    auto good = fxt::make_tuple(1, 2, 3);
    auto bad  = fxt::make_tuple(1, -2, 3);

    auto r1 = good | validate;
    auto r2 = bad  | validate;

    REQUIRE(r1.has_value());
    REQUIRE_FALSE(r2.has_value());
}

TEST_CASE("fxt::traverse for tuple — pipe, heterogeneous polymorphic lambda", "[traverse][tuple][pipe][expected]")
{
    auto validate_pos = [](auto v) -> fxt::expected<std::decay_t<decltype(v)>, Err> {
        if (v <= 0) return fxt::unexpected<Err>("non-positive");
        return v;
    };

    auto t = fxt::make_tuple(42, 3.14, 7);
    auto result = t | fxt::traverse(validate_pos);

    REQUIRE(result.has_value());
    REQUIRE(std::get<0>(*result) == 42);
    REQUIRE(std::get<1>(*result) == Catch::Approx(3.14));
    REQUIRE(std::get<2>(*result) == 7);
}

// ============================================================================
// Mismatched error type — static failure
//
// The following code must NOT compile:
//
//   auto t = fxt::make_tuple(
//       fxt::expected<int,    std::string>{1},
//       fxt::expected<double, int        >{2.0}   // different error type
//   );
//   fxt::sequence(std::move(t));  // COMPILE ERROR: E can't be deduced
//
// The function signature template<typename... Ts, typename E>
// sequence(fxt::tuple<fxt::expected<Ts, E>...>) uses a single E for all elements.
// If error types differ, template argument deduction fails and there is no match.
// ============================================================================

// ============================================================================
// fxt::sequence for fxt::tuple — optional overloads
// ============================================================================

TEST_CASE("fxt::sequence for tuple — optional, all elements present", "[sequence][tuple][optional]")
{
    SECTION("homogeneous: tuple<optional<int>, optional<int>>")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{1},
            fxt::optional<int>{2},
            fxt::optional<int>{3}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
    }

    SECTION("heterogeneous: tuple<optional<int>, optional<string>, optional<double>>")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{42},
            fxt::optional<std::string>{"hello"},
            fxt::optional<double>{3.14}
        );
        auto result = fxt::sequence(std::move(t));

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 42);
        REQUIRE(std::get<1>(*result) == "hello");
        REQUIRE(std::get<2>(*result) == Catch::Approx(3.14));
    }

    SECTION("result type is exactly optional<tuple<int, string, double>>")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{1},
            fxt::optional<std::string>{"x"},
            fxt::optional<double>{1.0}
        );
        auto result = fxt::sequence(std::move(t));
        STATIC_REQUIRE(std::is_same_v<
            decltype(result),
            fxt::optional<fxt::tuple<int, std::string, double>>
        >);
    }
}

TEST_CASE("fxt::sequence for tuple — optional, absent element short-circuits", "[sequence][tuple][optional]")
{
    SECTION("first element absent")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{fxt::nullopt},
            fxt::optional<int>{2},
            fxt::optional<int>{3}
        );
        auto result = fxt::sequence(std::move(t));
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("middle element absent")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{1},
            fxt::optional<int>{fxt::nullopt},
            fxt::optional<int>{3}
        );
        auto result = fxt::sequence(std::move(t));
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("last element absent")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{1},
            fxt::optional<int>{2},
            fxt::optional<int>{fxt::nullopt}
        );
        auto result = fxt::sequence(std::move(t));
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("first absent wins when multiple elements are absent")
    {
        auto t = fxt::make_tuple(
            fxt::optional<int>{fxt::nullopt},
            fxt::optional<int>{fxt::nullopt},
            fxt::optional<int>{fxt::nullopt}
        );
        auto result = fxt::sequence(std::move(t));
        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("fxt::sequence for tuple — optional, single element", "[sequence][tuple][optional]")
{
    SECTION("single present element")
    {
        auto t = fxt::make_tuple(fxt::optional<int>{99});
        auto result = fxt::sequence(std::move(t));
        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 99);
    }

    SECTION("single absent element")
    {
        auto t = fxt::make_tuple(fxt::optional<int>{fxt::nullopt});
        auto result = fxt::sequence(std::move(t));
        REQUIRE_FALSE(result.has_value());
    }
}

// ============================================================================
// fxt::traverse for fxt::tuple — optional overloads
// ============================================================================

static fxt::optional<int> maybe_positive(int v)
{
    if (v <= 0) return fxt::nullopt;
    return v;
}

static fxt::optional<double> maybe_positive_d(double v)
{
    if (v <= 0.0) return fxt::nullopt;
    return v;
}

TEST_CASE("fxt::traverse for tuple — optional, free function, all succeed", "[traverse][tuple][optional]")
{
    SECTION("homogeneous tuple")
    {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::traverse(t, maybe_positive);

        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 1);
        REQUIRE(std::get<1>(*result) == 2);
        REQUIRE(std::get<2>(*result) == 3);
    }

    SECTION("result type correctly deduced")
    {
        auto t = fxt::make_tuple(1, 2, 3);
        auto result = fxt::traverse(t, maybe_positive);
        STATIC_REQUIRE(std::is_same_v<
            decltype(result),
            fxt::optional<fxt::tuple<int, int, int>>
        >);
    }
}

TEST_CASE("fxt::traverse for tuple — optional, absent short-circuits", "[traverse][tuple][optional]")
{
    SECTION("first element fails")
    {
        auto t = fxt::make_tuple(-1, 2, 3);
        REQUIRE_FALSE(fxt::traverse(t, maybe_positive).has_value());
    }

    SECTION("middle element fails")
    {
        auto t = fxt::make_tuple(1, 0, 3);
        REQUIRE_FALSE(fxt::traverse(t, maybe_positive).has_value());
    }

    SECTION("last element fails")
    {
        auto t = fxt::make_tuple(1, 2, -1);
        REQUIRE_FALSE(fxt::traverse(t, maybe_positive).has_value());
    }
}

TEST_CASE("fxt::traverse for tuple — optional, pipe adaptor", "[traverse][tuple][optional][pipe]")
{
    auto t = fxt::make_tuple(1, 2, 3);
    auto result = t | fxt::traverse(maybe_positive);

    REQUIRE(result.has_value());
    REQUIRE(std::get<0>(*result) == 1);
    REQUIRE(std::get<1>(*result) == 2);
    REQUIRE(std::get<2>(*result) == 3);
}

TEST_CASE("fxt::traverse for tuple — optional, pipe adaptor, absent", "[traverse][tuple][optional][pipe]")
{
    auto t = fxt::make_tuple(1, -2, 3);
    REQUIRE_FALSE((t | fxt::traverse(maybe_positive)).has_value());
}

TEST_CASE("fxt::traverse for tuple — optional, heterogeneous polymorphic lambda", "[traverse][tuple][optional]")
{
    auto maybe_pos = [](auto v) -> fxt::optional<std::decay_t<decltype(v)>> {
        if (v <= 0) return fxt::nullopt;
        return v;
    };

    SECTION("all succeed")
    {
        auto t = fxt::make_tuple(5, 3.14);
        auto result = fxt::traverse(t, maybe_pos);
        REQUIRE(result.has_value());
        REQUIRE(std::get<0>(*result) == 5);
        REQUIRE(std::get<1>(*result) == Catch::Approx(3.14));
    }

    SECTION("one fails")
    {
        auto t = fxt::make_tuple(5, -3.14);
        REQUIRE_FALSE(fxt::traverse(t, maybe_pos).has_value());
    }
}
