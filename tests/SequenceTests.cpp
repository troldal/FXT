#include <catch_amalgamated.hpp>
#include <fxt.hpp>
#include <deque>
#include <list>
#include <set>
#include <string>
#include <vector>

// ============================================================================
// Helpers shared across test cases
// ============================================================================

using Err = std::string;

static fxt::expected<int, Err> parse_positive(int v)
{
    if (v <= 0) return fxt::unexpected<Err>("not positive: " + std::to_string(v));
    return v;
}

static fxt::expected<double, Err> to_double(int v)
{
    return static_cast<double>(v);
}

// ============================================================================
// fxt::sequence
// ============================================================================

TEST_CASE("fxt::sequence — all elements succeed", "[sequence][expected]")
{
    SECTION("vector: all values present")
    {
        std::vector<fxt::expected<int, Err>> input = {1, 2, 3, 4, 5};
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::vector<int>{1, 2, 3, 4, 5});
    }

    SECTION("list: all values present")
    {
        std::list<fxt::expected<int, Err>> input = {10, 20, 30};
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::list<int>{10, 20, 30});
    }

    SECTION("deque: all values present")
    {
        std::deque<fxt::expected<int, Err>> input = {7, 8, 9};
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::deque<int>{7, 8, 9});
    }
}

TEST_CASE("fxt::sequence — empty container", "[sequence][expected]")
{
    SECTION("vector: empty input -> expected<empty vector>")
    {
        std::vector<fxt::expected<int, Err>> input;
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(result->empty());
    }

    SECTION("list: empty input")
    {
        std::list<fxt::expected<int, Err>> input;
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(result->empty());
    }
}

TEST_CASE("fxt::sequence — error short-circuits", "[sequence][expected]")
{
    SECTION("first element is an error")
    {
        std::vector<fxt::expected<int, Err>> input = {
            fxt::unexpected<Err>("first bad"),
            fxt::unexpected<Err>("second bad"),
            42
        };
        auto result = fxt::sequence(input);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "first bad");
    }

    SECTION("error in the middle")
    {
        std::vector<fxt::expected<int, Err>> input = {
            1,
            2,
            fxt::unexpected<Err>("middle error"),
            4,
            5
        };
        auto result = fxt::sequence(input);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "middle error");
    }

    SECTION("last element is an error")
    {
        std::vector<fxt::expected<int, Err>> input = {
            1,
            2,
            3,
            fxt::unexpected<Err>("last error")
        };
        auto result = fxt::sequence(input);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "last error");
    }

    SECTION("first error wins when multiple errors present")
    {
        std::vector<fxt::expected<int, Err>> input = {
            fxt::unexpected<Err>("error A"),
            fxt::unexpected<Err>("error B"),
            fxt::unexpected<Err>("error C")
        };
        auto result = fxt::sequence(input);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "error A");
    }
}

TEST_CASE("fxt::sequence — rvalue container moves values", "[sequence][expected]")
{
    std::vector<fxt::expected<std::string, Err>> input = {
        std::string("hello"),
        std::string("world")
    };
    auto result = fxt::sequence(std::move(input));

    REQUIRE(result.has_value());
    REQUIRE(*result == std::vector<std::string>{"hello", "world"});
}

TEST_CASE("fxt::sequence — output container type matches input", "[sequence][expected]")
{
    SECTION("vector in -> vector out")
    {
        std::vector<fxt::expected<int, Err>> input = {1, 2, 3};
        auto result = fxt::sequence(input);
        STATIC_REQUIRE(std::is_same_v<decltype(result),
                                      fxt::expected<std::vector<int>, Err>>);
    }

    SECTION("list in -> list out")
    {
        std::list<fxt::expected<int, Err>> input = {1, 2};
        auto result = fxt::sequence(input);
        STATIC_REQUIRE(std::is_same_v<decltype(result),
                                      fxt::expected<std::list<int>, Err>>);
    }

    SECTION("deque in -> deque out")
    {
        std::deque<fxt::expected<int, Err>> input = {1};
        auto result = fxt::sequence(input);
        STATIC_REQUIRE(std::is_same_v<decltype(result),
                                      fxt::expected<std::deque<int>, Err>>);
    }
}

// ============================================================================
// fxt::traverse — free function form
// ============================================================================

TEST_CASE("fxt::traverse free function — all elements succeed", "[traverse][expected]")
{
    SECTION("vector<int> mapped to double")
    {
        std::vector<int> input = {1, 2, 3, 4};
        auto result = fxt::traverse(input, to_double);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::vector<double>{1.0, 2.0, 3.0, 4.0});
    }

    SECTION("vector<int> filtered by parse_positive")
    {
        std::vector<int> input = {1, 2, 3};
        auto result = fxt::traverse(input, parse_positive);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::vector<int>{1, 2, 3});
    }
}

TEST_CASE("fxt::traverse free function — empty container", "[traverse][expected]")
{
    std::vector<int> input;
    auto result = fxt::traverse(input, parse_positive);

    REQUIRE(result.has_value());
    REQUIRE(result->empty());
}

TEST_CASE("fxt::traverse free function — error short-circuits", "[traverse][expected]")
{
    SECTION("first element fails")
    {
        std::vector<int> input = {-1, 2, 3};
        auto result = fxt::traverse(input, parse_positive);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: -1");
    }

    SECTION("middle element fails")
    {
        std::vector<int> input = {1, -5, 3};
        auto result = fxt::traverse(input, parse_positive);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: -5");
    }

    SECTION("last element fails")
    {
        std::vector<int> input = {1, 2, 0};
        auto result = fxt::traverse(input, parse_positive);

        REQUIRE_FALSE(result.has_value());
        REQUIRE(result.error() == "not positive: 0");
    }
}

TEST_CASE("fxt::traverse free function — container types", "[traverse][expected]")
{
    SECTION("list")
    {
        std::list<int> input = {1, 2, 3};
        auto result = fxt::traverse(input, parse_positive);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::list<int>{1, 2, 3});
    }

    SECTION("deque")
    {
        std::deque<int> input = {4, 5, 6};
        auto result = fxt::traverse(input, parse_positive);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::deque<int>{4, 5, 6});
    }
}

TEST_CASE("fxt::traverse free function — lambda mapping", "[traverse][expected]")
{
    std::vector<std::string> input = {"1", "2", "3"};

    auto parse_int = [](const std::string& s) -> fxt::expected<int, Err> {
        try { return std::stoi(s); }
        catch (...) { return fxt::unexpected<Err>("parse error: " + s); }
    };

    auto result = fxt::traverse(input, parse_int);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::vector<int>{1, 2, 3});
}

TEST_CASE("fxt::traverse free function — lambda mapping fails", "[traverse][expected]")
{
    std::vector<std::string> input = {"1", "abc", "3"};

    auto parse_int = [](const std::string& s) -> fxt::expected<int, Err> {
        try { return std::stoi(s); }
        catch (...) { return fxt::unexpected<Err>("parse error: " + s); }
    };

    auto result = fxt::traverse(input, parse_int);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "parse error: abc");
}

// ============================================================================
// fxt::traverse — pipe adaptor form
// ============================================================================

TEST_CASE("fxt::traverse pipe adaptor — all elements succeed", "[traverse][pipe][expected]")
{
    std::vector<int> input = {1, 2, 3, 4, 5};
    auto result = input | fxt::traverse(parse_positive);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::vector<int>{1, 2, 3, 4, 5});
}

TEST_CASE("fxt::traverse pipe adaptor — error short-circuits", "[traverse][pipe][expected]")
{
    std::vector<int> input = {1, 2, -3, 4};
    auto result = input | fxt::traverse(parse_positive);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error() == "not positive: -3");
}

TEST_CASE("fxt::traverse pipe adaptor — empty container", "[traverse][pipe][expected]")
{
    std::vector<int> input;
    auto result = input | fxt::traverse(parse_positive);

    REQUIRE(result.has_value());
    REQUIRE(result->empty());
}

TEST_CASE("fxt::traverse pipe adaptor — container types", "[traverse][pipe][expected]")
{
    SECTION("list")
    {
        std::list<int> input = {1, 2, 3};
        auto result = input | fxt::traverse(to_double);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::list<double>{1.0, 2.0, 3.0});
    }

    SECTION("deque")
    {
        std::deque<int> input = {1, 2, 3};
        auto result = input | fxt::traverse(to_double);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::deque<double>{1.0, 2.0, 3.0});
    }
}

TEST_CASE("fxt::traverse pipe adaptor — stored adaptor is reusable", "[traverse][pipe][expected]")
{
    auto validate = fxt::traverse(parse_positive);

    std::vector<int> good = {1, 2, 3};
    std::vector<int> bad  = {1, -2, 3};

    auto r1 = good | validate;
    auto r2 = bad  | validate;

    REQUIRE(r1.has_value());
    REQUIRE_FALSE(r2.has_value());
}

// ============================================================================
// traverse + sequence equivalence
// ============================================================================

TEST_CASE("traverse and sequence are equivalent", "[traverse][sequence][expected]")
{
    std::vector<int> input = {1, 2, 3, 4, 5};

    // traverse(input, f) == sequence(input mapped with f)
    std::vector<fxt::expected<int, Err>> mapped;
    for (auto x : input) mapped.push_back(parse_positive(x));

    auto via_sequence = fxt::sequence(mapped);
    auto via_traverse = fxt::traverse(input, parse_positive);

    REQUIRE(via_sequence.has_value());
    REQUIRE(via_traverse.has_value());
    REQUIRE(*via_sequence == *via_traverse);
}

// ============================================================================
// fxt::sequence — optional overloads (container)
// ============================================================================

TEST_CASE("fxt::sequence — optional, all elements present", "[sequence][optional]")
{
    SECTION("vector: all values present")
    {
        std::vector<fxt::optional<int>> input = {1, 2, 3};
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::vector<int>{1, 2, 3});
    }

    SECTION("list: all values present")
    {
        std::list<fxt::optional<int>> input = {10, 20};
        auto result = fxt::sequence(input);

        REQUIRE(result.has_value());
        REQUIRE(*result == std::list<int>{10, 20});
    }
}

TEST_CASE("fxt::sequence — optional, empty container", "[sequence][optional]")
{
    std::vector<fxt::optional<int>> input;
    auto result = fxt::sequence(input);

    REQUIRE(result.has_value());
    REQUIRE(result->empty());
}

TEST_CASE("fxt::sequence — optional, absent element short-circuits", "[sequence][optional]")
{
    SECTION("first element absent")
    {
        std::vector<fxt::optional<int>> input = {fxt::nullopt, 2, 3};
        auto result = fxt::sequence(input);
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("middle element absent")
    {
        std::vector<fxt::optional<int>> input = {1, fxt::nullopt, 3};
        auto result = fxt::sequence(input);
        REQUIRE_FALSE(result.has_value());
    }

    SECTION("last element absent")
    {
        std::vector<fxt::optional<int>> input = {1, 2, fxt::nullopt};
        auto result = fxt::sequence(input);
        REQUIRE_FALSE(result.has_value());
    }
}

TEST_CASE("fxt::sequence — optional, result type matches input container", "[sequence][optional]")
{
    std::vector<fxt::optional<int>> input = {1, 2};
    auto result = fxt::sequence(input);
    STATIC_REQUIRE(std::is_same_v<decltype(result), fxt::optional<std::vector<int>>>);
}

TEST_CASE("fxt::sequence — optional, set container (insert-based)", "[sequence][optional]")
{
    std::set<fxt::optional<int>> input = {1, 2, 3};
    auto result = fxt::sequence(input);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::set<int>{1, 2, 3});
}

// ============================================================================
// fxt::traverse — optional overloads (container)
// ============================================================================

static fxt::optional<int> maybe_positive(int v)
{
    if (v <= 0) return fxt::nullopt;
    return v;
}

TEST_CASE("fxt::traverse — optional, free function, all succeed", "[traverse][optional]")
{
    std::vector<int> input = {1, 2, 3};
    auto result = fxt::traverse(input, maybe_positive);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::vector<int>{1, 2, 3});
}

TEST_CASE("fxt::traverse — optional, free function, absent short-circuits", "[traverse][optional]")
{
    SECTION("first element fails")
    {
        std::vector<int> input = {-1, 2, 3};
        REQUIRE_FALSE(fxt::traverse(input, maybe_positive).has_value());
    }

    SECTION("middle element fails")
    {
        std::vector<int> input = {1, 0, 3};
        REQUIRE_FALSE(fxt::traverse(input, maybe_positive).has_value());
    }
}

TEST_CASE("fxt::traverse — optional, pipe adaptor", "[traverse][optional][pipe]")
{
    std::vector<int> input = {1, 2, 3};
    auto result = input | fxt::traverse(maybe_positive);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::vector<int>{1, 2, 3});
}

TEST_CASE("fxt::traverse — optional, pipe adaptor, empty container", "[traverse][optional][pipe]")
{
    std::vector<int> input;
    auto result = input | fxt::traverse(maybe_positive);

    REQUIRE(result.has_value());
    REQUIRE(result->empty());
}

TEST_CASE("fxt::traverse — optional, set container (insert-based)", "[traverse][optional]")
{
    std::set<int> input = {1, 2, 3};
    auto result = fxt::traverse(input, maybe_positive);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::set<int>{1, 2, 3});
}

// ============================================================================
// fxt::traverse — expected with set (insert-based container)
// ============================================================================

TEST_CASE("fxt::traverse — expected, set container (insert-based)", "[traverse][expected]")
{
    std::set<int> input = {1, 2, 3};
    auto result = fxt::traverse(input, parse_positive);

    REQUIRE(result.has_value());
    REQUIRE(*result == std::set<int>{1, 2, 3});
}

TEST_CASE("fxt::traverse — expected, set container, error short-circuits", "[traverse][expected]")
{
    std::set<int> input = {-1, 2, 3};
    auto result = fxt::traverse(input, parse_positive);
    REQUIRE_FALSE(result.has_value());
}
